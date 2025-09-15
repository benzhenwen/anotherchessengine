#include <random>
#include <iostream>
#include <fstream>

#include <lib/chess/util.hpp>
#include <lib/chess/ascii.hpp>
#include <lib/lookuptables/bitboards.hpp>

#include <generators/genBitboard/stringifyU64.hpp>

#include <logger/logger.hpp>

using namespace Chess;

// random

std::mt19937_64 gen_a(8693890653555522);
std::mt19937_64 gen_b(1349840982024467);
std::uniform_int_distribution<uint64_t> dis(0, ~0ULL);


// origional functions from chess/movegenerator.hpp to be replaced
inline U64 genBishopRays(int position, U64 occupied_spaces) {
    U64 output = 0;

    for (int d = 0; d < 4; ++d) {
        U64 ray = Bitboards::bishop_rays[position][d]; // NE NW SE SW
        U64 blockers = ray & occupied_spaces;

        if (!blockers) {
            output |= ray; // no blocker: full ray
        } else {
            int blocker_square = (d <= 1) ? getLeastBitboardSquare(blockers) : getMostBitboardSquare(blockers);
            output |= Bitboards::between[position][blocker_square] | squareToBitboard(blocker_square);
        }
    }
    return output;
}
inline U64 genRookRays(int position, U64 occupied_spaces) {
    U64 output = 0;

    for (int d = 0; d < 4; ++d) {
        U64 ray = Bitboards::rook_rays[position][d]; // N, S, E, W
        U64 blockers = ray & occupied_spaces;

        if (!blockers) {
            output |= ray; // no blocker: full ray
        } else {
            int blocker_square = (d % 2 == 0) ? getLeastBitboardSquare(blockers) : getMostBitboardSquare(blockers);
            output |= Bitboards::between[position][blocker_square] | squareToBitboard(blocker_square);
        }
    }
    return output;
}
    
// relevant blocker mask gen
inline U64 genBlockerMask(U64 moves_mask, int position) {
    U64 output = moves_mask;
    if (squareRow(position) != 0) output &= ~Bitboards::rows[0];
    if (squareRow(position) != 7) output &= ~Bitboards::rows[7];
    if (squareCol(position) != 0) output &= ~Bitboards::cols[0];
    if (squareCol(position) != 7) output &= ~Bitboards::cols[7];
    return output;
}


// subsets of a mask
inline std::vector<U64> maskSubsets(U64 mask) {
    auto subsets = std::vector<U64>();
    subsets.reserve(4096);

    U64 subset = 0;
    do {
        subsets.push_back(subset);
        subset = (subset - mask) & mask;
    } while (subset != 0);

    return subsets;
}

