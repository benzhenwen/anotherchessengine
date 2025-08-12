#include <logger/logger.hpp>

#include <chrono>

#include <lib/chess/gamestate.hpp>
#include <lib/chess/fen.hpp>
#include <lib/chess/move.hpp>
#include <lib/chess/unmove.hpp>

#include <lib/chess/engine/engine.hpp>

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

    Chess::Engine::Engine engine = Chess::Engine::Engine();

    Chess::GameState gs = Chess::FEN::FENToGameState("3Q2k1/pp4p1/1n6/2p4p/2Pb4/P7/3BrPPP/6K1 b - - 7 36");
    // Chess::GameState gs = Chess::FEN::FENToGameState("3r1rk1/ppqn1ppp/2p1p3/2P5/2BP4/P2Q1P1P/5PP1/3RR1K1 b - - 4 18");

    auto start = std::chrono::high_resolution_clock::now();

    auto results = engine.evaluateAllMoves(gs, depth);

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