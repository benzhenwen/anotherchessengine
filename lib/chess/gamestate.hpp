#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <cstring>
#include <cassert>

#include <iostream>

#include <lib/chess/util.hpp>
#include <lib/chess/move.hpp>
#include <lib/chess/unmove.hpp>

#include <lib/lookuptables/zobristhashes.hpp>

namespace Chess {

class GameState {
public:
    // bitboards
    U64 pieces[2][6] = { 0 }; // [color][type];

    // cached aggregates
    U64 occupied_spaces = { 0 }; 
    U64 occupied_spaces_color[2] = { 0 }; 

    // data
    COLOR turn = WHITE;
    bool castle_K = false, castle_Q = false, castle_k = false, castle_q = false;
    int en_passant = -1; // -1 = none, 0-63 for square
    unsigned int halfmove_clock = 0;
    unsigned int fullmove_clock = 0;

protected:
    U64 hash_code = Chess::ZobristHashes::castle_codes[0][0][0][0];


public:
    constexpr GameState() = default;

    GameState(const GameState & other):
    turn(other.turn),
    castle_K(other.castle_K),
    castle_Q(other.castle_Q),
    castle_k(other.castle_k),
    castle_q(other.castle_q),
    en_passant(other.en_passant),
    halfmove_clock(other.halfmove_clock),
    fullmove_clock(other.fullmove_clock),
    hash_code(other.hash_code)
    {
        // bitboards
        std::memcpy(pieces, other.pieces, sizeof(pieces));

        // cached aggregates
        occupied_spaces = other.occupied_spaces;
        std::memcpy(occupied_spaces_color, other.occupied_spaces_color, sizeof(occupied_spaces_color));
    }


// board manipulation & hashcode handling
public:
    inline void alternateTurn() {
        turn = (COLOR) !(bool)turn;
        hash_code ^= Chess::ZobristHashes::black_move_code;
    }

    inline void addPiece(const int position, const COLOR piece_color, const PIECE piece_type) { // NOT HASH SAFE, do not use to replace a piece
        /* FOR DEBUG */ assert((occupied_spaces & squareToBitboard(position)) == 0); // assert square empty
        
        pieces[piece_color][piece_type]    |= squareToBitboard(position);
        occupied_spaces                    |= squareToBitboard(position);
        occupied_spaces_color[piece_color] |= squareToBitboard(position);

        hash_code ^= Chess::ZobristHashes::piece_codes[position][piece_type][piece_color]; 
    }

    inline void removePiece(const int position, const COLOR piece_color, const PIECE piece_type) { // NOT HASH SAFE, do not use to clear an empty square or mis-clear
        /* FOR DEBUG */ assert((pieces[piece_color][piece_type] & squareToBitboard(position)) != 0); // assert piece is there
        
        pieces[piece_color][piece_type]    &= ~squareToBitboard(position);
        occupied_spaces                    &= ~squareToBitboard(position);
        occupied_spaces_color[piece_color] &= ~squareToBitboard(position);
        
        hash_code ^= Chess::ZobristHashes::piece_codes[position][piece_type][piece_color]; 
    }

private:
    inline void addPiece_noHashUpdate(const int position, const COLOR piece_color, const PIECE piece_type) { // NOT HASH SAFE, do not use to replace a piece
        /* FOR DEBUG */ assert((occupied_spaces & squareToBitboard(position)) == 0); // assert square empty
        
        pieces[piece_color][piece_type]    |= squareToBitboard(position);
        occupied_spaces                    |= squareToBitboard(position);
        occupied_spaces_color[piece_color] |= squareToBitboard(position);
    }

