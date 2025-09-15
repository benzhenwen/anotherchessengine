#include <string>
#include <vector>
#include <variant>
#include <sstream>
#include <iomanip>
#include <fstream>

#include <lib/chess/util.hpp>
#include <lib/chess/ascii.hpp>

#include <generators/genBitboard/stringifyU64.hpp>

#include <logger/logger.hpp>

using namespace Chess;

U64 computeBetween(int sq1, int sq2) {
    U64 between = 0ULL;

    int r1 = squareRow(sq1), c1 = squareCol(sq1);
    int r2 = squareRow(sq2), c2 = squareCol(sq2);

    int dr = (r2 > r1) - (r2 < r1);
    int dc = (c2 > c1) - (c2 < c1);

    // Not aligned in a straight line (orthogonal or diagonal)
    if (dr != 0 && dc != 0 && std::abs(r1 - r2) != std::abs(c1 - c2))
        return 0;

    int r = r1 + dr;
    int c = c1 + dc;

    while (r != r2 || c != c2) {
        between |= squareToBitboard(r * 8 + c);
        r += dr;
        c += dc;
    }

    return between & ~squareToBitboard(sq1) & ~squareToBitboard(sq2); // exclude endpoints
}

int main() {
    static constexpr Logger logger = Logger("Bitboard Gen");
    
    std::ofstream out("bitboards.hpp");
    out << "#pragma once\n"
        << "#include <cstdint>\n\n"
        << "namespace Chess::Bitboards {\n\n"
        << "    using U64 = uint64_t;\n";
    

    // MOVE BITBOARDS --------------------------------------------------------------

    U64 pawn_pushes[64][2] = { 0 }; // [color][square]
    U64 pawn_double_pushes[64][2] = { 0 }; 
    U64 pawn_attacks[64][2] = { 0 }; 

    U64 knight_moves[64] = { 0 };

    U64 bishop_moves[64] = { 0 };
    U64 bishop_rays[64][4] = { 0 }; // [space][direction]
    
    U64 rook_moves[64] = { 0 };
    U64 rook_rays[64][4] = { 0 }; // [space][direction]

    U64 king_moves[64] = { 0 };


    // pawns
    for (int color = WHITE; color <= BLACK; color++) {
        const int dir = (color == WHITE) ? 1 : -1;
        const int doubleRow = (color == WHITE) ? 1 : 6;
        const int endRow = (color == WHITE) ? 7 : 0;

        for (int space = 0; space < 64; space++) {
            if (squareRow(space) != endRow) {
                pawn_pushes[space][color] |= squareToBitboard(offsetSquare(space, dir, 0));
                if (squareRow(space) == doubleRow) pawn_double_pushes[space][color] |= squareToBitboard(offsetSquare(space, 2*dir, 0));

                if (squareCol(space) != 7) pawn_attacks[space][color] |= squareToBitboard(offsetSquare(space, dir, 1));
                if (squareCol(space) != 0) pawn_attacks[space][color] |= squareToBitboard(offsetSquare(space, dir, -1));
            }
        }
    }
    out << "    static constexpr U64 pawn_pushes[64][2] = ";
    out << stringifyU64Array(pawn_pushes, 1) << ";\n";
    out << "    static constexpr U64 pawn_double_pushes[64][2] = ";
    out << stringifyU64Array(pawn_double_pushes, 1) << ";\n";
    out << "    static constexpr U64 pawn_attacks[64][2] = ";
    out << stringifyU64Array(pawn_attacks, 1) << ";\n";
    logger << "Generated pawn bitboards";


    // knights
    for (int space = 0; space < 64; space++) {
        const int movesKnight[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
        for (int i = 0; i < 8; i++) {
            if (offsetInBounds(space, movesKnight[i][0], movesKnight[i][1])) {
                knight_moves[space] |= squareToBitboard(offsetSquare(space, movesKnight[i][0], movesKnight[i][1]));
            }
        }
    }
    out << "    static constexpr U64 knight_moves[64] = ";
    out << stringifyU64Array(knight_moves, 1) << ";\n";
    logger << "Generated knight bitboards";


    // king
    for (int space = 0; space < 64; space++) {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) continue;
                if (offsetInBounds(space, dr, dc)) {
                    king_moves[space] |= squareToBitboard(offsetSquare(space, dr, dc));
                }
            }
        }
    }
    out << "    static constexpr U64 king_moves[64] = ";
    out << stringifyU64Array(king_moves, 1) << ";\n";
    logger << "Generated king bitboard";




    // rook rays & moves
    const int rook_dirs[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} }; // NSEW
    for (int sq = 0; sq < 64; ++sq) {
        for (int d = 0; d < 4; ++d) {
            int dr = rook_dirs[d][0];
            int dc = rook_dirs[d][1];
            int r = squareRow(sq) + dr;
            int c = squareCol(sq) + dc;
            while (r >= 0 && r < 8 && c >= 0 && c < 8) {
                int target = r * 8 + c;
                rook_rays[sq][d] |= squareToBitboard(target);
                rook_moves[sq]   |= squareToBitboard(target);
                r += dr;
                c += dc;
            }
        }
    }
    out << "    static constexpr U64 rook_moves[64] = ";
    out << stringifyU64Array(rook_moves, 1) << ";\n";
    out << "    static constexpr U64 rook_rays[64][4] = ";
    out << stringifyU64Array(rook_rays, 1) << ";\n";
    logger << "Generated rook bitboards";


    // bishop rays & moves
    const int bishop_dirs[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} }; // NE NW SE SW
    for (int sq = 0; sq < 64; ++sq) {
        for (int d = 0; d < 4; ++d) {
            int dr = bishop_dirs[d][0];
            int dc = bishop_dirs[d][1];
            int r = squareRow(sq) + dr;
            int c = squareCol(sq) + dc;
            while (r >= 0 && r < 8 && c >= 0 && c < 8) {
                int target = r * 8 + c;
                bishop_rays[sq][d] |= squareToBitboard(target);
                bishop_moves[sq]   |= squareToBitboard(target);
                r += dr;
                c += dc;
            }
        }
    }
    out << "    static constexpr U64 bishop_moves[64] = ";
    out << stringifyU64Array(bishop_moves, 1) << ";\n";
    out << "    static constexpr U64 bishop_rays[64][4] = ";
    out << stringifyU64Array(bishop_rays, 1) << ";\n";
    logger << "Generated bishop bitboards";

    
    // UNTILITY BITBOARDS --------------------------------------------------------------

    U64 rows[8] = { 0 };
    U64 cols[8] = { 0 };

    U64 between[64][64] = { 0 }; // [a][b]

    U64 center4 = 0;
    U64 center16 = 0;
    U64 edge = 0;

    // row
    for (int r = 0; r < 8; ++r) {
        for (int f = 0; f < 8; ++f) {
            rows[r] |= squareToBitboard(r * 8 + f);
        }
    }
    out << "    static constexpr U64 rows[8] = ";
    out << stringifyU64Array(rows, 1) << ";\n";
    logger << "Generated row bitboards";

    // column
    for (int f = 0; f < 8; ++f) {
        for (int r = 0; r < 8; ++r) {
            cols[f] |= squareToBitboard(r * 8 + f);
        }
    }
    out << "    static constexpr U64 cols[8] = ";
    out << stringifyU64Array(cols, 1) << ";\n";
    logger << "Generated col bitboards";

    // between
    for (int from = 0; from < 64; ++from) {
        for (int to = 0; to < 64; ++to) {
            between[from][to] = computeBetween(from, to);
        }
    }
    out << "    static constexpr U64 between[64][64] = ";
    out << stringifyU64Array(between, 1) << ";\n";
    logger << "Generated between bitboards";

    // center and edge
    for (int i = 0; i < 64; i++) {
        const int r = squareRow(i);
        const int c = squareCol(i);

        if (r >= 3 && r <= 4 && c >= 3 && c <= 4) center4 |= squareToBitboard(i);
        if (r >= 2 && r <= 5 && c >= 2 && c <= 5) center16 |= squareToBitboard(i);
        if (r == 0 || r == 7 || c == 0 || c == 7) edge |= squareToBitboard(i);
    }
    out << "    static constexpr U64 center4 = " << "0x" << std::hex << std::setw(16) << std::setfill('0') << center4 << ";\n";
    out << "    static constexpr U64 center16 = " << "0x" << std::hex << std::setw(16) << std::setfill('0') << center16 << ";\n";
    out << "    static constexpr U64 edge = " << "0x" << std::hex << std::setw(16) << std::setfill('0') << edge << ";\n";
    logger << "Generated center and edge bitboards";



    logger << '\n' << Ascii::bitboardToAscii(pawn_pushes[54][BLACK]);

    out << '}';
    out.close();

}
