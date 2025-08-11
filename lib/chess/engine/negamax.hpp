#pragma once

#include <algorithm>

#include <lib/chess/util.hpp>
#include <lib/chess/gamestate.hpp>
#include <lib/chess/move.hpp>
#include <lib/chess/movegenerator.hpp>
#include <lib/chess/unmove.hpp>

#include <lib/chess/engine/evaluation.hpp>

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


    int quiescence(GameState& gs, int alpha, int beta, int ply) {
        bool in_check = MoveGenerator::isInCheck(gs);

        Move moves[256];
        if (in_check) {
            int moves_c = MoveGenerator::genAllMoves(gs, moves, in_check, false); 
            if (moves_c == 0) return -MATE + ply;

            orderMoves(gs, moves, moves_c);
            for (int i = 0; i < moves_c; i++) {
                Move move = moves[i];

                Unmove unmove = gs.applyMove(move);

                /* temp */ qcnt++;

                int score = -quiescence(gs, -beta, -alpha, ply+1);
                gs.applyUnmove(unmove);

                if (score > alpha) alpha = score;
                if (score >= beta) break;
            }

            return alpha;
        } 

        // else not in check
        int static_eval = Evaluation::staticEvaluation(gs);
        if (gs.turn == BLACK) static_eval = -static_eval;
        
        if (static_eval >= beta) return static_eval;
        if (static_eval > alpha) alpha = static_eval;

        int moves_c = MoveGenerator::genAllMoves(gs, moves, in_check, true); 
        if (moves_c == 0) {
            return in_check ? -MATE + ply : alpha; // checkmate / draw
        }

        orderMoves(gs, moves, moves_c);

        for (int i = 0; i < moves_c; i++) {
            Move move = moves[i];

            Unmove unmove = gs.applyMove(move);

            /* temp */ qcnt++;

            int score = -quiescence(gs, -beta, -alpha, ply+1);
            gs.applyUnmove(unmove);

            if (score > alpha) alpha = score;
            if (score >= beta) break;
        }

        return alpha;
    }

    int negamax(GameState & gs, int depth, int alpha, int beta, int ply = 0) {
        if (depth == 0) {
            return quiescence(gs, alpha, beta, ply);
        }

        bool in_check;
        Move moves[256];
        int moves_c = MoveGenerator::genAllMoves(gs, moves, in_check);

        if (moves_c == 0) {
            return in_check ? -MATE + ply : 0; // checkmate / draw
        }

        orderMoves(gs, moves, moves_c);

        for (int i = 0; i < moves_c; i++) {
            Move move = moves[i];

            Unmove unmove = gs.applyMove(move);

            /* temp */ ncnt++;

            int score = -negamax(gs, depth-1, -beta, -alpha, ply+1);
            gs.applyUnmove(unmove);

            if (score > alpha) alpha = score;
            if (score >= beta) break;
        }

        return alpha;
    }



    static inline void orderRootMoves(Move * moves, int n, const std::vector<int> & last_scores) {
        // last_scores[i] corresponds to moves[i] from previous iteration
        // fallback: if last_scores empty, do your normal order_moves(gs,...)
        std::vector<int> idx(n); for (int i=0;i<n;++i) idx[i]=i;
        std::stable_sort(idx.begin(), idx.end(), [&](int a, int b){
            return last_scores[a] > last_scores[b];
        });
        // apply permutation in-place (small n, so simple cycle permute is fine)
        for (int i=0;i<n;++i){
            while (idx[i]!=i){
                std::swap(moves[i], moves[idx[i]]);
                std::swap(const_cast<int&>(last_scores[i]), const_cast<int&>(last_scores[idx[i]])); // if last_scores is non-const
                std::swap(idx[i], idx[idx[i]]);
            }
        }
    }

    struct RootResult {
        Move move;
        int score;   // from side-to-move POV
    };

    
    static constexpr int ASP_WINDOW = 50; // start narrow; widen on fail

    // Iterative deepening driver
    // Returns the final list of root moves with their depth-N scores (sorted best-first).
    std::vector<RootResult> evaluateAllMoves(GameState & root_gs, int maxDepth) {
        bool in_check;
        Move rootMoves[256];
        int n = MoveGenerator::genAllMoves(root_gs, rootMoves, in_check);
        if (n == 0) return {}; // stalemate/checkmate handled in negamax

        // seed order once (captures first etc.)
        orderMoves(root_gs, rootMoves, n);

        std::vector<int> lastScores(n, 0);  // scores from previous iteration
        std::vector<int> currScores(n, 0);  // scores at current iteration

        int bestIndex = 0;
        int lastBestScore = 0;

        for (int depth = 1; depth <= maxDepth; ++depth) {
            if (depth > 1) {
                // Stable reorder rootMoves by lastScores (higher first)
                std::vector<int> tmpLast = lastScores; // copy because order_root_moves mutates
                orderRootMoves(rootMoves, n, tmpLast);
            }

            // Aspiration window around last best score
            int alpha0 = -EVAL_INF, beta0 = EVAL_INF;
            if (depth > 1) {
                alpha0 = lastBestScore - ASP_WINDOW;
                beta0  = lastBestScore + ASP_WINDOW;
            }

            int bestScore = -EVAL_INF;
            bestIndex = 0;

            // Search all root moves
            for (int i = 0; i < n; ++i) {
                Move m = rootMoves[i];

                Unmove u = root_gs.applyMove(m);

                // PVS at root
                int score = -negamax(root_gs, depth - 1, -beta0, -alpha0, /*ply=*/1);

                // Aspiration fail: re-search with full window
                if (score <= alpha0 || score >= beta0) {
                    score = -negamax(root_gs, depth - 1, -EVAL_INF, EVAL_INF, 1);
                }

                root_gs.applyUnmove(u);

                currScores[i] = score;

                if (score > bestScore) {
                    bestScore = score;
                    bestIndex = i;
                }
                if (score > alpha0) alpha0 = score; // keep window somewhat tightened
            }

            // Prepare for next iteration
            lastBestScore = bestScore;
            lastScores = currScores;
            // (Optional) bump TT generation here if you add a TT: tt.bump_generation();
        }

        // Build result vector sorted by final scores
        std::vector<RootResult> out;
        out.reserve(n);
        for (int i=0;i<n;++i) out.emplace_back(RootResult {rootMoves[i], lastScores[i]});
        std::stable_sort(out.begin(), out.end(),
            [](auto& a, auto& b){ return a.score > b.score; });
        return out;
    }

}