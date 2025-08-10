# It's another chess engine
written in c++

...

crazy i know

## Engine
This engine is fully bitboard-based 

currently capable of ~200M nodes/second raw perft search (depth 6 from starting position) - running on one core of an i71400k

# Projects
## Perft
contains a perft method with reasonable generation speeds, tested against all examples in [Chessprogramming](https://www.chessprogramming.org/Perft_Results) for accuracy
## Game
play the game against yourself, the engine accepts algebreic notation

## Generation
Includes subprojects in the generators for building the lookup tables for zorbist hashing and bitboards

# Roadmap
- Alpha-Beta pruning, looking to improve perft using zorbist hashes
- Basic search tree with negmax (engine!)
- Magic bitboards for bishop and rook slider moves
