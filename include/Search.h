#ifndef SEARCH_H
#define SEARCH_H

#include "Board.h"

class Search {
public:
    Search();
    ~Search();

    // Find the best move for the current side to move
    Move findBestMove(Board& board, int depth);

private:
    // Alpha-Beta Pruning recursive search
    int alphaBeta(Board& board, int depth, int alpha, int beta, bool maximizingPlayer);
    
    // Stats
    int nodesVisited;
};

#endif // SEARCH_H
