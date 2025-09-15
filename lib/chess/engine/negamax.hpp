#pragma once

#include <algorithm>

#include <lib/chess/util.hpp>
#include <lib/chess/gamestate.hpp>
#include <lib/chess/move.hpp>
#include <lib/chess/movegenerator.hpp>
#include <lib/chess/unmove.hpp>

#include <lib/chess/engine/staticevaluation.hpp>
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
    int captureScore(const GameState & gs, const Move & m, const Move & best_move) {
        if (!m.isCapture()) return 0;
        if (m.v == best_move.v) return 1024;
        // Map CAPTURE enum to piece index (0 = PAWN, ..., 5 = KING)
        int victim = static_cast<int>(gs.isMoveEnPassant(m) ? PAWN : gs.getPieceTypeAtSquare(m.to(), (COLOR) !gs.turn));
        int attacker = static_cast<int>(gs.getPieceTypeAtSquare(m.from(), gs.turn));
        return mvv_lva[victim][attacker];
    }
    void orderMoves(const GameState & gs, Move * moves, int moves_c, const Move & best_move) {
        std::sort(moves, moves + moves_c, [gs, best_move](const Move & a, const Move & b) {
            int sa = captureScore(gs, a, best_move);
            int sb = captureScore(gs, b, best_move);
            return sa > sb; // higher score = earlier
        });
    }
    
    constexpr I16 EVAL_INF = 30000;
    constexpr I16 MATE = 20000;
    constexpr I16 MATE_THRESHOLD = MATE - 2048;


    // temp counters
    static int qcnt = 0;
    static int ncnt = 0;
    static int tthit = 0;
    static int ttcut = 0;

    int quiescence(GameState & gs, TranspositionTable & tt, int alpha, int beta, int ply) {
        /* temp */ qcnt++;

        // transposition table hit check
        TranspositionTable::Node hit = tt.findNode(gs.getHashCode());
        if (hit.is_valid()) {
            tthit++;
            int hit_score = hit.score;
            if (hit_score >  MATE_THRESHOLD) hit_score -= ply;   // convert back to node-local POV
            if (hit_score < -MATE_THRESHOLD) hit_score += ply;

            switch (hit.type) {
                case TranspositionTable::Node::EXACT: 
                    ttcut++;
                    return hit_score;
                case TranspositionTable::Node::LOWERBOUND:
                    if(hit_score > alpha) alpha = hit_score;
                    break;
                case TranspositionTable::Node::UPPERBOUND:
                    if (hit_score < beta) beta = hit_score;
                    break;
            }

            if (alpha >= beta) return hit_score;
        }

        
        int moves_c;
        Move moves[256];
        MoveGenerator::PreMoveData pre_move_data = MoveGenerator::genPreMoveData(gs);
        if (pre_move_data.isCheck()) {
            moves_c = MoveGenerator::genAllMoves(gs, pre_move_data, moves, false); 
            if (moves_c == 0) return -MATE + ply;
        } else {
            // else not in check
            int static_eval = StaticEvaluation::staticEvaluation(gs, pre_move_data, alpha, beta);
            if (gs.turn == BLACK) static_eval = -static_eval;
            
            if (static_eval >= beta) return static_eval;
            if (static_eval > alpha) alpha = static_eval;

            moves_c = MoveGenerator::genAllMoves(gs, pre_move_data, moves, true); 
            if (moves_c == 0) {
                return pre_move_data.isCheck() ? -MATE + ply : alpha; // checkmate / draw
            }
        }

        orderMoves(gs, moves, moves_c, hit.best_move);

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
        TranspositionTable::Node hit = tt.findNode(gs.getHashCode());
        if (hit.is_valid() && hit.depth >= depth) {
            tthit++;
            int hit_score = hit.score;
            if (hit_score >  MATE_THRESHOLD) hit_score -= ply;   // convert back to node-local POV
            if (hit_score < -MATE_THRESHOLD) hit_score += ply;

            switch (hit.type) {
                case TranspositionTable::Node::EXACT: 
                    ttcut++;
                    return hit_score;
                case TranspositionTable::Node::LOWERBOUND:
                    if (hit_score > alpha) alpha = hit_score;
                    break;
                case TranspositionTable::Node::UPPERBOUND:
                    if (hit_score < beta) beta = hit_score;
                    break;
            }

            if (alpha >= beta) {
                ttcut++;
                return hit_score;
            }
        }

        Move moves[256];
        MoveGenerator::PreMoveData pre_move_data = MoveGenerator::genPreMoveData(gs);
        int moves_c = MoveGenerator::genAllMoves(gs, pre_move_data, moves);

        if (moves_c == 0) {
            return pre_move_data.isCheck() ? -MATE + ply : 0; // checkmate / draw
        }

        orderMoves(gs, moves, moves_c, hit.best_move);

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