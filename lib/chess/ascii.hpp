#pragma once

#include <unordered_map>
#include <vector>

#include <lib/chess/gamestate.hpp>
#include <lib/chess/move.hpp>

namespace Chess::Ascii {
    inline char squareColChar(int sq) {
        return (char) ('a' + squareCol(sq));
    }
    std::string squareToLetterNumber(int square) {
        return (char) ('a' + (square%8)) + std::to_string(square/8 + 1);
    }

    std::string gameStateToAscii(GameState gs) {
        // board
        std::stringstream ss;

        for (int row = 7; row >= 0; row--) {
            ss << row + 1 << "   ";
            for (int col = 0; col < 8; col++) {
                ss << pieceAsciiAtSquare(gs.pieces, row*8 + col);
                if (col < 8) ss << ' ';
            }
            ss << '\n';
        }

        ss << '\n';
        ss << "*   a b c d e f g h\n";
        ss << '\n';

        ss << "move: " << (gs.turn ? "Black" : "White") << "\n";

        ss << "castle: ";
        if (!gs.castle_K && !gs.castle_Q && !gs.castle_k && !gs.castle_q) ss << '-';
        else {
            if (gs.castle_K) ss << 'K';
            if (gs.castle_Q) ss << 'Q';
            if (gs.castle_k) ss << 'k';
            if (gs.castle_q) ss << 'q';
        }
        ss << '\n';

        ss << "en passant: ";
        if (gs.en_passant == -1) ss << '-';
        else ss << squareToLetterNumber(gs.en_passant);
        ss << '\n';

        ss << "halfmove clock: " << gs.halfmove_clock << '\n';
        ss << "fullmove clock: " << gs.fullmove_clock;

        return ss.str();
    }

    std::string bitboardToAscii(U64 bb) {
        std::stringstream ss;

        for (int row = 7; row >= 0; row--) {
            ss << row + 1 << "   ";
            for (int col = 0; col < 8; col++) {
                ss << (char) ((bb & squareToBitboard(row*8 + col)) ? '#' : '-');
                if (col < 8) ss << ' ';
            }
            ss << '\n';
        }

        ss << '\n';
        ss << "*   a b c d e f g h\n";

        return ss.str();
    }

    // i just let gpt-5 generate this one ngl
    std::vector<std::string> formatMoveList(int moves_c, Move * moves) {
        std::vector<std::string> out;
        out.reserve(moves_c);

        // Group by (piece type, destination) for SAN disambiguation
        struct Key     { int pt; int to; };
        struct KeyHash { size_t operator()(const Key& k) const noexcept { return (size_t)((k.pt<<6) ^ k.to); } };
        struct KeyEq   { bool operator()(const Key& a, const Key& b) const noexcept { return a.pt==b.pt && a.to==b.to; } };

        std::unordered_map<Key, std::vector<int>, KeyHash, KeyEq> groups;
        groups.reserve(moves_c);
        for (int i = 0; i < (int)moves_c; ++i) {
            const auto& m = moves[i];
            if (m.is_castle) continue;               // castles never need disambiguation
            if (m.starting_piece == PAWN) continue;    // pawns use file on capture, not SAN piece disambig
            groups[{(int)m.starting_piece, m.to}].push_back(m.from);
        }

        auto disambig = [&](const std::vector<int>& froms, int my_from) -> std::string {
            if (froms.size() <= 1) return "";
            bool files_diff = false, ranks_diff = false;
            int f0 = squareCol(froms.front());
            int r0 = squareRow(froms.front());
            for (int sq : froms) {
                if (squareCol(sq) != f0) files_diff = true;
                if (squareRow(sq) != r0) ranks_diff = true;
                if (files_diff && ranks_diff) break;
            }
            char file = (char) ('a' + squareCol(my_from));
            char rank = (char) ('1' + squareRow(my_from));
            if (files_diff) return std::string(1, file);
            if (ranks_diff) return std::string(1, rank);
            return std::string{file} + std::string{rank};
        };

        for (int i = 0; i < moves_c; i++) {
            auto m = moves[i];
            std::stringstream ss;

            // Castling
            if (m.is_castle) {
                int from_f = squareCol(m.from), to_f = squareCol(m.to);
                ss << ((to_f > from_f) ? "O-O" : "O-O-O");
                out.push_back(ss.str());
                continue;
            }

            const bool is_pawn = (m.starting_piece == PAWN);

            // Piece letter (pawns omit)
            if (!is_pawn) {
                switch (m.starting_piece) {
                    case KNIGHT: ss << 'N'; break;
                    case BISHOP: ss << 'B'; break;
                    case ROOK:   ss << 'R'; break;
                    case QUEEN:  ss << 'Q'; break;
                    case KING:   ss << 'K'; break;
                    default: break;
                }
                auto it = groups.find({(int)m.starting_piece, m.to});
                if (it != groups.end()) ss << disambig(it->second, m.from);
            }

            // Capture marker; pawns print file letter when capturing
            bool is_capture = (m.capture != Move::CAPTURE::NONE) || m.is_en_passant;
            if (is_pawn && is_capture) ss << Ascii::squareColChar(m.from);
            if (is_capture) ss << 'x';

            // Destination square
            ss << Ascii::squareToLetterNumber(m.to);

            // Promotion
            if (is_pawn && m.ending_piece != PAWN) {
                char promo = 'Q';
                switch (m.ending_piece) {
                    case KNIGHT: promo = 'N'; break;
                    case BISHOP: promo = 'B'; break;
                    case ROOK:   promo = 'R'; break;
                    case QUEEN:  promo = 'Q'; break;
                    default: break;
                }
                ss << '=' << promo;
            }

            // EP tag
            // if (m.is_en_passant) ss << " e.p.";

            out.push_back(ss.str());
        }

        return out;
    }
}