#pragma once

#include <iostream>

#include <lib/chess/util.hpp>
#include <lib/chess/gamestate.hpp>
#include <lib/chess/move.hpp>
#include <lib/chess/movegenerator.hpp>
#include <lib/chess/unmove.hpp>

namespace Chess::Engine::Perft {

    U64 perft(GameState& gs, int depth, bool top_depth = true) {
        if (depth == 0) return 1;

        Move moves[256];
        int n = MoveGenerator::genAllMoves(gs, moves);
        if (depth == 1) return n;

        U64 nodes = 0;
        for (int i = 0; i < n; ++i) {
            const Move& m = moves[i];

            Unmove u = gs.get_unmove(m); 
            gs.applyMove(m); 

            U64 cnt = perft(gs, depth - 1, false);

            if (top_depth) {
                std::cout << m.toString() << ' ' << cnt << '\n';
            }

            nodes += cnt;
            gs.applyUnmove(u); 
        }
        return nodes;
    }

}