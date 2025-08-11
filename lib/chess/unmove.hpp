#pragma once

#include <lib/chess/util.hpp>
#include <lib/chess/move.hpp>

namespace Chess {
struct Unmove {
    int from;
    int to;

    PIECE starting_piece;
    PIECE ending_piece;

    enum CAPTURE {PAWN=0, KNIGHT, BISHOP, ROOK, QUEEN, NONE} captured_piece;

    bool is_en_passant;
    bool is_castle;

    bool castle_K, castle_Q, castle_k, castle_q;
    
    int en_passant;

    unsigned int halfmove_clock;
    unsigned int fullmove_clock;

    COLOR turn_before;
    U64 hash_before;

    Unmove(int _from, int _to, PIECE _starting_piece, PIECE _ending_piece, CAPTURE _captured_piece, bool _is_en_passant, bool _is_castle, bool pre_castle_K, bool pre_castle_Q, bool pre_castle_k, bool pre_castle_q, int pre_en_passant, bool pre_halfmove_clock, bool pre_fullmove_clock, COLOR pre_turn, U64 pre_hash_code):
        from(_from),
        to(_to),
        starting_piece(_starting_piece),
        ending_piece(_ending_piece),
        captured_piece(_captured_piece),
        is_en_passant(_is_en_passant),
        is_castle(_is_castle),
        castle_K(pre_castle_K),
        castle_Q(pre_castle_Q),
        castle_k(pre_castle_k),
        castle_q(pre_castle_q),
        en_passant(pre_en_passant),
        halfmove_clock(pre_halfmove_clock),
        fullmove_clock(pre_fullmove_clock),
        turn_before(pre_turn),
        hash_before(pre_hash_code)
    {}
};
}