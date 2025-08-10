#pragma once

#include <string>
#include <cstdint>
#include <stdexcept>
#include <cassert>

namespace Chess {
    using U64 = uint64_t;
    using U64f = uint_fast64_t;

    enum COLOR {WHITE=0, BLACK=1};
    enum PIECE {PAWN=0, KNIGHT, BISHOP, ROOK, QUEEN, KING};

    struct Space {
        bool empty = true;
        COLOR color;
        PIECE piece;
    };

    constexpr uint64_t square_to_bit[64] = {
        1ULL << 0,  1ULL << 1,  1ULL << 2,  1ULL << 3, 1ULL << 4,  1ULL << 5,  1ULL << 6,  1ULL << 7,
        1ULL << 8,  1ULL << 9,  1ULL << 10, 1ULL << 11, 1ULL << 12, 1ULL << 13, 1ULL << 14, 1ULL << 15,
        1ULL << 16, 1ULL << 17, 1ULL << 18, 1ULL << 19, 1ULL << 20, 1ULL << 21, 1ULL << 22, 1ULL << 23,
        1ULL << 24, 1ULL << 25, 1ULL << 26, 1ULL << 27, 1ULL << 28, 1ULL << 29, 1ULL << 30, 1ULL << 31,
        1ULL << 32, 1ULL << 33, 1ULL << 34, 1ULL << 35, 1ULL << 36, 1ULL << 37, 1ULL << 38, 1ULL << 39,
        1ULL << 40, 1ULL << 41, 1ULL << 42, 1ULL << 43, 1ULL << 44, 1ULL << 45, 1ULL << 46, 1ULL << 47,
        1ULL << 48, 1ULL << 49, 1ULL << 50, 1ULL << 51, 1ULL << 52, 1ULL << 53, 1ULL << 54, 1ULL << 55,
        1ULL << 56, 1ULL << 57, 1ULL << 58, 1ULL << 59, 1ULL << 60, 1ULL << 61, 1ULL << 62, 1ULL << 63
    };

    inline U64 squareToBitboard(int sq) {
        return square_to_bit[sq];
    }

    inline int getLeastBitboardSquare(U64 bb) { // returns the index of the LSB on the bitboard - UNDEFINED BEHAVIOUR WHEN bb = 0
        /* FOR DEBUG */ assert(bb != 0); 
        return __builtin_ctzll(bb); // this method counts the trailing 0s on a number in o(1) (1-2 cpu clock cycles)
    }
    inline int getMostBitboardSquare(U64 bb) { // returns the index of the MSB on the bitboard - UNDEFINED BEHAVIOUR WHEN bb = 0
        /* FOR DEBUG */ assert(bb != 0); 
        return 63 - __builtin_clzll(bb);
    }

    inline int getBitboardPopulation(U64 bb) { // returns the count of '1's on the bitboard
        return __builtin_popcountll(bb);
    }

    inline int squareRow(int sq) { // inneficient! do not use in tight loops
        return sq/8;
    }
    inline int squareCol(int sq) { // inneficient! do not use in tight loops
        return sq%8;
    }

    inline bool offsetInBounds(int sq, int row_offset, int col_offset) { // inneficient! do not use in tight loops
        int row = sq / 8;
        int col = sq % 8;

        int new_row = row + row_offset;
        int new_col = col + col_offset;

        return new_row >= 0 && new_row < 8 && new_col >= 0 && new_col < 8;
    }
    inline int offsetSquare(int sq, int row_offset, int col_offset) { // inneficient! do not use in tight loops
        int row = sq / 8;
        int col = sq % 8;

        int new_row = row + row_offset;
        int new_col = col + col_offset;

        assert(new_row >= 0 && new_row < 8);
        assert(new_col >= 0 && new_col < 8);

        return new_row * 8 + new_col;
    }

    inline uint_fast8_t pieceValue(PIECE p) {
        switch (p) {
            case PAWN: return 1;
            case BISHOP: return 3;
            case KNIGHT: return 3;
            case ROOK: return 5;
            case QUEEN: return 9;
            default: return 0;
        }
    }

    inline char pieceAsciiAtSquare(const U64 pieces[2][6], int sq) {
        U64 mask = squareToBitboard(sq);

        for (int color = 0; color < 2; ++color) {
            for (int piece = 0; piece < 6; ++piece) {
                if (pieces[color][piece] & mask) {
                    switch (piece) {
                        case PAWN:   return color ? 'p' : 'P';
                        case KNIGHT: return color ? 'n' : 'N';
                        case BISHOP: return color ? 'b' : 'B';
                        case ROOK:   return color ? 'r' : 'R';
                        case QUEEN:  return color ? 'q' : 'Q';
                        case KING:   return color ? 'k' : 'K';
                    }
                }
            }
        }

        return '-'; // empty square
    }
} 
