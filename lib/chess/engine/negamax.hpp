#pragma once

#include <algorithm>

#include <lib/chess/util.hpp>
#include <lib/chess/gamestate.hpp>
#include <lib/chess/move.hpp>
#include <lib/chess/movegenerator.hpp>
#include <lib/chess/unmove.hpp>

#include <lib/chess/engine/evaluation.hpp>
#include <lib/chess/engine/transpositiontable.hpp>

namespace Chess::Engine::Negamax {

    static const int mvv_lva[6][6] = {
        // Attacker:  PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
        {105, 205, 305, 405, 505, 605}, // Victim PAWN
        {104, 204, 304, 404, 504, 604}, // Victim KNIGHT
        {103, 203, 303, 403, 503, 603}, // Victim BISHOP
        {102, 202, 302, 402, 502, 602}, // Victim ROOK
        {101, 201, 301, 401, 501, 601}, // Victim QUEEN
        {100, 200, 300, 400, 500, 600}  // Victim KING
    };
    int captureScore(const GameState & gs, const Move & m) {
        if (!m.isCapture()) return 0;
        // Map CAPTURE enum to piece index (0 = PAWN, ..., 5 = KING)
        int victim = static_cast<int>(gs.isMoveEnPassant(m) ? PAWN : gs.getPieceTypeAtSquare(m.to(), (COLOR) !gs.turn));
        int attacker = static_cast<int>(gs.getPieceTypeAtSquare(m.from(), gs.turn));
        return mvv_lva[victim][attacker];
    }
    void orderMoves(const GameState & gs, Move * moves, int moves_c) {
        std::sort(moves, moves + moves_c, [gs](const Move & a, const Move & b) {
            int sa = captureScore(gs, a);
            int sb = captureScore(gs, b);
            return sa > sb; // higher score = earlier
        });
    }
    
    constexpr I16 EVAL_INF = 30000;
    constexpr I16 MATE = 20000;
    constexpr I16 MATE_THRESHOLD = MATE - 2048;


    // temp counters
    static int qcnt = 0;
    static int ncnt = 0;

    int quiescence(GameState & gs, TranspositionTable & tt, int alpha, int beta, int ply) {
        /* temp */ qcnt++;

        // transposition table hit check
        TranspositionTable::EntryData hit = tt.find_entry(gs.getHashCode());
        if (hit.is_valid()) {
            int hit_score = hit.score;
            if (hit_score >  MATE_THRESHOLD) hit_score -= ply;   // convert back to node-local POV
            if (hit_score < -MATE_THRESHOLD) hit_score += ply;

            switch (hit.type) {
                case TranspositionTable::Node::EXACT: return hit_score;
                case TranspositionTable::Node::LOWERBOUND:
                    if(hit_score > alpha) alpha = hit_score;
                    break;
                case TranspositionTable::Node::UPPERBOUND:
                    if (hit_score < beta) beta = hit_score;
                    break;
            }

            if (alpha >= beta) return hit_score;
        }

        
        bool in_check = MoveGenerator::isInCheck(gs);
        int moves_c;
        Move moves[256];
        if (in_check) {
            moves_c = MoveGenerator::genAllMoves(gs, moves, in_check, false); 
            if (moves_c == 0) return -MATE + ply;
        } else {
            // else not in check
            int static_eval = Evaluation::staticEvaluation(gs);
            if (gs.turn == BLACK) static_eval = -static_eval;
            
            if (static_eval >= beta) return static_eval;
            if (static_eval > alpha) alpha = static_eval;

            moves_c = MoveGenerator::genAllMoves(gs, moves, in_check, true); 
            if (moves_c == 0) {
                return in_check ? -MATE + ply : alpha; // checkmate / draw
            }
        }

        orderMoves(gs, moves, moves_c);

        // try to order best move first
        if (hit.is_valid()) {
            for (int i = 0; i < moves_c; i++) {
                if (moves[i].v == hit.best_move.v) {
                    std::swap(moves[0], moves[i]);
                    break;
                }
            }
        }

        const int ORIG_ALPHA = alpha;
        int best_score = -EVAL_INF;
        Move best_move;
        for (int i = 0; i < moves_c; i++) {
            Move move = moves[i];
            Unmove unmove = gs.applyMove(move);

            int score = -quiescence(gs, tt, -beta, -alpha, ply+1);

            gs.applyUnmove(unmove);

            if (score > best_score) {
                best_score = score;
                best_move = move;
            }
            if (score > alpha) {
                alpha = score;
                if (alpha >= beta) break;
            }
        }

        // store results to TT
        TranspositionTable::Node::Type bound;
        if (best_score <= ORIG_ALPHA) bound = TranspositionTable::Node::UPPERBOUND; // fail-low
        else if (best_score >= beta)  bound = TranspositionTable::Node::LOWERBOUND; // fail-high
        else                          bound = TranspositionTable::Node::EXACT;      // PV node

        int best_score_mate_adjusted = best_score;
        if (best_score_mate_adjusted >  MATE_THRESHOLD) best_score_mate_adjusted += ply;
        if (best_score_mate_adjusted < -MATE_THRESHOLD) best_score_mate_adjusted -= ply;
        tt.add_entry(gs.getHashCode(), best_move, best_score_mate_adjusted, 0, bound);

        return best_score;
    }



