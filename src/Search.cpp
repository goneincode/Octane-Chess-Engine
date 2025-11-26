#include "Search.h"
#include <iostream>
#include <limits>
#include <algorithm>

Search::Search() : nodesVisited(0) {}

Search::~Search() {}

Move Search::findBestMove(Board& board, int depth) {
    nodesVisited = 0;
    std::vector<Move> moves = board.generateLegalMoves();
    
    if (moves.empty()) {
        return Move(); // No moves available (Checkmate or Stalemate)
    }

    Move bestMove;
    int bestValue = std::numeric_limits<int>::min();
    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    // We are always the "maximizing" player at the root from the engine's perspective
    // But wait, evaluate() returns positive for White advantage.
    // If it's Black's turn, Black wants to minimize the score (make it negative).
    // Standard Minimax usually assumes "Max" is the current player.
    // Let's adjust:
    // If we use NegaMax, it's simpler. But let's stick to standard Minimax for clarity first.
    // Actually, let's use NegaMax framework where evaluate() returns score relative to side to move.
    // But our evaluate() returns absolute score.
    
    // Let's stick to: White maximizes, Black minimizes.
    bool maximizingPlayer = (board.getTurn() == WHITE);
    
    if (!maximizingPlayer) {
        bestValue = std::numeric_limits<int>::max();
    }

    std::cout << "Thinking..." << std::endl;

    for (const auto& move : moves) {
        board.makeMove(move);
        
        int value = alphaBeta(board, depth - 1, alpha, beta, !maximizingPlayer);
        
        board.unmakeMove(move);
        
        if (maximizingPlayer) {
            if (value > bestValue) {
                bestValue = value;
                bestMove = move;
            }
            alpha = std::max(alpha, bestValue);
        } else {
            if (value < bestValue) {
                bestValue = value;
                bestMove = move;
            }
            beta = std::min(beta, bestValue);
        }
    }
    
    std::cout << "Best move found: " << (bestValue / 100.0) << " (Nodes: " << nodesVisited << ")" << std::endl;
    return bestMove;
}

int Search::alphaBeta(Board& board, int depth, int alpha, int beta, bool maximizingPlayer) {
    nodesVisited++;
    
    if (depth == 0) {
        return board.evaluate();
    }

    std::vector<Move> moves = board.generateLegalMoves();
    
    if (moves.empty()) {
        // Checkmate or Stalemate
        if (board.isInCheck(board.getTurn())) {
            // Checkmate: Return a very large/small value preferring faster mates
            return maximizingPlayer ? -100000 - depth : 100000 + depth;
        }
        return 0; // Stalemate
    }

    if (maximizingPlayer) {
        int value = std::numeric_limits<int>::min();
        for (const auto& move : moves) {
            board.makeMove(move);
            value = std::max(value, alphaBeta(board, depth - 1, alpha, beta, false));
            board.unmakeMove(move);
            
            alpha = std::max(alpha, value);
            if (alpha >= beta) break; // Beta cutoff
        }
        return value;
    } else {
        int value = std::numeric_limits<int>::max();
        for (const auto& move : moves) {
            board.makeMove(move);
            value = std::min(value, alphaBeta(board, depth - 1, alpha, beta, true));
            board.unmakeMove(move);
            
            beta = std::min(beta, value);
            if (beta <= alpha) break; // Alpha cutoff
        }
        return value;
    }
}
