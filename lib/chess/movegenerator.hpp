#pragma once

#include <lib/chess/util.hpp>
#include <lib/chess/ascii.hpp>
#include <lib/chess/gamestate.hpp>
#include <lib/chess/ascii.hpp>
#include <lib/chess/move.hpp>
#include <lib/lookuptables/bitboards.hpp>
#include <lib/lookuptables/magicbitboards.hpp>

#include <lib/chess/fen.hpp>

namespace Chess {
class MoveGenerator {
private:
    MoveGenerator() = delete;
    MoveGenerator(MoveGenerator & other) = delete;
    MoveGenerator(MoveGenerator && other) = delete;

public:
    struct CheckData {
        U64 checkers_bitboard;
        bool is_double_check;
        U64 evasion_bitboard; // leagal moves for single check non-king
    };

    struct PinData {
        U64 pins = 0;
        U64 allowed_moves[64];

        inline void addPin(int pin_location, U64 pin_mask_bitboard) {
            U64 pin_location_bitboard = squareToBitboard(pin_location);
            if (pins & pin_location_bitboard) { // double pin
                allowed_moves[pin_location] &= pin_mask_bitboard;
            } else { // single pin
                allowed_moves[pin_location] = pin_mask_bitboard;
                pins |= pin_location_bitboard;
            }
        }
    };

    struct PreMoveData { // data generated before extrapolating moves - useful for generating all moves but also evaluation
        U64 controlled_squares_friendly;
        U64 controlled_squares_unfriendly;
        CheckData check_data;
        PinData pin_data;
        
        inline int friendlyControlledSquaresCnt() const {
            return getBitboardPopulation(controlled_squares_friendly);
        }
        inline int unfriendlyControlledSquaresCnt() const {
            return getBitboardPopulation(controlled_squares_unfriendly);
        }
        inline bool isCheck() const {
            return check_data.checkers_bitboard;
        }
        inline int pinCount() const {
            return getBitboardPopulation(pin_data.pins);
        }
    };

public:

    static inline unsigned int genAllMoves(const GameState & gs, const PreMoveData & pre_move_data, Move * moves_v, bool gen_only_captures = false) { // returns move_c
        unsigned int moves_c = 0;

        const U64 enemy_controlled_squares = pre_move_data.controlled_squares_unfriendly;
        const CheckData & enemy_checks = pre_move_data.check_data;
        const PinData & enemy_pins = pre_move_data.pin_data;

        const U64 check_evasion_bitboard = enemy_checks.checkers_bitboard ? enemy_checks.evasion_bitboard : ~0ULL; // bitboard of moves that would block a (single) check - NOT moves the king can make to mvoe out of check

        if (enemy_checks.is_double_check) {
            // just king moves
            genKing(gs, moves_c, moves_v, enemy_controlled_squares, gen_only_captures);
        }
        else {
            // all moves
            genPawns(gs, moves_c, moves_v, check_evasion_bitboard, enemy_pins, gen_only_captures);
            genKnights(gs, moves_c, moves_v, check_evasion_bitboard, enemy_pins, gen_only_captures);
            genBishops(gs, moves_c, moves_v, check_evasion_bitboard, enemy_pins, gen_only_captures);
            genRooks(gs, moves_c, moves_v, check_evasion_bitboard, enemy_pins, gen_only_captures);
            genQueens(gs, moves_c, moves_v, check_evasion_bitboard, enemy_pins, gen_only_captures);
            genKing(gs, moves_c, moves_v, enemy_controlled_squares, gen_only_captures);
        }   

        return moves_c;
    }
    static inline PreMoveData genPreMoveData(const GameState & gs) {
        return PreMoveData {
            genControlledSquares(gs, gs.turn),
            genControlledSquares(gs, (COLOR) !gs.turn),
            genCheckData(gs, gs.turn),
            genPinData(gs, gs.turn)
        };
    }

private:
    // bishop and rook rays
    // TODO: optimization - replace generation with magic bitboards
    static inline U64 genBishopRays(int position, U64 occupied_spaces) {
        const int index = (((occupied_spaces & MagicBitboards::bishop_magic_relevant_squares_mask[position]) * MagicBitboards::bishop_magic_numbers[position]) >> (64 - MagicBitboards::bishop_magic_relevant_bits_count[position]));
        return MagicBitboards::bishop_magic_bitboards[position][index];
    }
    static inline U64 genRookRays(int position, U64 occupied_spaces) {
        const int index = (((occupied_spaces & MagicBitboards::rook_magic_relevant_squares_mask[position]) * MagicBitboards::rook_magic_numbers[position]) >> (64 - MagicBitboards::rook_magic_relevant_bits_count[position]));
        return MagicBitboards::rook_magic_bitboards[position][index];
    }
    