    int negamax(GameState & gs, TranspositionTable & tt, int depth, int alpha, int beta, int ply = 0) {
        /* temp */ ncnt++;

        if (depth == 0) {
            return quiescence(gs, tt, alpha, beta, ply);
        }

        // transposition table hit check
        TranspositionTable::EntryData hit = tt.find_entry(gs.getHashCode());
        if (hit.is_valid() && hit.depth >= depth) {
            int hit_score = hit.score;
            if (hit_score >  MATE_THRESHOLD) hit_score -= ply;   // convert back to node-local POV
            if (hit_score < -MATE_THRESHOLD) hit_score += ply;

            switch (hit.type) {
                case TranspositionTable::Node::EXACT: return hit_score;
                case TranspositionTable::Node::LOWERBOUND:
                    if(hit_score > alpha) alpha = hit_score;
                    break;
                case TranspositionTable::Node::UPPERBOUND:
                    if (hit_score < beta) beta = hit_score;
                    break;
            }

            if (alpha >= beta) return hit_score;
        }

        bool in_check;
        Move moves[256];
        int moves_c = MoveGenerator::genAllMoves(gs, moves, in_check);

        if (moves_c == 0) {
            return in_check ? -MATE + ply : 0; // checkmate / draw
        }

        orderMoves(gs, moves, moves_c);

        // try to order best move first
        // if (hit.is_valid()) {
        //     for (int i = 0; i < moves_c; i++) {
        //         if (moves[i].v == hit.best_move.v) {
        //             std::swap(moves[0], moves[i]);
        //             break;
        //         }
        //     }
        // }

        const int ORIG_ALPHA = alpha;
        int best_score = -EVAL_INF;
        Move best_move;
        for (int i = 0; i < moves_c; i++) {
            Move move = moves[i];
            Unmove unmove = gs.applyMove(move);

            int score = -negamax(gs, tt, depth-1, -beta, -alpha, ply+1);

            gs.applyUnmove(unmove);

            if (score > best_score) {
                best_score = score;
                best_move = move;
            }
            if (score > alpha) {
                alpha = score;
                if (alpha >= beta) break;
            }
        }

        // store results to TT
        TranspositionTable::Node::Type bound;
        if (best_score <= ORIG_ALPHA) bound = TranspositionTable::Node::UPPERBOUND; // fail-low
        else if (best_score >= beta)  bound = TranspositionTable::Node::LOWERBOUND; // fail-high
        else                          bound = TranspositionTable::Node::EXACT;      // PV node

        int best_score_mate_adjusted = best_score;
        if (best_score_mate_adjusted >  MATE_THRESHOLD) best_score_mate_adjusted += ply;
        if (best_score_mate_adjusted < -MATE_THRESHOLD) best_score_mate_adjusted -= ply;
        tt.add_entry(gs.getHashCode(), best_move, best_score_mate_adjusted, depth, bound);

        return best_score;
    }


}