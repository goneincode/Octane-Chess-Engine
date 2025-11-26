# Project TODOs

## Core Logic
- [x] **PGN Reader**: Implement a parser to read Portable Game Notation files and play through games.
    - [x] SAN (Standard Algebraic Notation) parsing (e.g., "Nf3", "O-O").
    - [ ] Handling of game metadata (White, Black, Date, etc.).
- [ ] **Draw Rules**:
    - [ ] Threefold Repetition detection (requires Zobrist Hashing or similar history tracking).
    - [ ] 50-Move Rule implementation.
    - [ ] Insufficient Material check.
- [x] **UCI Protocol**: Implement Universal Chess Interface to allow the engine to work with standard GUIs (Arena, Lichess, etc.).
    - [x] `uci`, `isready`, `position`, `go` commands.
- [ ] **Pawn Promotion**:
    - [ ] Allow user to choose promotion piece in CLI (currently defaults to Queen).
    - [ ] Handle under-promotions in search/PGN.

## Search & Evaluation Improvements
- [ ] **Quiescence Search**: Extend search at leaf nodes for captures to avoid "horizon effect".
- [ ] **Move Ordering**: Sort moves (captures first, killers, history) to improve Alpha-Beta pruning efficiency.
- [ ] **Transposition Table**: Cache search results to avoid re-calculating the same positions.
- [ ] **Iterative Deepening**: Search depth 1, then 2, then 3... allows for better time management.

## GUI & Usability
- [x] **Graphical User Interface (GUI)**:
    - [x] Option B: Web-based frontend (Flask + Chessboard.js).
- [ ] **CLI Improvements**:
    - [ ] Better board visualization (Unicode pieces?).
    - [ ] Command history/editing.

## Code Quality
- [ ] **Refactoring**: Move utility functions (square parsing) to a shared `Utils` header.
- [ ] **Testing**: Add unit tests for move generation and perft (performance test) debugging.