    inline void removePiece_noHashUpdate(const int position, const COLOR piece_color, const PIECE piece_type) { // NOT HASH SAFE, do not use to clear an empty square or mis-clear
        /* FOR DEBUG */ assert((pieces[piece_color][piece_type] & squareToBitboard(position)) != 0); // assert piece is there
        
        pieces[piece_color][piece_type]    &= ~squareToBitboard(position);
        occupied_spaces                    &= ~squareToBitboard(position);
        occupied_spaces_color[piece_color] &= ~squareToBitboard(position);
    }

public:
    inline void setCastleRights(bool cK, bool cQ, bool ck, bool cq) {
        hash_code ^= Chess::ZobristHashes::castle_codes[castle_K][castle_Q][castle_k][castle_q];
        hash_code ^= Chess::ZobristHashes::castle_codes[cK][cQ][ck][cq];
        castle_K = cK;
        castle_Q = cQ;
        castle_k = ck;
        castle_q = cq;
    } 
    inline void setEnPassantSquare(int square) { // -1 to 7, where -1 is none and 0-7 are col values
        /* for debug */ assert(square >= -1 && square <= 63 && (squareRow(square) == 2 || squareRow(square) == 5 || square == -1));

        if (en_passant != -1) hash_code ^= Chess::ZobristHashes::en_passant_codes[squareCol(square)]; // clear the current value from hash if it's set
        if (square != -1) hash_code ^= Chess::ZobristHashes::en_passant_codes[squareCol(square)]; // set the new value assuming it's not none (in which case previous instruction makes it already accurate)
        en_passant = square;
    }

// comparison
public:
    // equality
    inline bool operator==(const GameState & other) const {
        return  std::memcmp(pieces, other.pieces, sizeof(pieces)) == 0 &&
                turn == other.turn &&
                castle_K == other.castle_K &&
                castle_Q == other.castle_Q &&
                castle_k == other.castle_k &&
                castle_q == other.castle_q &&
                en_passant == other.en_passant;
    }

    // hash
    inline U64 operator()(const GameState & key) const {
        return key.hash_code;
    }

    inline U64 getHashCode() const {
        return hash_code;
    }

    inline void recalculateHashCode() {
        hash_code = 0;
        for (int c = WHITE; c <= BLACK; c++) {
            for (int pt = PAWN; pt <= KING; pt++) {
                U64 bitboard = pieces[c][pt];
                while (bitboard) {
                    int sq = __builtin_ctzll(bitboard);
                    hash_code ^= Chess::ZobristHashes::piece_codes[sq][pt][c];
                    bitboard &= bitboard - 1;
                }
            }
        }

        if (turn == BLACK) hash_code ^= Chess::ZobristHashes::black_move_code;
        hash_code ^= Chess::ZobristHashes::castle_codes[castle_K][castle_Q][castle_k][castle_q];
        if (en_passant != -1) hash_code ^= Chess::ZobristHashes::en_passant_codes[en_passant];
    }

