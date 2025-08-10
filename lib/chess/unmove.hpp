#pragma once

#include <lib/chess/util.hpp>
#include <lib/chess/move.hpp>

namespace Chess {
struct Unmove {
    int from;
    int to;

    PIECE starting_piece;
    PIECE ending_piece;

    Move::CAPTURE capture;

    bool is_en_passant;
    bool is_castle;

    bool castle_K, castle_Q, castle_k, castle_q;
    
    int en_passant;

    unsigned int halfmove_clock;
    unsigned int fullmove_clock;

    COLOR turn_before;
    U64 hash_before;

    Unmove(const Move & move, bool pre_castle_K, bool pre_castle_Q, bool pre_castle_k, bool pre_castle_q, bool pre_en_passant, bool pre_halfmove_clock, bool pre_fullmove_clock, COLOR pre_turn, U64 pre_hash_code):
        from(move.from),
        to(move.to),
        starting_piece(move.starting_piece),
        ending_piece(move.ending_piece),
        capture(move.capture),
        is_en_passant(move.is_en_passant),
        is_castle(move.is_castle),
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