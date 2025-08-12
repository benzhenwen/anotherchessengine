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

    int boardVision(const GameState & gs, COLOR color) {
        int output = 0;

        for (int piece_type = PAWN; piece_type <= KING; piece_type++) {
            U64 bb = gs.pieces[color][piece_type];
            while (bb) {
                const int piece_location = getLeastBitboardSquare(bb);
                switch (piece_type) {
                    case PAWN:   output += getBitboardPopulation(Bitboards::pawn_attacks[piece_location][color]); break;
                    case KNIGHT: output += getBitboardPopulation(Bitboards::knight_moves[piece_location]); break;
                    case BISHOP: output += getBitboardPopulation(MoveGenerator::gen_bishop_rays(piece_location, gs.occupied_spaces)); break;
                    case ROOK:   output += getBitboardPopulation(MoveGenerator::gen_rook_rays(piece_location, gs.occupied_spaces)); break;
                    case QUEEN:  output += getBitboardPopulation(MoveGenerator::gen_bishop_rays(piece_location, gs.occupied_spaces) | MoveGenerator::gen_rook_rays(piece_location, gs.occupied_spaces)); break;
                    case KING:   output += getBitboardPopulation(Bitboards::king_moves[piece_location]); break;
                }
                bb &= bb - 1;
            }
        }

        return output;
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

        eval += (boardVision(gs, WHITE) - boardVision(gs, BLACK)) / 2;

        return eval;
    }

}