    static inline U64 genControlledSquares(const GameState & gs, COLOR color) {
        U64 output = 0;

        for (int piece_type = PAWN; piece_type <= KING; piece_type++) {
            U64 bb = gs.pieces[color][piece_type];
            while (bb) {
                const int piece_location = getLeastBitboardSquare(bb);
                switch (piece_type) {
                    case PAWN:   output |= Bitboards::pawn_attacks[piece_location][color]; break;
                    case KNIGHT: output |= Bitboards::knight_moves[piece_location]; break;
                    case BISHOP: output |= genBishopRays(piece_location, gs.occupied_spaces & ~gs.pieces[(COLOR) !color][KING]); break;
                    case ROOK:   output |= genRookRays(piece_location, gs.occupied_spaces & ~gs.pieces[(COLOR) !color][KING]); break;
                    case QUEEN:  output |= genBishopRays(piece_location, gs.occupied_spaces & ~gs.pieces[(COLOR) !color][KING]) | genRookRays(piece_location, gs.occupied_spaces & ~gs.pieces[(COLOR) !color][KING]); break;
                    case KING:   output |= Bitboards::king_moves[piece_location]; break;
                }
                bb &= bb - 1;
            }
        }

        return output;
    }
    static inline CheckData genCheckData(const GameState & gs, COLOR color) {
        const int king_location = getLeastBitboardSquare(gs.pieces[color][KING]); // assumes one king

        U64 checkers_bitboard = 0;
        U64 evasion_bitboard = 0;

        // basic checks
        checkers_bitboard |= Bitboards::pawn_attacks[king_location][color] & gs.pieces[!color][PAWN];
        checkers_bitboard |= Bitboards::knight_moves[king_location] & gs.pieces[!color][KNIGHT];
        checkers_bitboard |= genBishopRays(king_location, gs.occupied_spaces) & (gs.pieces[!color][BISHOP] | gs.pieces[!color][QUEEN]);
        checkers_bitboard |= genRookRays(king_location, gs.occupied_spaces) & (gs.pieces[!color][ROOK] | gs.pieces[!color][QUEEN]);
        checkers_bitboard |= Bitboards::king_moves[king_location] & gs.pieces[!color][KING];

        bool dbl = getBitboardPopulation(checkers_bitboard) >= 2;

        if (!dbl && checkers_bitboard) {
            int checker_square = getLeastBitboardSquare(checkers_bitboard); // the only checker’s square

            // If checker is a slider → can interpose OR capture
            if ((gs.pieces[!color][BISHOP] | gs.pieces[!color][ROOK] | gs.pieces[!color][QUEEN]) & checkers_bitboard) {
                evasion_bitboard = Bitboards::between[king_location][checker_square] | checkers_bitboard;
            } else {
                // Knight/pawn/king check → only capture square evades
                evasion_bitboard = checkers_bitboard;
            }
        }

        return {checkers_bitboard, dbl, evasion_bitboard};
    }
    static inline PinData genPinData(const GameState & gs, COLOR color) {
        const int king_location = getLeastBitboardSquare(gs.pieces[color][KING]); // assumes one king
        PinData output;

        // diag pins
        for (int d = 0; d < 4; ++d) {
            const U64 ray = Bitboards::bishop_rays[king_location][d]; // NE NW SE SW
            const U64 blockers = ray & gs.occupied_spaces;
            const U64 friendly_blockers = ray & gs.occupied_spaces_color[color];
            const U64 enemy_pinners = ray & (gs.pieces[!color][BISHOP] | gs.pieces[!color][QUEEN]);

            if (enemy_pinners) { // at least one enemy pinner on the ray
                const int pinning_piece_pos = (d <= 1) ? getLeastBitboardSquare(enemy_pinners) : getMostBitboardSquare(enemy_pinners);
                const U64 spaces_between = Bitboards::between[king_location][pinning_piece_pos];
                const U64 blockers_between = spaces_between & blockers;
                if (getBitboardPopulation(blockers_between) == 1 && (blockers_between & friendly_blockers)) { // only one piece between the king and pinner, and it's a friendly piece
                    output.addPin(getMostBitboardSquare(blockers_between), spaces_between | squareToBitboard(pinning_piece_pos));
                }
            }
        }
        
        // ortho pins
        for (int d = 0; d < 4; ++d) {
            const U64 ray = Bitboards::rook_rays[king_location][d]; // N S E W
            const U64 blockers = ray & gs.occupied_spaces;
            const U64 friendly_blockers = ray & gs.occupied_spaces_color[color];
            const U64 enemy_pinners = ray & (gs.pieces[!color][ROOK] | gs.pieces[!color][QUEEN]);

            if (enemy_pinners) { // at least one enemy pinner on the ray
                const int pinning_piece_pos = (d % 2 == 0) ? getLeastBitboardSquare(enemy_pinners) : getMostBitboardSquare(enemy_pinners);
                const U64 spaces_between = Bitboards::between[king_location][pinning_piece_pos];
                const U64 blockers_between = spaces_between & blockers;
                if (getBitboardPopulation(blockers_between) == 1 && (blockers_between & friendly_blockers)) { // only one piece between the king and pinner, and it's a friendly piece
                    // TODO: special case - horizontal pin enpassant??

                    output.addPin(getMostBitboardSquare(blockers_between), spaces_between | squareToBitboard(pinning_piece_pos));
                }
            }
        }

        return output;
    }

private:
    static inline void addMove(unsigned int & moves_c, Move * moves_v, const Move && move) {
        moves_v[moves_c] = move;
        moves_c++;
    }

