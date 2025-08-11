#pragma once

#include <lib/chess/util.hpp>
#include <lib/chess/gamestate.hpp>

namespace Chess::Engine::Evaluation {

    int getPieceValue(PIECE piece) {
        switch (piece) {
            case PAWN: return 100;
            case KNIGHT: return 320;
            case BISHOP: return 330;
            case ROOK: return 500;
            case QUEEN: return 900;
            default: return 0;
        }
    }

    int staticEvaluation(const GameState & gs) {
        int eval = 0;
        for (int color = WHITE; color <= BLACK; color++) {
            for (int piece = PAWN; piece <= QUEEN; piece++) {
                U64 curr_pieces = gs.pieces[color][piece];
                while (curr_pieces) {
                    // TODO: table that gives pieces more value in certain squares, like closer to the middle

                    const int piece_val = getPieceValue((PIECE) piece);
                    eval += (color == WHITE) ? piece_val : -piece_val;

                    curr_pieces &= curr_pieces - 1;
                }
            }
        }

        return eval;
    }

}