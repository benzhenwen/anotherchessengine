#pragma once

#include <algorithm>
#include <thread>
#include <atomic>

#include <lib/chess/util.hpp>
#include <lib/chess/gamestate.hpp>
#include <lib/chess/move.hpp>
#include <lib/chess/movegenerator.hpp>
#include <lib/chess/unmove.hpp>

#include <lib/chess/engine/negamax.hpp>
#include <lib/chess/engine/transpositiontable.hpp>

namespace Chess::Engine {
using namespace Chess::Engine::Negamax;
class Engine {
public:
    struct MoveResult {
        Move move;
        int score;   // from side-to-move POV
    };

protected:
    GameState game_state; // should only be modified when the engine is not running

    MoveResult move_scores[256]; // should only be accessed when the engine has not flagged lock_move_scores
    unsigned int scores_c;
    U8 current_depth;
    
    TranspositionTable tt; 

    // thread stuff
    std::atomic<bool> running_flag_in = false; // flag the engine object modifies to tell the worker flag when to start/stop
    std::atomic<bool> running_flag_out = false; // flag the worker thread modifies to tell the engine once it has started/stopped after the running_flag_in is switched
    std::atomic<bool> lock_move_scores_flag_in = false; // flag that, when true, move_scores and current_depth should not be accessed by the worker as it's being read by the engine
    std::atomic<bool> lock_move_scores_flag_out = false; // flag that, when true, move_scores and current_depth should not be accessed by the engine as it's being written/read to by the worker
    std::thread worker;

public:
    Engine(size_t tt_size = 8):
        game_state(),
        move_scores(),
        scores_c(0),
        current_depth(0),
        tt(tt_size) {}

    void load_game_state(const GameState & gs) {
        assert(!running_flag_in && !running_flag_out); // ensure the engine is not running right now
        game_state = gs;
    }







    inline void resize_tt(size_t new_size) { // expensive call and wipes the tt
        assert(!running_flag_in && !running_flag_out); // ensure the engine is not running right now
        tt = TranspositionTable(new_size);
    }






protected:
    static constexpr int ASP_WINDOW = 50; // start narrow; widen on fail


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

    
public:
    // Iterative deepening driver
    // Returns the final list of root moves with their depth-N scores (sorted best-first).
    std::vector<MoveResult> evaluateAllMoves(GameState & root_gs, int maxDepth) {
        
        Move root_moves[256];
        MoveGenerator::PreMoveData pre_move_data = MoveGenerator::genPreMoveData(root_gs);
        int n = MoveGenerator::genAllMoves(root_gs, pre_move_data, root_moves);
        if (n == 0) return {}; // stalemate/checkmate handled in negamax

        // seed order once (captures first etc.)
        orderMoves(root_gs, root_moves, n, Move());

        std::vector<int> last_scores(n, 0);  // scores from previous iteration
        std::vector<int> curr_scores(n, 0);  // scores at current iteration

        int last_best_score = 0;

        for (int depth = 1; depth <= maxDepth; ++depth) {
            if (depth > 1) {
                // Stable reorder rootMoves by lastScores (higher first)
                std::vector<int> tmp_last = last_scores; // copy because orderRootMoves mutates
                orderRootMoves(root_moves, n, tmp_last);
            }

            // Aspiration window around last best score
            int alpha0 = -EVAL_INF, beta0 = EVAL_INF;
            if (depth > 1) {
                alpha0 = last_best_score - ASP_WINDOW;
                beta0  = last_best_score + ASP_WINDOW;
            }

            const int ORIG_ALPHA = alpha0;
            int best_score = -EVAL_INF;
            Move best_move;

            // Search all root moves
            for (int i = 0; i < n; ++i) {
                Move move = root_moves[i];
                Unmove u = root_gs.applyMove(move);

                // PVS at root
                int score = -negamax(root_gs, tt, depth - 1, -beta0, -alpha0, 1);

                // Aspiration fail: re-search with full window
                if (score <= alpha0 || score >= beta0) {
                    score = -negamax(root_gs, tt, depth - 1, -EVAL_INF, EVAL_INF, 1);
                }

                root_gs.applyUnmove(u);

                curr_scores[i] = score;

                if (score > best_score) {
                    best_score = score;
                    best_move = move;
                }
                if (score > alpha0) {
                    alpha0 = score;
                    if (alpha0 >= beta0) break;
                }
            }

            // store results to TT
            TranspositionTable::Node::Type bound;
            if (best_score <= ORIG_ALPHA) bound = TranspositionTable::Node::UPPERBOUND; // fail-low
            else if (best_score >= beta0) bound = TranspositionTable::Node::LOWERBOUND; // fail-high
            else                          bound = TranspositionTable::Node::EXACT;      // PV node
            tt.add_entry(root_gs.getHashCode(), best_move, best_score, depth, bound);

            // Prepare for next iteration
            last_best_score = best_score;
            last_scores = curr_scores;
            tt.bump_generation();
        }

        // Build result vector sorted by final scores
        std::vector<MoveResult> out;
        out.reserve(n);
        for (int i=0;i<n;++i) out.emplace_back(MoveResult {root_moves[i], last_scores[i]});
        std::stable_sort(out.begin(), out.end(),
            [](auto& a, auto& b){ return a.score > b.score; });
        return out;
    }

};
}