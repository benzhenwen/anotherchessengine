#include <logger/logger.hpp>

#include <chrono>

#include <lib/chess/gamestate.hpp>
#include <lib/chess/fen.hpp>
#include <lib/chess/move.hpp>
#include <lib/chess/unmove.hpp>

#include <lib/chess/engine/negamax.hpp>

int main(int argc, char* argv[]) {
    static constexpr Logger logger = Logger("MAIN");

    if(argc != 2) {
        logger.log(Logger::WARNING) << "Please provide an int argument for depth";
        return 0;
    }
    char *p;
    errno = 0;
    long depth = strtol(argv[1], &p, 10);

    if (errno != 0 || *p != '\0' || depth > 100 || depth < 0) {
        logger.log(Logger::WARNING) << "Please provide an int argument for depth (0 - 100)";
        return 0;
    } 

    // Chess::GameState gs = Chess::FEN::FENToGameState("5k2/4pppp/8/8/4Q3/8/8/4R2K w - - 4 3");
    Chess::GameState gs = Chess::FEN::FENToGameState("4r1k1/1ppq1p2/p2b1nnQ/3P1b2/1BB5/P1N2N1P/1P3PP1/4R1K1 b - - 0 18");

    auto start = std::chrono::high_resolution_clock::now();

    auto results = Chess::Engine::Negamax::evaluateAllMoves(gs, depth);

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = finish - start;

    for (const auto & result : results) {
        logger << result.move.toString() << " -> " << result.score;
    }

    logger << "in: " << elapsed.count() << "ms";

    logger << "qcnt: " << Chess::Engine::Negamax::qcnt;
    logger << "ncnt: " << Chess::Engine::Negamax::ncnt;

    return 0;
}