// RUN genBitboard.main project fisrt (genBitboard) before this, as it is dependent on the lookuptables provided
int main() {
    static constexpr Logger logger = Logger("Magic Bitboard Gen");

    std::ofstream out("magicbitboards.hpp");
    out << "#pragma once\n"
        << "#include <cstdint>\n\n"
        << "namespace Chess::MagicBitboards {\n\n"
        << "    using U64 = uint64_t;\n";

    // rook magic bitboard
    // 4096 for 2^12 relevant bits maximum (rook sees 14 squares, minus 2 maximum when in corner to shave off edges)
    int rook_magic_relevant_bits_count[64];
    U64 rook_magic_relevant_squares_mask[64];
    U64 rook_magic_numbers[64];
    U64 rook_magic_bitboards[64][4096]; // square, magic index

    for (int sq = 0; sq < 64; sq++) {
        U64 relevant_squares = genBlockerMask(Bitboards::rook_moves[sq], sq); // generate all moves except for ones that "hit" the edge
        rook_magic_relevant_squares_mask[sq] = relevant_squares;
        int relevant_squares_cnt = getBitboardPopulation(relevant_squares);
        rook_magic_relevant_bits_count[sq] = relevant_squares_cnt;

        std::vector<U64> subsets = maskSubsets(relevant_squares);

        std::vector<U64> attacks(subsets.size());
        for (size_t i = 0; i < subsets.size(); i++) {
            attacks[i] = genRookRays(sq, subsets[i]);
        }

        // find magic number
        U64 found_magic = 0;
        for (int attempt = 0; attempt < 1000000; attempt++) {
            U64 magic_number = dis(gen_a) & dis(gen_a) & dis(gen_a); // sparse random
            if (((relevant_squares * magic_number) & 0xFF00000000000000ULL) == 0) continue; // basic filter - top bits are "random enough"

            // check if magic number is good
            std::vector<U64> used_boards(1ull << relevant_squares_cnt, ~0ULL);
            std::fill(used_boards.begin(), used_boards.end(), 0xFFFFFFFFFFFFFFFFULL);

            bool is_valid = true;
            for (size_t i = 0; i < subsets.size(); i++) {
                const int index = ((subsets[i] * magic_number) >> (64 - relevant_squares_cnt));
                if (used_boards[index] == 0xFFFFFFFFFFFFFFFFULL) {
                    used_boards[index] = attacks[i];
                } 
                else if (used_boards[index] != attacks[i]) {
                    is_valid = false;
                    break;
                }
            }

            if (is_valid) {
                found_magic = magic_number;
                break;
            }
        }

        if (!found_magic) {
            logger.log(Logger::CRITICAL) << "failed to find a magic number for rooks, try again with a different seed?";
            logger << "squares completed: " << sq;
            return -1;
        }

        // populate
        rook_magic_numbers[sq] = found_magic;
        for (size_t i = 0; i < subsets.size(); i++) {
            const int index = ((subsets[i] * found_magic) >> (64 - relevant_squares_cnt));
            rook_magic_bitboards[sq][index] = attacks[i];
        }
    }

    out << "    static constexpr U64 rook_magic_relevant_bits_count[64] = ";
    out << stringifyIntArray(rook_magic_relevant_bits_count, 1) << ";\n";
    out << "    static constexpr U64 rook_magic_relevant_squares_mask[64] = ";
    out << stringifyU64Array(rook_magic_relevant_squares_mask, 1) << ";\n";
    out << "    static constexpr U64 rook_magic_numbers[64] = ";
    out << stringifyU64Array(rook_magic_numbers, 1) << ";\n";
    out << "    static constexpr U64 rook_magic_bitboards[64][4096] = ";
    out << stringifyU64Array(rook_magic_bitboards, 1) << ";\n";
    logger << "generated magic bitboards for rooks!";





    // bishop magic bitboard
    // 4096 for 2^9 relevant bits maximum (rook sees 13 squares, minus 4 maximum when in center to shave off edges)
    int bishop_magic_relevant_bits_count[64];
    U64 bishop_magic_relevant_squares_mask[64];
    U64 bishop_magic_numbers[64];
    U64 bishop_magic_bitboards[64][512]; // square, magic index

    for (int sq = 0; sq < 64; sq++) {
        U64 relevant_squares = genBlockerMask(Bitboards::bishop_moves[sq], sq); // generate all moves except for ones that "hit" the edge
        bishop_magic_relevant_squares_mask[sq] = relevant_squares;
        int relevant_squares_cnt = getBitboardPopulation(relevant_squares);
        bishop_magic_relevant_bits_count[sq] = relevant_squares_cnt;

        std::vector<U64> subsets = maskSubsets(relevant_squares);

        std::vector<U64> attacks(subsets.size());
        for (size_t i = 0; i < subsets.size(); i++) {
            attacks[i] = genBishopRays(sq, subsets[i]);
        }

        // find magic number
        U64 found_magic = 0;
        for (int attempt = 0; attempt < 1000000; attempt++) {
            U64 magic_number = dis(gen_a) & dis(gen_a) & dis(gen_a); // sparse random
            if (((relevant_squares * magic_number) & 0xFF00000000000000ULL) == 0) continue; // basic filter - top bits are "random enough"

            // check if magic number is good
            std::vector<U64> used_boards(1ull << relevant_squares_cnt, ~0ULL);
            std::fill(used_boards.begin(), used_boards.end(), 0xFFFFFFFFFFFFFFFFULL);

            bool is_valid = true;
            for (size_t i = 0; i < subsets.size(); i++) {
                const int index = ((subsets[i] * magic_number) >> (64 - relevant_squares_cnt));
                if (used_boards[index] == 0xFFFFFFFFFFFFFFFFULL) {
                    used_boards[index] = attacks[i];
                } 
                else if (used_boards[index] != attacks[i]) {
                    is_valid = false;
                    break;
                }
            }

            if (is_valid) {
                found_magic = magic_number;
                break;
            }
        }

        if (!found_magic) {
            logger.log(Logger::CRITICAL) << "failed to find a magic number for bishops, try again with a different seed?";
            logger << "squares completed: " << sq;
            return -1;
        }

        // populate
        bishop_magic_numbers[sq] = found_magic;
        for (size_t i = 0; i < subsets.size(); i++) {
            const int index = ((subsets[i] * found_magic) >> (64 - relevant_squares_cnt));
            bishop_magic_bitboards[sq][index] = attacks[i];
        }
    }

    out << "    static constexpr U64 bishop_magic_relevant_bits_count[64] = ";
    out << stringifyIntArray(bishop_magic_relevant_bits_count, 1) << ";\n";
    out << "    static constexpr U64 bishop_magic_relevant_squares_mask[64] = ";
    out << stringifyU64Array(bishop_magic_relevant_squares_mask, 1) << ";\n";
    out << "    static constexpr U64 bishop_magic_numbers[64] = ";
    out << stringifyU64Array(bishop_magic_numbers, 1) << ";\n";
    out << "    static constexpr U64 bishop_magic_bitboards[64][512] = ";
    out << stringifyU64Array(bishop_magic_bitboards, 1) << ";\n";
    logger << "generated magic bitboards for bishops!";

    

    out << '}';
    out.close();


    return 0;
}