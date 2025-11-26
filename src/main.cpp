#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "Board.h"
#include "Search.h"

int parseSquare(std::string s) {
    if (s.length() != 2) return -1;
    int file = s[0] - 'a';
    int rank = s[1] - '1';
    if (file < 0 || file > 7 || rank < 0 || rank > 7) return -1;
    return rank * 8 + file;
}

std::string squareToString(int square) {
    int rank = square / 8;
    int file = square % 8;
    return std::string(1, 'a' + file) + std::to_string(rank + 1);
}

void drawEvalBar(int score) {
    const int width = 40; // Total width of the bar
    const int maxScore = 1000; // Score at which bar is full (10 pawns)
    
    // Clamp score
    int clampedScore = score;
    if (clampedScore > maxScore) clampedScore = maxScore;
    if (clampedScore < -maxScore) clampedScore = -maxScore;
    
    float ratio = (float)clampedScore / maxScore;
    int chars = (int)(ratio * (width / 2));
    
    std::cout << "\nEvaluation: " << (score / 100.0) << " ";
    std::cout << "[";
    
    for (int i = 0; i < width; ++i) {
        if (i == width / 2) {
            std::cout << "|";
        } else if (i < width / 2) {
            if (chars < 0 && i >= (width / 2 + chars)) std::cout << "=";
            else std::cout << " ";
        } else {
            if (chars > 0 && i < (width / 2 + chars)) std::cout << "=";
            else std::cout << " ";
        }
    }
    std::cout << "]" << std::endl;
}

int main(int argc, char* argv[]) {
    bool verbose = false;
    bool playComputer = false;
    int computerColor = BLACK; // Default computer plays Black
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "-vv") {
            verbose = true;
        }
        if (arg == "--play") {
            playComputer = true;
        }
    }

    std::cout << "Octant Engine Started" << std::endl;
    if (verbose) std::cout << "Verbose mode enabled." << std::endl;
    if (playComputer) std::cout << "Playing against Computer (Black)." << std::endl;
    
    Board board;
    board.setVerbose(verbose);
    Search search;
    
    std::string input;
    while (true) {
        board.printBoard();
        drawEvalBar(board.evaluate());
        
        // Check game over
        if (board.generateLegalMoves().empty()) {
            if (board.isInCheck(board.getTurn())) {
                std::cout << "Checkmate! " << (board.getTurn() == WHITE ? "Black" : "White") << " wins!" << std::endl;
            } else {
                std::cout << "Stalemate!" << std::endl;
            }
            break;
        }

        if (playComputer && board.getTurn() == computerColor) {
            // Computer's turn
            Move bestMove = search.findBestMove(board, 4); // Depth 4
            board.makeMove(bestMove);
            std::cout << "Computer played: " << squareToString(bestMove.from) << squareToString(bestMove.to) << std::endl;
            continue;
        }

        std::cout << "Enter move (e.g., e2e4) or 'q' to quit: ";
        if (!(std::cin >> input)) break;
        
        if (input == "q" || input == "quit") break;
        
        if (input.length() == 4) {
            int from = parseSquare(input.substr(0, 2));
            int to = parseSquare(input.substr(2, 2));
            
            if (from != -1 && to != -1) {
                if (!board.makeMove(from, to)) {
                    std::cout << "Illegal move!" << std::endl;
                }
            } else {
                std::cout << "Invalid coordinate format." << std::endl;
            }
        } else {
            std::cout << "Invalid input format. Use 4 characters like 'e2e4'." << std::endl;
        }
    }

    return 0;
}
