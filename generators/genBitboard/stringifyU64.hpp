#pragma once

#include <iomanip>

#include <lib/chess/util.hpp>

using namespace Chess;

inline std::string indent(int level) {
    return std::string(level * 4, ' ');
}


// 1D array
template <size_t N>
std::string stringifyIntArray(const int (&arr)[N], int indent_level = 0) {
    std::ostringstream out;

    if (N <= 8) {
        // Small: single line
        out << "{";
        for (size_t i = 0; i < N; ++i) {
            out << arr[i];
            if (i + 1 < N) out << ", ";
        }
        out << "}";
    } else {
        // Large: multiline with 8-per-line wrap
        out << "{\n";
        for (size_t i = 0; i < N; ++i) {
            if (i % 8 == 0) out << indent(indent_level + 1);
            out << arr[i];
            if (i + 1 < N) out << ", ";
            if (i % 8 == 7 || i + 1 == N) out << "\n";
        }
        out << indent(indent_level) << "}";
    }

    return out.str();
}



// 1D array
template <size_t N>
std::string stringifyU64Array(const U64 (&arr)[N], int indent_level = 0) {
    std::ostringstream out;

    if (N <= 8) {
        // Small: single line
        out << "{";
        for (size_t i = 0; i < N; ++i) {
            out << "0x" << std::hex << std::setw(16) << std::setfill('0') << arr[i];
            if (i + 1 < N) out << ", ";
        }
        out << "}";
    } else {
        // Large: multiline with 8-per-line wrap
        out << "{\n";
        for (size_t i = 0; i < N; ++i) {
            if (i % 8 == 0) out << indent(indent_level + 1);
            out << "0x" << std::hex << std::setw(16) << std::setfill('0') << arr[i];
            if (i + 1 < N) out << ", ";
            if (i % 8 == 7 || i + 1 == N) out << "\n";
        }
        out << indent(indent_level) << "}";
    }

    return out.str();
}

// 2D array
template <size_t N, size_t M>
std::string stringifyU64Array(const U64 (&arr)[N][M], int indent_level = 0) {
    std::ostringstream out;
    out << "{\n";
    for (size_t i = 0; i < N; ++i) {
        out << indent(indent_level + 1)
            << stringifyU64Array(arr[i], indent_level + 1);
        if (i + 1 < N) out << ",";
        out << "\n";
    }
    out << indent(indent_level) << "}";
    return out.str();
}