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

    Chess::GameState gs = Chess::FEN::FENToGameState("r3k1r1/ppp2p1p/1qn5/1B1p1b2/1P3B1p/P1NP1P2/2P1N2P/R2QK2R w KQq - 1 15");
    // Chess::GameState gs = Chess::FEN::FENToGameState("3r1rk1/ppqn1ppp/2p1p3/2P5/2BP4/P2Q1P1P/5PP1/3RR1K1 b - - 4 18");

    auto start = std::chrono::high_resolution_clock::now();

    auto results = engine.evaluateAllMoves(gs, depth);

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = finish - start;

    for (const auto & result : results) {
        logger << result.move.toString() << " -> " << result.score;
    }

    logger << "in: " << elapsed.count() << "ms";

    logger << "qcnt:  " << Chess::Engine::Negamax::qcnt;
    logger << "ncnt:  " << Chess::Engine::Negamax::ncnt;
    logger << "tthit: " << Chess::Engine::Negamax::tthit << " (" << (int) (((float) Chess::Engine::Negamax::tthit / (Chess::Engine::Negamax::qcnt + Chess::Engine::Negamax::ncnt)) * 10000) / 100.0f << "%)";
    logger << "ttoff: " << Chess::Engine::Negamax::ttcut << " (" << (int) (((float) Chess::Engine::Negamax::ttcut / (Chess::Engine::Negamax::qcnt + Chess::Engine::Negamax::ncnt)) * 10000) / 100.0f << "%)";

    logger << "popcnt cnt: " << Chess::popcnt_callcnt;

    return 0;
}