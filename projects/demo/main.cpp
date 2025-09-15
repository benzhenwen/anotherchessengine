#include <logger/logger.hpp>

#include <vector>
#include <chrono>
#include <iostream>

#include <lib/chess/game.hpp>
#include <lib/chess/ascii.hpp>
#include <lib/chess/fen.hpp>

#include <lib/chess/engine/engine.hpp>


Chess::Game game;
bool in_game = false;

Chess::Engine::Engine engine;
int engine_depth = 6;

void run_command(const std::string & command) {
    if (command == "help") {
        std::cout   << "Commands are:\n"
                    << "!help - get list of all commands\n"
                    << "!start - starts a new game, overrides your current game\n"
                    << "!moves - shows all your currently avaliable moves\n"
                    << "!fen - export your current game state to a FEN string\n" 
                    << "!board - print the board state again\n" 
                    << "!undo - undo your previous move\n"
                    << "!engine - get the engine moves\n" << std::flush;
    }
    else if (command == "start") {
        game = Chess::Game();
        std::cout << Chess::Ascii::gameStateToAscii(game.getGameState()) << std::endl;
        in_game = true;
    }
    else if (command == "moves") {
        if (!in_game) {
            std::cout << "you need to be in an active game to use this" << std::endl;
            return;
        }
        std::cout << "moves:\n";
        for (const std::string & move_str : game.getAvailableFormattedMoves()) {
            std::cout << move_str << '\n';
        }
        std::cout << std::flush;
    }
    else if (command == "fen") {
        if (!in_game) {
            std::cout << "you need to be in an active game to use this" << std::endl;
            return;
        }
        std::cout << Chess::FEN::gameStateToFEN(game.getGameState()) << std::endl;
    }
    else if (command == "board") {
        if (!in_game) {
            std::cout << "you need to be in an active game to use this" << std::endl;
            return;
        }
        std::cout << Chess::Ascii::gameStateToAscii(game.getGameState()) << std::endl;
    }
    else if (command == "undo") {
        if (!in_game) {
            std::cout << "you need to be in an active game to use this" << std::endl;
            return;
        }
        if (game.undo_move()) {
            std::cout << "Undid move" << std::endl;
            std::cout << Chess::Ascii::gameStateToAscii(game.getGameState()) << std::endl;
        }
        else {
            std::cout << "could not perform undo" << std::endl;
        }
    }
    else if (command == "engine") {
        if (!in_game) {
            std::cout << "you need to be in an active game to use this" << std::endl;
            return;
        }
        std::cout << "starting engine" << std::endl;

        auto start = std::chrono::high_resolution_clock::now();

        auto results = engine.evaluateAllMoves(game.getGameState(), engine_depth);

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = finish - start;

        for (const auto & result : results) {
            std::cout << result.move.toString() << " -> " << result.score << std::endl;
        }
        std::cout << "in: " << elapsed.count() << "ms" << std::endl;
    }
    else {
        std::cout << "unknown command" << std::endl;
    }
}


int main(int argc, char* argv[]) {
    std::cout   << "Welcome to chess!!\n"
                << "play a game against yourself (you make both moves)\n"
                << "Type !start to begin a game, then simply type out the move you want to play in algebreic notation\n"
                << "See !help for more commands and detailed information\n";

    std::string input;
    while (true) {
        std::cout << "  >> " << std::flush;
        std::cin >> input;

        if (input.length() == 0) continue;
        if (input.at(0) == '!') {
            run_command(input.substr(1));
            continue;
        } else {
            if (!in_game) {
                std::cout << "you can't make a move without being in a game!\n";
                continue;
            } else {
                if (!game.makeMove(input)) {
                    std::cout << "you can't make that move!\n";
                } else {
                    std::cout << Chess::Ascii::gameStateToAscii(game.getGameState()) << std::endl;
                }
            }
        }
    }

    return 0;
}