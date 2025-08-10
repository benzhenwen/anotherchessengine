#include <logger/logger.hpp>

#include <chrono>

#include <lib/chess/gamestate.hpp>
#include <lib/chess/fen.hpp>

#include <lib/chess/engine/perft.hpp>

int main(int argc, char* argv[]) {
    static constexpr Logger logger = Logger("MAIN");

    if(argc != 2) {
        logger.log(Logger::WARNING) << "Please provide an int argument for depth";
        return 0;
    }
    char *p;
    errno = 0;
    long conv = strtol(argv[1], &p, 10);

    if (errno != 0 || *p != '\0' || conv > 100 || conv < 0) {
        logger.log(Logger::WARNING) << "Please provide an int argument for depth (0 - 100)";
        return 0;
    } 

    Chess::GameState gs = Chess::FEN::FENToGameState("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // Chess::GameState gs = Chess::FEN::FENToGameState("rnbqkbnr/pppppppp/8/8/8/3P4/PPP1PPPP/RNBQKBNR b KQkq - 0 1"); // after d2 to d3
    // Chess::GameState gs = Chess::FEN::FENToGameState("rnbqkbnr/pppp1ppp/8/4p3/8/3P4/PPP1PPPP/RNBQKBNR w KQkq e6 0 2"); // after d2 to d3, then e7 to e5

    // Chess::GameState gs = Chess::FEN::FENToGameState("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    auto start = std::chrono::high_resolution_clock::now();

    auto output = Chess::Engine::Perft::perft(gs, conv);

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = finish - start;

    logger << output;
    logger << "in: " << elapsed.count() << "ms";

    return 0;
}