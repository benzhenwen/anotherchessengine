#pragma once

#include <vector>
#include <stack>
#include <cctype>

#include <lib/chess/gamestate.hpp>
#include <lib/chess/move.hpp>
#include <lib/chess/movegenerator.hpp>
#include <lib/chess/unmove.hpp>

#include <lib/chess/fen.hpp>
#include <lib/chess/ascii.hpp>

#include <logger/logger.hpp>

namespace Chess {
class Game {
private:
    static constexpr Logger logger = Logger("GAME");

protected:
    // game info
    GameState game_state;

    // avaliable moves
    unsigned int moves_c;
    Move moves[256];
    std::vector<std::string> available_formatted_moves;

    // move history
    std::stack<Unmove> undo_stack = std::stack<Unmove>();

public:
    Game(GameState load_game_state = FEN::FENToGameState("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")):
        game_state(load_game_state)
    {
        updateAvaliableMoves();
    }

    const GameState & getGameState() {
        return game_state;
    }

    // gameloop
    bool makeMove(unsigned int move_index) { // returns true if successful
        if (move_index < 0 || move_index >= moves_c) return false;

        // apply move and undo stack
        undo_stack.push(game_state.get_unmove(moves[move_index]));
        game_state.applyMove(moves[move_index]);
        updateAvaliableMoves();
        return true;
    }

    bool makeMove(const std::string & move_notated) { // accepts formatted moves (like e4, e5, Kf3, Kc6)
        int move_index = findMoveFormatted(move_notated);
        if (move_index == -1) return false;
        return makeMove(move_index);
    }

    const std::vector<std::string> & getAvailableFormattedMoves() {
        return available_formatted_moves;
    }

    bool undo_move() {
        if (undo_stack.size() == 0) return false;
        game_state.applyUnmove(undo_stack.top());
        undo_stack.pop();
        updateAvaliableMoves();
        return true;
    }


protected:
    void updateAvaliableMoves() {
        moves_c = MoveGenerator::genAllMoves(game_state, moves);
        available_formatted_moves = Ascii::formatMoveList(moves_c, moves);
    }

    int findMoveFormatted(const std::string & move_str) {
        for (size_t i = 0; i < available_formatted_moves.size(); ++i) {
            if (stringEqualsCaseInsensitive(available_formatted_moves[i], move_str)) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    bool stringEqualsCaseInsensitive(const std::string & s1, const std::string & s2) {
        if (s1.length() != s2.length()) {
            return false;
        }
        return std::equal(s1.begin(), s1.end(), s2.begin(), [](char a, char b) {
            return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
        });
    }
};
}