    // util
public:
    inline PIECE getPieceTypeAtSquare(int square, COLOR color) const {
        U64 bit = squareToBitboard(square); // returns 1ULL << square

        for (int pt = PAWN; pt <= KING; pt++) {
            if (pieces[color][pt] & bit) {
                return static_cast<PIECE>(pt);
            }
        }

        // Square is either empty or contains a piece of the opposite color
        assert(false);
    }

public:
    inline bool isMoveCastle(const Move & m) const {
        return (pieces[turn][KING] & squareToBitboard(m.from())) && (std::abs(m.from() - m.to()) == 2); // king moved horizontally 2 squares
    }
    inline bool isMoveEnPassant(const Move & m) const {
        if (en_passant == -1) return false;
        return (pieces[turn][PAWN] & squareToBitboard(m.from())) && (en_passant == m.to()); // pawn moved and it was into the en passant square (pawns cannot push into ep square as the square before is always occupied by an opposing pawn)
    }

// moves
public:
    inline Unmove applyMove(const Move & move) { // returns the unmove mirror of move
        const PIECE starting_piece = getPieceTypeAtSquare(move.from(), turn);
        const PIECE ending_piece = (starting_piece == PAWN) ? move.promo_piece() : starting_piece;

        const int capture_square = move.isCapture() ? (move.to() + (isMoveEnPassant(move) ? ((turn == WHITE) ? -8 : 8) : 0)) : 0;

        Unmove um = Unmove(
            move.from(), move.to(), 
            starting_piece, ending_piece, 
            (move.isCapture() ? 
                (isMoveEnPassant(move) ? Unmove::CAPTURE::PAWN : (Unmove::CAPTURE) getPieceTypeAtSquare(capture_square, (COLOR) !turn)) 
                : Unmove::CAPTURE::NONE),
            isMoveEnPassant(move),
            isMoveCastle(move),
            castle_K, castle_Q, castle_k, castle_q, 
            en_passant, 
            halfmove_clock, fullmove_clock, 
            turn, 
            hash_code
        );

        // capture
        if (move.isCapture()) {
            removePiece(capture_square, (COLOR) !turn, getPieceTypeAtSquare(capture_square, (COLOR) !turn)); // remove opposing color piece at moving to square
        }

        // move part of move
        removePiece(move.from(), turn, starting_piece);
        addPiece(move.to(), turn, ending_piece);

        if (um.is_castle) {
            if (move.from() < move.to()) { // kingside castle
                removePiece((turn == WHITE) ? 7 : 63, turn, ROOK); // remove rook from h1 / h8
                addPiece((turn == WHITE) ? 5 : 61, turn, ROOK); // add rook to f1 / f8
            } else { // queenside castle
                removePiece((turn == WHITE) ? 0 : 56, turn, ROOK); // remove rook from a1 / a8
                addPiece((turn == WHITE) ? 3 : 59, turn, ROOK); // add rook to d1 / d8
            }
        }

        // en passant enable
        if (starting_piece == PAWN && std::abs(move.from() - move.to()) == 16) { // if pawn move and moved 2 squres vertically (postion changed by 16)
            setEnPassantSquare((turn == WHITE) ? (move.from() + 8) : (move.from() - 8));
        } else {
            setEnPassantSquare(-1);
        }

        assert (squareRow(en_passant) == 2 || squareRow(en_passant) == 5 || en_passant == -1);

        // castle disable
        if (castle_K || castle_Q || castle_k || castle_q) {
            bool temp_cK = castle_K;
            bool temp_cQ = castle_Q;
            bool temp_ck = castle_k;
            bool temp_cq = castle_q;
            switch (move.from()) {
                case 4: // if move from e1 (king square) no white castling
                    temp_cK = false;
                    temp_cQ = false;
                    break;
                case 60: // if move from e8 (king square) no black castling
                    temp_ck = false;
                    temp_cq = false;
                    break;
                case 0: // if move from a1, no white queenside
                    temp_cQ = false;
                    break;
                case 7: // if move from h1, no white kingside
                    temp_cK = false;
                    break;
                case 56: // if move from a8, no black queenside
                    temp_cq = false;
                    break;
                case 63: // if move from h8, no kingside kingside
                    temp_ck = false;
                    break;
            }
            switch (move.to()) {
                case 0: // if capture to a1, no white queenside
                    temp_cQ = false;
                    break;
                case 7: // if capture to h1, no white kingside
                    temp_cK = false;
                    break;
                case 56: // if capture to a8, no black queenside
                    temp_cq = false;
                    break;
                case 63: // if capture to h8, no kingside kingside
                    temp_ck = false;
                    break;
            }
            if (castle_K != temp_cK || castle_Q != temp_cQ || castle_k != temp_ck || castle_q != temp_cq) { // check for differences as setCastleRights() does not and is expensive
                setCastleRights(temp_cK, temp_cQ, temp_ck, temp_cq);
            }
        }

        // clocks
        if (move.isCapture() || starting_piece == PAWN) {
            halfmove_clock = 0;
        } else {
            halfmove_clock++;
        }

        if (turn == BLACK) fullmove_clock++;

        turn = (COLOR) !turn; // next move

        return um;
    }


    inline void applyUnmove(const Unmove & unmove) { 
        
        // The side that made the move
        const COLOR mover = unmove.turn_before;

        // Undo special rook shift if castling
        if (unmove.is_castle) {

            if (unmove.from < unmove.to) { // king-side
                // rook f1/f8 -> h1/h8
                removePiece_noHashUpdate((mover == WHITE) ? 5 : 61, mover, ROOK);
                addPiece_noHashUpdate((mover == WHITE) ? 7 : 63, mover, ROOK);
            } else {             // queen-side
                // rook d1/d8 -> a1/a8
                removePiece_noHashUpdate((mover == WHITE) ? 3 : 59, mover, ROOK);
                addPiece_noHashUpdate((mover == WHITE) ? 0 : 56, mover, ROOK);
            }
        }

        // Move (or un-promote) the mover's piece back: to -> from
        removePiece_noHashUpdate(unmove.to, mover, unmove.ending_piece);
        addPiece_noHashUpdate(unmove.from, mover, unmove.starting_piece);

        // Restore captured piece (normal or en passant)
        if (unmove.captured_piece != Unmove::CAPTURE::NONE) {
            const int cap_sq = unmove.is_en_passant ? (unmove.to + (mover == WHITE ? -8 : +8)) : unmove.to;
            addPiece_noHashUpdate(cap_sq, (COLOR) !unmove.turn_before, (PIECE) unmove.captured_piece);
        }

        // Restore state
        castle_K = unmove.castle_K;
        castle_Q = unmove.castle_Q;
        castle_k = unmove.castle_k;
        castle_q = unmove.castle_q;
        en_passant = unmove.en_passant;
        halfmove_clock = unmove.halfmove_clock;
        fullmove_clock = unmove.fullmove_clock;

        // Restore metadata
        turn = unmove.turn_before;
        hash_code = unmove.hash_before; 

    }

};
}