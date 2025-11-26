#include "UCI.h"
#include "Utils.h"
#include "PGNLoader.h"
#include <iostream>
#include <sstream>
#include <vector>

void UCI::loop() {
    std::string line;
    std::string token;

    // Disable board verbose logging for UCI
    board.setVerbose(false);

    while (std::getline(std::cin, line)) {
        line = trim(line);
        if (line.empty()) continue;

        std::istringstream ss(line);
        ss >> token;

        if (token == "uci") {
            std::cout << "id name Octant 0.1" << std::endl;
            std::cout << "id author Stumbles" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (token == "isready") {
            std::cout << "readyok" << std::endl;
        }
        else if (token == "ucinewgame") {
            board.setupStandardPosition();
        }
        else if (token == "position") {
            handlePosition(line);
        }
        else if (token == "go") {
            handleGo(line);
        }
        else if (token == "quit") {
            break;
        }
        else if (token == "print") {
            board.printBoard();
        }
    }
}

void UCI::handlePosition(const std::string& line) {
    // Format: position startpos [moves e2e4 d7d5 ...]
    // Format: position fen <fen_string> [moves ...] (FEN not implemented yet)
    
    std::vector<std::string> tokens = split(line, ' ');
    size_t movesIndex = 0;

    // Reset board first
    if (tokens.size() > 1 && tokens[1] == "startpos") {
        board.setupStandardPosition();
        movesIndex = 2;
    } 
    // TODO: Handle 'fen'

    // Find "moves" keyword
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "moves") {
            movesIndex = i + 1;
            break;
        }
    }

    // Apply moves
    if (movesIndex > 0 && movesIndex < tokens.size()) {
        for (size_t i = movesIndex; i < tokens.size(); ++i) {
            std::string moveStr = tokens[i];
            // UCI uses long algebraic (e2e4, e7e8q)
            // Our parseSquare handles e2e4.
            // We need to handle promotion suffix.
            
            if (moveStr.length() < 4) continue;
            
            int from = parseSquare(moveStr.substr(0, 2));
            int to = parseSquare(moveStr.substr(2, 2));
            
            if (from == -1 || to == -1) continue;

            // Find the legal move that matches
            std::vector<Move> legalMoves = board.generateLegalMoves();
            bool found = false;
            for (const auto& m : legalMoves) {
                if (m.from == from && m.to == to) {
                    // Check promotion
                    if (m.isPromotion) {
                        // UCI promotion is 5th char: e7e8q
                        char promoChar = 'q'; // default
                        if (moveStr.length() > 4) promoChar = moveStr[4];
                        
                        int promoType = QUEEN;
                        if (promoChar == 'r') promoType = ROOK;
                        else if (promoChar == 'b') promoType = BISHOP;
                        else if (promoChar == 'n') promoType = KNIGHT;
                        
                        if (m.promotionPiece == promoType) {
                            board.makeMove(m);
                            found = true;
                            break;
                        }
                    } else {
                        board.makeMove(m);
                        found = true;
                        break;
                    }
                }
            }
        }
    }
}

void UCI::handleGo(const std::string& line) {
    // Format: go wtime 300000 btime 300000 ...
    // For now, just fixed depth
    
    Move bestMove = search.findBestMove(board, 4);
    
    std::string moveStr = squareToString(bestMove.from) + squareToString(bestMove.to);
    if (bestMove.isPromotion) {
        switch (bestMove.promotionPiece) {
            case QUEEN: moveStr += "q"; break;
            case ROOK: moveStr += "r"; break;
            case BISHOP: moveStr += "b"; break;
            case KNIGHT: moveStr += "n"; break;
        }
    }
    
    std::cout << "bestmove " << moveStr << std::endl;
}
