#pragma once

#include <lib/chess/util.hpp>
#include <lib/chess/gamestate.hpp>
#include <lib/chess/movegenerator.hpp>

namespace Chess::Engine::StaticEvaluation {

    // tunables
    static constexpr int lazy_eval_threshold = 150;
    static constexpr int controlled_square_weight = 1;
    static constexpr int castle_single_weight = 20;
    static constexpr int castle_double_weight = 30;


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

    int staticEvaluation(const GameState & gs, const MoveGenerator::PreMoveData & pre_move_data, int alpha, int beta) {
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

        // castling rights bonus
        if (gs.castle_K && gs.castle_Q) eval += castle_double_weight;
        else if (gs.castle_K || gs.castle_Q) eval += castle_single_weight;

        if (gs.castle_k && gs.castle_q) eval -= castle_double_weight;
        else if (gs.castle_k || gs.castle_q) eval -= castle_single_weight;


        // lazy eval
        if (eval - lazy_eval_threshold >= beta)  return eval;
        if (eval + lazy_eval_threshold <= alpha) return eval;
        
        // more controlled squares better (changed from turn pov to WHITE centric pov)
        if (gs.turn == WHITE) eval += (pre_move_data.friendlyControlledSquaresCnt() - pre_move_data.unfriendlyControlledSquaresCnt()) * controlled_square_weight;
        else                  eval -= (pre_move_data.friendlyControlledSquaresCnt() - pre_move_data.unfriendlyControlledSquaresCnt()) * controlled_square_weight;

        return eval;
    }

}