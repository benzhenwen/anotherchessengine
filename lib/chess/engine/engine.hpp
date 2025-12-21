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

    // Iterative deepening driver
    void evaluateAllMoves(unsigned int max_depth, unsigned int current_depth, MoveResult * move_score_v, unsigned int * move_score_c) {
        
    }


public:
    void evaluateAllMoves(unsigned int max_depth) {
        evaluateAllMoves(max_depth, 1, move_scores, &scores_c);
    }

    


    std::vector<MoveResult> evaluateAllMoves(GameState & root_gs, int max_depth, int curr_depth = 1) {
        
        // generate all moves
        Move root_moves[256];
        MoveGenerator::PreMoveData pre_move_data = MoveGenerator::genPreMoveData(root_gs);
        int n = MoveGenerator::genAllMoves(root_gs, pre_move_data, root_moves);

        // return nothing if no moves can be made
        if (n == 0) return {}; 

        
        
    }




};
}