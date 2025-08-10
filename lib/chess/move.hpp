#pragma once

#include <lib/chess/util.hpp>

namespace Chess {
struct Move {
    int from;
    int to;
    
    PIECE starting_piece;
    PIECE ending_piece;

    // useful metadata
    enum CAPTURE {PAWN=0, KNIGHT, BISHOP, ROOK, QUEEN, KING, NONE} capture;

    bool is_en_passant;
    bool is_castle;

    Move() = default;
    Move(int _from, int _to, PIECE _starting_piece, PIECE _ending_piece, CAPTURE _capture = NONE, bool _is_en_passant = false, bool _is_castle = false):
        from(_from),
        to(_to),
        starting_piece(_starting_piece),
        ending_piece(_ending_piece),
        capture(_capture),
        is_en_passant(_is_en_passant),
        is_castle(_is_castle)
    {}

    std::string toString() const {
        return squareToLetterNumber(from) + " -> " + squareToLetterNumber(to);
    }

private:
    std::string squareToLetterNumber(int square) const {
        return (char) ('a' + (square%8)) + std::to_string(square/8 + 1);
    }

};
}