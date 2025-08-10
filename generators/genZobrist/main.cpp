#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>

#include <logger/logger.hpp>

int main() {
    static constexpr Logger logger = Logger("Zobrist Gen");

    std::mt19937_64 gen(8523958092582940);
    std::uniform_int_distribution<uint64_t> dis;

    std::ofstream out("zobristhashes.hpp");
    out << "#pragma once\n"
        << "#include <cstdint>\n\n"
        << "namespace Chess::ZobristHashes {\n\n"
        << "    static uint_fast64_t piece_codes[64][6][2] = {\n";

    for (int sq = 0; sq < 64; ++sq) {
        out << "        {\n";
        for (int p = 0; p < 6; ++p) {
            out << "            { ";
            for (int c = 0; c < 2; ++c) {
                out << "0x" << std::hex << dis(gen);
                if (c < 1) out << ", ";
            }
            out << " }";
            if (p < 5) out << ",";
            out << "\n";
        }
        out << "        }";
        if (sq < 63) out << ",";
        out << "\n";
    }

    out << "    };\n\n";

    // Castle codes
    out << "    static uint_fast64_t castle_codes[2][2][2][2] = {\n";
    for (int wk = 0; wk < 2; ++wk) {
        out << "    {\n";
        for (int wq = 0; wq < 2; ++wq) {
            out << "        {\n";
            for (int bk = 0; bk < 2; ++bk) {
                out << "            { ";
                for (int bq = 0; bq < 2; ++bq) {
                    out << "0x" << std::hex << dis(gen);
                    if (bq < 1) out << ", ";
                }
                out << " }";
                if (bk < 1) out << ",";
                out << "\n";
            }
            out << "        }";
            if (wq < 1) out << ",";
            out << "\n";
        }
        out << "    }";
        if (wk < 1) out << ",";
        out << "\n";
    }
    out << "    };\n\n";

    // En passant
    out << "    static uint_fast64_t en_passant_codes[8] = {\n        ";
    for (int i = 0; i < 8; ++i) {
        out << "0x" << std::hex << dis(gen);
        if (i < 7) out << ", ";
    }
    out << "\n    };\n\n";

    // Move code
    out << "    static uint_fast64_t black_move_code = 0x" << std::hex << dis(gen) << ";\n";

    out << "\n} // namespace Chess::Hashes\n";

    out.close();
    logger << "Generated zobristHashes.hpp";
    logger << "MOVE OUTPUT FILE FROM builddir to lib/chess";

    return 0;
}
