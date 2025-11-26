# Octant Chess Engine

Octant is a C++ chess engine featuring:
- Bitboard-ready structure (currently array-based for simplicity)
- Minimax Search with Alpha-Beta Pruning
- Piece-Square Tables for positional evaluation
- Full move validation (Castling, En Passant, Promotion)
- Interactive CLI with an evaluation bar

## Building

Requirements:
- CMake 3.10+
- C++20 compatible compiler

```bash
mkdir build
cd build
cmake ..
make
```

## Running

To play against the engine (you play White, Engine plays Black):
```bash
./Octant --play
```

To run in analysis mode (two-player / sandbox):
```bash
./Octant
```

## Features
- **Search**: Depth-limited Alpha-Beta search.
- **Evaluation**: Material balance + Positional factors (PSTs).
- **UI**: ASCII board representation with a dynamic evaluation bar.
