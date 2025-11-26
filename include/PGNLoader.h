#ifndef PGNLOADER_H
#define PGNLOADER_H

#include "Board.h"
#include <string>
#include <vector>

class PGNLoader {
public:
    // Loads the first game from a PGN file and plays it on the board.
    // Returns true if successful.
    static bool loadPGN(const std::string& filename, Board& board);

    // Parses a single SAN move string (e.g., "Nf3") and returns the corresponding Move object.
    // Returns an empty/invalid Move if parsing fails.
    static Move parseSAN(const std::string& san, Board& board);
};

#endif // PGNLOADER_H
