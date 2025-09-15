#pragma once

#include <lib/chess/gamestate.hpp>
#include <lib/chess/ascii.hpp>

namespace Chess::FEN {
    GameState FENToGameState(const std::string & FEN_str) {
        GameState gs = GameState();

        int i = 0;
        char curr = FEN_str[i];
        
        // board setup
        int row = 7;
        int col = 0;

        do {
            if (curr == '/') {
                col = 0;
                row--;
                curr = FEN_str[++i];
            }
            if (curr >= '1' && curr <= '8') { // digit 1-8
                col += curr - '0'; // char to int
            }
            else { // must be a piece
                switch (curr) {
                    case 'P': gs.addPiece(row*8 + col, WHITE, PAWN); break;
                    case 'N': gs.addPiece(row*8 + col, WHITE, KNIGHT); break;
                    case 'B': gs.addPiece(row*8 + col, WHITE, BISHOP); break;
                    case 'R': gs.addPiece(row*8 + col, WHITE, ROOK); break;
                    case 'Q': gs.addPiece(row*8 + col, WHITE, QUEEN); break;
                    case 'K': gs.addPiece(row*8 + col, WHITE, KING); break;

                    case 'p': gs.addPiece(row*8 + col, BLACK, PAWN); break;
                    case 'n': gs.addPiece(row*8 + col, BLACK, KNIGHT); break;
                    case 'b': gs.addPiece(row*8 + col, BLACK, BISHOP); break;
                    case 'r': gs.addPiece(row*8 + col, BLACK, ROOK); break;
                    case 'q': gs.addPiece(row*8 + col, BLACK, QUEEN); break;
                    case 'k': gs.addPiece(row*8 + col, BLACK, KING); break;
                }
                col++;
            }

            curr = FEN_str[++i];
        } while (curr != ' ');

        // turn
        curr = FEN_str[++i]; // w or b for turn
        const COLOR newTurn = curr == 'w' ? WHITE : BLACK;
        if (gs.turn != newTurn) gs.alternateTurn();

        // castling
        i += 2;
        curr = FEN_str[i];

        bool cK = false;
        bool cQ = false;
        bool ck = false;
        bool cq = false;

        if (curr != '-') {
            do {
                switch (curr) {
                    case 'K': cK = true; break;
                    case 'Q': cQ = true; break;
                    case 'k': ck = true; break;
                    case 'q': cq = true; break;
                }
                curr = FEN_str[++i];
            } while (curr != ' ');
        }
        else { i++; }
        gs.setCastleRights(cK, cQ, ck, cq);

        // en passant
        curr = FEN_str[++i];
        if (curr != '-') {
            gs.setEnPassantSquare(curr - 'a' + ((gs.turn == WHITE) ? 40 : 16)); // map a-h char to 0-7 int
        }
        i++;

        // clocks
        const int second_last_space = FEN_str.find_last_of(' ');
        gs.halfmove_clock = std::stoi(FEN_str.substr(i+1, second_last_space - (i+1)));
        gs.fullmove_clock = std::stoi(FEN_str.substr(second_last_space+1));

        return gs;
    }



    std::string gameStateToFEN(GameState gs) {
        std::stringstream ss;

        // board
        int empty_count = 0;
        for (int row = 7; row >= 0; row--) {
            for (int col = 0; col < 8; col++) {
                if ((gs.occupied_spaces & squareToBitboard(row*8 + col)) == 0) empty_count++;
                else {
                    if (empty_count > 0) {
                        ss << empty_count;
                        empty_count = 0;
                    }
                    ss << pieceAsciiAtSquare(gs.pieces, row*8 + col);
                }
            }
            if (empty_count > 0) {
                ss << empty_count;
                empty_count = 0;
            }
            if (row != 0) ss << '/';
        }
        ss << ' ';

        // move
        ss << (gs.turn ? 'b' : 'w') << ' ';

        // castling
        if (!gs.castle_K && !gs.castle_Q && !gs.castle_k && !gs.castle_q) ss << '-';
        else {
            if (gs.castle_K) ss << 'K';
            if (gs.castle_Q) ss << 'Q';
            if (gs.castle_k) ss << 'k';
            if (gs.castle_q) ss << 'q';
        }
        ss << ' ';

        // en passant
        if (gs.en_passant == -1) ss << '-';
        else ss << Ascii::squareToLetterNumber(gs.en_passant);
        ss << ' ';

        // clocks
        ss << gs.halfmove_clock << ' ';
        ss << gs.fullmove_clock;

        return ss.str();
    }
}