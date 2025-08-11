#pragma once

#include <lib/chess/util.hpp>

namespace Chess {
struct Move {
    // packed value
    U16 v;

    static constexpr U16 M_FROM  = 0b1111110000000000;
    static constexpr U16 M_TO    = 0b0000001111110000;

    enum PROMO {NONE=0, KNIGHT=1, BISHOP=2, ROOK=3, QUEEN=4};
    static constexpr U16 M_PROMO = 0b0000000000001110;
    static constexpr U16 M_IS_CAPTURE = 0b0000000000000001;

    constexpr Move(): v(0) {};
    constexpr Move(int _from, int _to, PROMO _promo, bool _is_capture):
        v((U16) ((_from << 10) | (_to << 4) | (_promo << 1) | _is_capture)) 
    {
        assert(_from >= 0 && _from <= 63);
        assert(_to >= 0 && _to <= 63);
        assert(_promo >= 0 && _promo <= 4);
    }

    constexpr int from() const {
        return (v & M_FROM) >> 10;
    }
    constexpr int to() const {
        return (v & M_TO) >> 4;
    }
    constexpr PIECE promo_piece() const {
        return (PIECE) ((v & M_PROMO) >> 1); // matches NONE = pawn, so main loop pawn moves can just make output piece a pawn;
    }
    constexpr bool isCapture() const {
        return v & M_IS_CAPTURE;
    }



    // some display stuff
public:
    std::string toString() const {
        return squareToLetterNumber(from()) + " -> " + squareToLetterNumber(to());
    }

private:
    std::string squareToLetterNumber(int square) const {
        return (char) ('a' + (square%8)) + std::to_string(square/8 + 1);
    }

};

static_assert(sizeof(Move)==2);
}