    static inline void genPawns(const GameState & gs, unsigned int & moves_c, Move * moves_v, const U64 check_evasion_bitboard, const PinData & pin_data, bool gen_only_captures) {
        U64 pawn_bitboard = gs.pieces[gs.turn][PAWN];
        while (pawn_bitboard) {
            const int pawn_search_square = getLeastBitboardSquare(pawn_bitboard); // for each pawn of gs.turn color
            const bool is_pinned = squareToBitboard(pawn_search_square) & pin_data.pins; // pins
            
            if (!gen_only_captures) {
                U64 pawn_pushes = Bitboards::pawn_pushes[pawn_search_square][gs.turn] & ~gs.occupied_spaces;
                if (pawn_pushes) pawn_pushes |= Bitboards::pawn_double_pushes[pawn_search_square][gs.turn] & ~gs.occupied_spaces;
                pawn_pushes &= check_evasion_bitboard;
                if (is_pinned) pawn_pushes &= pin_data.allowed_moves[pawn_search_square];

                while (pawn_pushes) {
                    const int pawn_push_square = getLeastBitboardSquare(pawn_pushes); // for each push move of given pawn
                    if (pawn_push_square / 8 == (gs.turn == WHITE ? 7 : 0)) { // promotion block
                        addMove(moves_c, moves_v, Move(pawn_search_square, pawn_push_square, Move::PROMO::KNIGHT, false));
                        addMove(moves_c, moves_v, Move(pawn_search_square, pawn_push_square, Move::PROMO::BISHOP, false));
                        addMove(moves_c, moves_v, Move(pawn_search_square, pawn_push_square, Move::PROMO::ROOK, false));
                        addMove(moves_c, moves_v, Move(pawn_search_square, pawn_push_square, Move::PROMO::QUEEN, false));
                    }
                    else {
                        addMove(moves_c, moves_v, Move(pawn_search_square, pawn_push_square, Move::PROMO::NONE, false)); // standard move
                    }
                    pawn_pushes &= pawn_pushes - 1;
                }
            }
            

            U64 en_passant_square = (gs.en_passant != -1) ? squareToBitboard(gs.en_passant) : 0;
            U64 en_passant_capture_square = (gs.en_passant != -1) ? squareToBitboard(gs.en_passant + (gs.turn == WHITE ? -8 : 8)) : 0;
            U64 pawn_captures = Bitboards::pawn_attacks[pawn_search_square][gs.turn] & ((gs.occupied_spaces_color[!gs.turn] | en_passant_square) & (check_evasion_bitboard));
            if ((en_passant_capture_square & check_evasion_bitboard) && (Bitboards::pawn_attacks[pawn_search_square][gs.turn] & en_passant_square)) pawn_captures |= en_passant_square; // taking en passant after the double pawn push never results in a block (from rook or bishop), so this is okay
            if (is_pinned) pawn_captures &= pin_data.allowed_moves[pawn_search_square]; // pins

            while (pawn_captures) {
                const int pawn_capture_square = getLeastBitboardSquare(pawn_captures);
                const bool is_en_passant = (gs.en_passant != -1) && (pawn_capture_square == gs.en_passant);
                if (pawn_capture_square / 8 == (gs.turn == WHITE ? 7 : 0)) {
                    addMove(moves_c, moves_v, Move(pawn_search_square, pawn_capture_square, Move::PROMO::KNIGHT, true));
                    addMove(moves_c, moves_v, Move(pawn_search_square, pawn_capture_square, Move::PROMO::BISHOP, true));
                    addMove(moves_c, moves_v, Move(pawn_search_square, pawn_capture_square, Move::PROMO::ROOK, true));
                    addMove(moves_c, moves_v, Move(pawn_search_square, pawn_capture_square, Move::PROMO::QUEEN, true));
                }
                else {
                    if (is_en_passant && (Bitboards::rows[gs.turn == WHITE ? 4 : 3] & gs.pieces[gs.turn][KING])) { // if enpassant and the same row contains the friendly king
                        bool is_legal = true;

                        U64 occupied_spaces_substitute = gs.occupied_spaces;
                        occupied_spaces_substitute ^= squareToBitboard(pawn_search_square) | squareToBitboard(pawn_capture_square + (gs.turn == WHITE ? -8 : 8)); // remove theoritical pawn move & capture square
                        
                        const int king_location = getLeastBitboardSquare(gs.pieces[gs.turn][KING]); // assumes one king
                        for (int d = 2; d < 4; ++d) {
                            U64 ray = Bitboards::rook_rays[king_location][d]; // (N, S,) E, W
                            U64 blockers = ray & occupied_spaces_substitute;

                            if (blockers) {
                                int blocker_square = (d % 2 == 0) ? getLeastBitboardSquare(blockers) : getMostBitboardSquare(blockers);
                                if (squareToBitboard(blocker_square) & (gs.pieces[!gs.turn][ROOK] | gs.pieces[!gs.turn][QUEEN])) {
                                    is_legal = false;
                                    break;
                                }
                            }
                        }
                        if (is_legal) addMove(moves_c, moves_v, Move(pawn_search_square, pawn_capture_square, Move::PROMO::NONE, true));
                    } else {
                        addMove(moves_c, moves_v, Move(pawn_search_square, pawn_capture_square, Move::PROMO::NONE, true));
                    }

                }

                pawn_captures &= pawn_captures - 1;
            }
            pawn_bitboard &= pawn_bitboard - 1; // remove rightmost 1 bit
        }
    }
    static inline void genKnights(const GameState & gs, unsigned int & moves_c, Move * moves_v, const U64 check_evasion_bitboard, const PinData & pin_data, bool gen_only_captures) {
        U64 knight_bitboard = gs.pieces[gs.turn][KNIGHT];
        while (knight_bitboard) {
            const int knight_search_square = getLeastBitboardSquare(knight_bitboard); // for each pawn of gs.turn color
            const bool is_pinned = squareToBitboard(knight_search_square) & pin_data.pins; // pins
            
            
            if (!gen_only_captures) {
                U64 knight_moves = Bitboards::knight_moves[knight_search_square] & ~gs.occupied_spaces & check_evasion_bitboard;
                if (is_pinned) knight_moves &= pin_data.allowed_moves[knight_search_square];
                while (knight_moves) {
                    addMove(moves_c, moves_v, Move(knight_search_square, getLeastBitboardSquare(knight_moves), Move::PROMO::NONE, false));
                    knight_moves &= knight_moves - 1;
                }
            }

            U64 knight_captures = Bitboards::knight_moves[knight_search_square] & gs.occupied_spaces_color[!gs.turn] & check_evasion_bitboard;
            if (is_pinned) knight_captures &= pin_data.allowed_moves[knight_search_square];
            while (knight_captures) {
                const int knight_capture_square = getLeastBitboardSquare(knight_captures);
                addMove(moves_c, moves_v, Move(knight_search_square, knight_capture_square, Move::PROMO::NONE, true));
                knight_captures &= knight_captures - 1;
            }
            knight_bitboard &= knight_bitboard - 1;
        }
    }
    static inline void genBishops(const GameState & gs, unsigned int & moves_c, Move * moves_v, const U64 check_evasion_bitboard, const PinData & pin_data, bool gen_only_captures) {
        U64 bishop_bitboard = gs.pieces[gs.turn][BISHOP];
        while (bishop_bitboard) {
            const int bishop_search_square = getLeastBitboardSquare(bishop_bitboard); // for each pawn of gs.turn color
            const bool is_pinned = squareToBitboard(bishop_search_square) & pin_data.pins; // pins
            const U64 bishop_controlled_squares = genBishopRays(bishop_search_square, gs.occupied_spaces);
            
            if (!gen_only_captures) {
                U64 bishop_moves = bishop_controlled_squares & ~gs.occupied_spaces & check_evasion_bitboard;
                if (is_pinned) bishop_moves &= pin_data.allowed_moves[bishop_search_square];
                while (bishop_moves) {
                    addMove(moves_c, moves_v, Move(bishop_search_square, getLeastBitboardSquare(bishop_moves), Move::PROMO::NONE, false));
                    bishop_moves &= bishop_moves - 1;
                }
            }

            U64 bishop_captures = bishop_controlled_squares & gs.occupied_spaces_color[!gs.turn] & check_evasion_bitboard;
            if (is_pinned) bishop_captures &= pin_data.allowed_moves[bishop_search_square];
            while (bishop_captures) {
                const int bishop_capture_square = getLeastBitboardSquare(bishop_captures);
                addMove(moves_c, moves_v, Move(bishop_search_square, bishop_capture_square, Move::PROMO::NONE, true));
                bishop_captures &= bishop_captures - 1;
            }
            bishop_bitboard &= bishop_bitboard - 1;
        }
    }
    static inline void genRooks(const GameState & gs, unsigned int & moves_c, Move * moves_v, const U64 check_evasion_bitboard, const PinData & pin_data, bool gen_only_captures) {
        U64 rook_bitboard = gs.pieces[gs.turn][ROOK];
        while (rook_bitboard) {
            const int rook_search_square = getLeastBitboardSquare(rook_bitboard); // for each pawn of gs.turn color
            const bool is_pinned = squareToBitboard(rook_search_square) & pin_data.pins; // pins
            const U64 rook_controlled_squares = genRookRays(rook_search_square, gs.occupied_spaces);
            
            if (!gen_only_captures) {
                if (!gen_only_captures) {
                    U64 rook_moves = rook_controlled_squares & ~gs.occupied_spaces & check_evasion_bitboard;
                    if (is_pinned) rook_moves &= pin_data.allowed_moves[rook_search_square];
                    while (rook_moves) {
                        addMove(moves_c, moves_v, Move(rook_search_square, getLeastBitboardSquare(rook_moves), Move::PROMO::NONE, false));
                        rook_moves &= rook_moves - 1;
                    }
                }
            }

            U64 rook_captures = rook_controlled_squares & gs.occupied_spaces_color[!gs.turn] & check_evasion_bitboard;
            if (is_pinned) rook_captures &= pin_data.allowed_moves[rook_search_square];
            while (rook_captures) {
                const int rook_capture_square = getLeastBitboardSquare(rook_captures);
                addMove(moves_c, moves_v, Move(rook_search_square, rook_capture_square, Move::PROMO::NONE, true));
                rook_captures &= rook_captures - 1;
            }
            rook_bitboard &= rook_bitboard - 1;
        }
    }
    static inline void genQueens(const GameState & gs, unsigned int & moves_c, Move * moves_v, const U64 check_evasion_bitboard, const PinData & pin_data, bool gen_only_captures) {
        U64 queen_bitboard = gs.pieces[gs.turn][QUEEN];
        while (queen_bitboard) {
            const int queen_search_square = getLeastBitboardSquare(queen_bitboard); // for each pawn of gs.turn color
            const bool is_pinned = squareToBitboard(queen_search_square) & pin_data.pins; // pins
            const U64 queen_controlled_squares = genBishopRays(queen_search_square, gs.occupied_spaces) | genRookRays(queen_search_square, gs.occupied_spaces);
            
            if (!gen_only_captures) {
                U64 queen_moves = queen_controlled_squares & ~gs.occupied_spaces & check_evasion_bitboard;
                if (is_pinned) queen_moves &= pin_data.allowed_moves[queen_search_square];
                while (queen_moves) {
                    addMove(moves_c, moves_v, Move(queen_search_square, getLeastBitboardSquare(queen_moves), Move::PROMO::NONE, false));
                    queen_moves &= queen_moves - 1;
                }
            }

            U64 queen_captures = queen_controlled_squares & gs.occupied_spaces_color[!gs.turn] & check_evasion_bitboard;
            if (is_pinned) queen_captures &= pin_data.allowed_moves[queen_search_square];
            while (queen_captures) {
                const int queen_capture_square = getLeastBitboardSquare(queen_captures);
                addMove(moves_c, moves_v, Move(queen_search_square, queen_capture_square, Move::PROMO::NONE, true));
                queen_captures &= queen_captures - 1;
            }
            queen_bitboard &= queen_bitboard - 1;
        }
    }
    static inline void genKing(const GameState & gs, unsigned int & moves_c, Move * moves_v, const U64 enemy_controlled_squares, bool gen_only_captures) {
        const int king_square = getLeastBitboardSquare(gs.pieces[gs.turn][KING]);

        if (!gen_only_captures) {
            U64 king_moves = Bitboards::king_moves[king_square] & ~gs.occupied_spaces & ~enemy_controlled_squares;
            while (king_moves) {
                addMove(moves_c, moves_v, Move(king_square, getLeastBitboardSquare(king_moves), Move::PROMO::NONE, false));
                king_moves &= king_moves - 1;
            }
        }

        U64 king_captures = Bitboards::king_moves[king_square] & gs.occupied_spaces_color[!gs.turn] & ~enemy_controlled_squares;
        while (king_captures) {
            const int king_capture_square = getLeastBitboardSquare(king_captures);
            addMove(moves_c, moves_v, Move(king_square, king_capture_square, Move::PROMO::NONE, true));
            king_captures &= king_captures - 1;
        }

        if (!gen_only_captures) {
            constexpr U64 castle_K_clear_squares = Bitboards::between[4][7]; // f1, g1
            constexpr U64 castle_K_no_check_squares = Bitboards::between[3][7]; // e1, f1, g1

            constexpr U64 castle_Q_clear_squares = Bitboards::between[0][4]; // b1, c1, d1
            constexpr U64 castle_Q_no_check_squares = Bitboards::between[1][5]; // c1, d1, e1

            constexpr U64 castle_k_clear_squares = Bitboards::between[60][63]; // f8, g8
            constexpr U64 castle_k_no_check_squares = Bitboards::between[59][63]; // e8, f8, g8

            constexpr U64 castle_q_clear_squares = Bitboards::between[56][60]; // b8, c8, d8
            constexpr U64 castle_q_no_check_squares = Bitboards::between[57][61]; // c8, d8, e8

            // castling kingside
            if ((gs.turn == WHITE ? gs.castle_K : gs.castle_k) &&
                (((gs.turn == WHITE ? castle_K_clear_squares : castle_k_clear_squares) & gs.occupied_spaces) == 0) &&
                (((gs.turn == WHITE ? castle_K_no_check_squares : castle_k_no_check_squares) & enemy_controlled_squares) == 0)) {
                    addMove(moves_c, moves_v, Move(king_square, king_square + 2, Move::PROMO::NONE, false));
            }

            // castling queenside
            if ((gs.turn == WHITE ? gs.castle_Q : gs.castle_q) &&
                (((gs.turn == WHITE ? castle_Q_clear_squares : castle_q_clear_squares) & gs.occupied_spaces) == 0) &&
                (((gs.turn == WHITE ? castle_Q_no_check_squares : castle_q_no_check_squares) & enemy_controlled_squares) == 0)) {
                    addMove(moves_c, moves_v, Move(king_square, king_square - 2, Move::PROMO::NONE, false));
            }
        }
   
    }


};
}