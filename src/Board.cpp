#include "Board.h"
#include <iostream>
#include <iomanip>
#include <cmath>

Board::Board() : turn(WHITE), verbose(false), enPassantSquare(-1), castlingRights(0) {
    for (int i = 0; i < 64; ++i) {
        squares[i] = EMPTY;
    }
    setupStandardPosition();
}

Board::~Board() {
}

void Board::setVerbose(bool v) {
    verbose = v;
}

void Board::log(const std::string& message) const {
    if (verbose) {
        std::cout << "[LOG] " << message << std::endl;
    }
}

void Board::setupStandardPosition() {
    // White pieces
    squares[0] = makePiece(WHITE, ROOK);
    squares[1] = makePiece(WHITE, KNIGHT);
    squares[2] = makePiece(WHITE, BISHOP);
    squares[3] = makePiece(WHITE, QUEEN);
    squares[4] = makePiece(WHITE, KING);
    squares[5] = makePiece(WHITE, BISHOP);
    squares[6] = makePiece(WHITE, KNIGHT);
    squares[7] = makePiece(WHITE, ROOK);
    for (int i = 8; i < 16; ++i) squares[i] = makePiece(WHITE, PAWN);

    // Black pieces
    squares[56] = makePiece(BLACK, ROOK);
    squares[57] = makePiece(BLACK, KNIGHT);
    squares[58] = makePiece(BLACK, BISHOP);
    squares[59] = makePiece(BLACK, QUEEN);
    squares[60] = makePiece(BLACK, KING);
    squares[61] = makePiece(BLACK, BISHOP);
    squares[62] = makePiece(BLACK, KNIGHT);
    squares[63] = makePiece(BLACK, ROOK);
    for (int i = 48; i < 56; ++i) squares[i] = makePiece(BLACK, PAWN);

    turn = WHITE;
    castlingRights = 0xF; // All castling allowed
    enPassantSquare = -1;
    log("Standard position setup complete.");
}

char getPieceChar(int piece) {
    int type = piece & 0x7;
    int color = piece & 0x18;
    
    if (type == EMPTY) return '.';
    
    char c = '?';
    switch (type) {
        case PAWN: c = 'P'; break;
        case KNIGHT: c = 'N'; break;
        case BISHOP: c = 'B'; break;
        case ROOK: c = 'R'; break;
        case QUEEN: c = 'Q'; break;
        case KING: c = 'K'; break;
    }
    
    if (color == BLACK) c = tolower(c);
    return c;
}

void Board::printBoard() const {
    std::cout << "  +-----------------+" << std::endl;
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << rank + 1 << " | ";
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            std::cout << getPieceChar(squares[square]) << " ";
        }
        std::cout << "|" << std::endl;
    }
    std::cout << "  +-----------------+" << std::endl;
    std::cout << "    a b c d e f g h" << std::endl;
    std::cout << "Turn: " << (turn == WHITE ? "White" : "Black") << std::endl;
    std::cout << "Castling: " << ((castlingRights & 1) ? "K" : "") << ((castlingRights & 2) ? "Q" : "") 
              << ((castlingRights & 4) ? "k" : "") << ((castlingRights & 8) ? "q" : "") << std::endl;
    if (enPassantSquare != -1) {
        int f = enPassantSquare % 8;
        int r = enPassantSquare / 8;
        std::cout << "EP Target: " << (char)('a' + f) << (r + 1) << std::endl;
    }
}

int Board::getPieceAt(int square) const {
    if (square < 0 || square >= 64) return EMPTY;
    return squares[square];
}

int Board::getTurn() const {
    return turn;
}

bool Board::isPathClear(int from, int to) const {
    int fromRank = from / 8;
    int fromFile = from % 8;
    int toRank = to / 8;
    int toFile = to % 8;

    int dRank = toRank - fromRank;
    int dFile = toFile - fromFile;

    int stepRank = (dRank == 0) ? 0 : (dRank > 0 ? 1 : -1);
    int stepFile = (dFile == 0) ? 0 : (dFile > 0 ? 1 : -1);

    int current = from + stepRank * 8 + stepFile;
    while (current != to) {
        if (squares[current] != EMPTY) {
            return false;
        }
        current += stepRank * 8 + stepFile;
    }
    return true;
}

bool Board::isSquareAttacked(int square, int attackerColor) const {
    int rank = square / 8;
    int file = square % 8;

    // Check Pawn attacks
    int pawnDir = (attackerColor == WHITE) ? -1 : 1; // Pawns attack "backwards" from target perspective
    // If we are checking if White attacks 'square', we look for White pawns at square - 1 rank
    // Wait, simpler: Iterate all squares? No, too slow.
    // Check possible source squares for the attacker.
    
    // Check for Pawns
    int pawnRank = rank - ((attackerColor == WHITE) ? 1 : -1);
    if (pawnRank >= 0 && pawnRank < 8) {
        if (file - 1 >= 0) {
            int p = squares[pawnRank * 8 + (file - 1)];
            if ((p & 0x18) == attackerColor && (p & 0x7) == PAWN) return true;
        }
        if (file + 1 < 8) {
            int p = squares[pawnRank * 8 + (file + 1)];
            if ((p & 0x18) == attackerColor && (p & 0x7) == PAWN) return true;
        }
    }

    // Check Knights
    int kMoves[8][2] = {{-2,-1}, {-2,1}, {-1,-2}, {-1,2}, {1,-2}, {1,2}, {2,-1}, {2,1}};
    for (auto& m : kMoves) {
        int r = rank + m[0];
        int f = file + m[1];
        if (r >= 0 && r < 8 && f >= 0 && f < 8) {
            int p = squares[r * 8 + f];
            if ((p & 0x18) == attackerColor && (p & 0x7) == KNIGHT) return true;
        }
    }

    // Check Kings
    int kingMoves[8][2] = {{-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,-1}, {1,0}, {1,1}};
    for (auto& m : kingMoves) {
        int r = rank + m[0];
        int f = file + m[1];
        if (r >= 0 && r < 8 && f >= 0 && f < 8) {
            int p = squares[r * 8 + f];
            if ((p & 0x18) == attackerColor && (p & 0x7) == KING) return true;
        }
    }

    // Check Sliding Pieces (Rook/Queen)
    int rookDirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
    for (auto& d : rookDirs) {
        int r = rank + d[0];
        int f = file + d[1];
        while (r >= 0 && r < 8 && f >= 0 && f < 8) {
            int p = squares[r * 8 + f];
            if (p != EMPTY) {
                if ((p & 0x18) == attackerColor && ((p & 0x7) == ROOK || (p & 0x7) == QUEEN)) return true;
                break; // Blocked
            }
            r += d[0];
            f += d[1];
        }
    }

    // Check Sliding Pieces (Bishop/Queen)
    int bishopDirs[4][2] = {{-1,-1}, {-1,1}, {1,-1}, {1,1}};
    for (auto& d : bishopDirs) {
        int r = rank + d[0];
        int f = file + d[1];
        while (r >= 0 && r < 8 && f >= 0 && f < 8) {
            int p = squares[r * 8 + f];
            if (p != EMPTY) {
                if ((p & 0x18) == attackerColor && ((p & 0x7) == BISHOP || (p & 0x7) == QUEEN)) return true;
                break; // Blocked
            }
            r += d[0];
            f += d[1];
        }
    }

    return false;
}

bool Board::isInCheck(int color) const {
    // Find King
    int kingSquare = -1;
    for (int i = 0; i < 64; ++i) {
        if ((squares[i] & 0x18) == color && (squares[i] & 0x7) == KING) {
            kingSquare = i;
            break;
        }
    }
    
    if (kingSquare == -1) return false; // Should not happen
    
    int enemyColor = (color == WHITE) ? BLACK : WHITE;
    return isSquareAttacked(kingSquare, enemyColor);
}

bool Board::validatePawnMove(int from, int to, int piece, int target) const {
    int color = piece & 0x18;
    int direction = (color == WHITE) ? 1 : -1;
    int startRank = (color == WHITE) ? 1 : 6;
    
    int fromRank = from / 8;
    int fromFile = from % 8;
    int toRank = to / 8;
    int toFile = to % 8;

    int diffRank = toRank - fromRank;
    int diffFile = std::abs(toFile - fromFile);

    // Forward move
    if (diffFile == 0) {
        if (target != EMPTY) {
            log("Pawn cannot capture forward.");
            return false;
        }
        // Single step
        if (diffRank == direction) return true;
        // Double step
        if (fromRank == startRank && diffRank == 2 * direction) {
            int midSquare = from + 8 * direction;
            if (squares[midSquare] != EMPTY) {
                log("Pawn double move blocked.");
                return false;
            }
            return true;
        }
    }
    // Capture
    else if (diffFile == 1 && diffRank == direction) {
        if (target != EMPTY && (target & 0x18) != color) return true;
        
        // En Passant
        if (to == enPassantSquare) {
            log("En Passant capture!");
            return true;
        }
        
        log("Pawn capture requires enemy piece.");
        return false;
    }

    return false;
}

bool Board::validateKnightMove(int from, int to) const {
    int fromRank = from / 8;
    int fromFile = from % 8;
    int toRank = to / 8;
    int toFile = to % 8;
    
    int dRank = std::abs(toRank - fromRank);
    int dFile = std::abs(toFile - fromFile);
    
    return (dRank == 2 && dFile == 1) || (dRank == 1 && dFile == 2);
}

bool Board::validateBishopMove(int from, int to) const {
    int fromRank = from / 8;
    int fromFile = from % 8;
    int toRank = to / 8;
    int toFile = to % 8;
    
    if (std::abs(toRank - fromRank) != std::abs(toFile - fromFile)) return false;
    return isPathClear(from, to);
}

bool Board::validateRookMove(int from, int to) const {
    int fromRank = from / 8;
    int fromFile = from % 8;
    int toRank = to / 8;
    int toFile = to % 8;
    
    if (fromRank != toRank && fromFile != toFile) return false;
    return isPathClear(from, to);
}

bool Board::validateQueenMove(int from, int to) const {
    return validateBishopMove(from, to) || validateRookMove(from, to);
}

bool Board::validateKingMove(int from, int to) const {
    int fromRank = from / 8;
    int fromFile = from % 8;
    int toRank = to / 8;
    int toFile = to % 8;
    
    int dRank = std::abs(toRank - fromRank);
    int dFile = std::abs(toFile - fromFile);
    
    if (dRank <= 1 && dFile <= 1) return true;
    
    // Castling
    // White King e1(4) -> g1(6) (Kingside) or c1(2) (Queenside)
    // Black King e8(60) -> g8(62) (Kingside) or c8(58) (Queenside)
    
    int color = squares[from] & 0x18;
    
    if (dRank == 0 && dFile == 2) {
        // Kingside
        if (toFile == 6) {
            if (color == WHITE) {
                if (!(castlingRights & 1)) { log("White Kingside castling rights lost."); return false; }
                if (squares[5] != EMPTY || squares[6] != EMPTY) { log("Path blocked for castling."); return false; }
                if (isSquareAttacked(4, BLACK) || isSquareAttacked(5, BLACK) || isSquareAttacked(6, BLACK)) {
                    log("Cannot castle through or into check.");
                    return false;
                }
                return true;
            } else {
                if (!(castlingRights & 4)) { log("Black Kingside castling rights lost."); return false; }
                if (squares[61] != EMPTY || squares[62] != EMPTY) { log("Path blocked for castling."); return false; }
                if (isSquareAttacked(60, WHITE) || isSquareAttacked(61, WHITE) || isSquareAttacked(62, WHITE)) {
                    log("Cannot castle through or into check.");
                    return false;
                }
                return true;
            }
        }
        // Queenside
        else if (toFile == 2) {
            if (color == WHITE) {
                if (!(castlingRights & 2)) { log("White Queenside castling rights lost."); return false; }
                if (squares[1] != EMPTY || squares[2] != EMPTY || squares[3] != EMPTY) { log("Path blocked for castling."); return false; }
                if (isSquareAttacked(4, BLACK) || isSquareAttacked(3, BLACK) || isSquareAttacked(2, BLACK)) {
                    log("Cannot castle through or into check.");
                    return false;
                }
                return true;
            } else {
                if (!(castlingRights & 8)) { log("Black Queenside castling rights lost."); return false; }
                if (squares[57] != EMPTY || squares[58] != EMPTY || squares[59] != EMPTY) { log("Path blocked for castling."); return false; }
                if (isSquareAttacked(60, WHITE) || isSquareAttacked(59, WHITE) || isSquareAttacked(58, WHITE)) {
                    log("Cannot castle through or into check.");
                    return false;
                }
                return true;
            }
        }
    }
    
    return false;
}

bool Board::isValidMove(int from, int to) const {
    if (from < 0 || from >= 64 || to < 0 || to >= 64) {
        log("Move out of bounds.");
        return false;
    }
    
    if (from == to) {
        log("Source and destination are the same.");
        return false;
    }

    int piece = squares[from];
    if (piece == EMPTY) {
        log("No piece at source square.");
        return false;
    }

    int pieceColor = piece & 0x18;
    if (pieceColor != turn) {
        log("Not your turn!");
        return false;
    }

    int target = squares[to];
    if (target != EMPTY) {
        int targetColor = target & 0x18;
        if (targetColor == pieceColor) {
            log("Cannot capture own piece.");
            return false;
        }
    }

    int type = piece & 0x7;
    bool valid = false;
    switch (type) {
        case PAWN: valid = validatePawnMove(from, to, piece, target); break;
        case KNIGHT: valid = validateKnightMove(from, to); break;
        case BISHOP: valid = validateBishopMove(from, to); break;
        case ROOK: valid = validateRookMove(from, to); break;
        case QUEEN: valid = validateQueenMove(from, to); break;
        case KING: valid = validateKingMove(from, to); break;
        default: valid = false; break;
    }

    if (!valid) {
        log("Piece-specific move validation failed.");
        return false;
    }
    
    return true;
}

// ... (Previous code)

// --- Move Generation ---

void Board::generatePawnMoves(int square, std::vector<Move>& moves) const {
    int piece = squares[square];
    int color = piece & 0x18;
    int direction = (color == WHITE) ? 1 : -1;
    int startRank = (color == WHITE) ? 1 : 6;
    int promotionRank = (color == WHITE) ? 7 : 0;
    
    int rank = square / 8;
    int file = square % 8;
    
    // Forward moves
    int oneStep = square + 8 * direction;
    if (oneStep >= 0 && oneStep < 64 && squares[oneStep] == EMPTY) {
        // Promotion?
        if ((oneStep / 8) == promotionRank) {
            moves.emplace_back(square, oneStep, piece, 0, false, true, QUEEN);
            moves.emplace_back(square, oneStep, piece, 0, false, true, ROOK);
            moves.emplace_back(square, oneStep, piece, 0, false, true, BISHOP);
            moves.emplace_back(square, oneStep, piece, 0, false, true, KNIGHT);
        } else {
            moves.emplace_back(square, oneStep, piece);
            // Double step
            if (rank == startRank) {
                int twoStep = square + 16 * direction;
                if (squares[twoStep] == EMPTY) {
                    moves.emplace_back(square, twoStep, piece);
                }
            }
        }
    }
    
    // Captures
    int captureOffsets[] = {7, 9}; // Relative to direction * 8. Wait, simpler: file +/- 1
    for (int dFile : {-1, 1}) {
        int targetFile = file + dFile;
        if (targetFile >= 0 && targetFile < 8) {
            int targetSquare = square + 8 * direction + dFile;
            if (targetSquare >= 0 && targetSquare < 64) {
                int targetPiece = squares[targetSquare];
                // Normal capture
                if (targetPiece != EMPTY && (targetPiece & 0x18) != color) {
                    if ((targetSquare / 8) == promotionRank) {
                        moves.emplace_back(square, targetSquare, piece, targetPiece, false, true, QUEEN);
                        moves.emplace_back(square, targetSquare, piece, targetPiece, false, true, ROOK);
                        moves.emplace_back(square, targetSquare, piece, targetPiece, false, true, BISHOP);
                        moves.emplace_back(square, targetSquare, piece, targetPiece, false, true, KNIGHT);
                    } else {
                        moves.emplace_back(square, targetSquare, piece, targetPiece);
                    }
                }
                // En Passant
                else if (targetSquare == enPassantSquare) {
                    // The captured pawn is at square + dFile (same rank)
                    int capturedPawnSq = square + dFile;
                    int capturedPawn = squares[capturedPawnSq];
                    moves.emplace_back(square, targetSquare, piece, capturedPawn, false, false, 0); 
                    // Note: capturedPiece in Move struct usually refers to piece at 'to', 
                    // but for EP it's special. I'll handle it in makeMove.
                }
            }
        }
    }
}

void Board::generateKnightMoves(int square, std::vector<Move>& moves) const {
    int piece = squares[square];
    int color = piece & 0x18;
    int movesOffsets[8][2] = {{-2,-1}, {-2,1}, {-1,-2}, {-1,2}, {1,-2}, {1,2}, {2,-1}, {2,1}};
    
    int rank = square / 8;
    int file = square % 8;
    
    for (auto& m : movesOffsets) {
        int r = rank + m[0];
        int f = file + m[1];
        if (r >= 0 && r < 8 && f >= 0 && f < 8) {
            int targetSquare = r * 8 + f;
            int targetPiece = squares[targetSquare];
            if (targetPiece == EMPTY || (targetPiece & 0x18) != color) {
                moves.emplace_back(square, targetSquare, piece, targetPiece);
            }
        }
    }
}

void Board::generateSlidingMoves(int square, std::vector<Move>& moves, const int directions[][2], int numDirs) const {
    int piece = squares[square];
    int color = piece & 0x18;
    int rank = square / 8;
    int file = square % 8;
    
    for (int i = 0; i < numDirs; ++i) {
        int r = rank + directions[i][0];
        int f = file + directions[i][1];
        while (r >= 0 && r < 8 && f >= 0 && f < 8) {
            int targetSquare = r * 8 + f;
            int targetPiece = squares[targetSquare];
            
            if (targetPiece == EMPTY) {
                moves.emplace_back(square, targetSquare, piece);
            } else {
                if ((targetPiece & 0x18) != color) {
                    moves.emplace_back(square, targetSquare, piece, targetPiece);
                }
                break; // Blocked
            }
            
            r += directions[i][0];
            f += directions[i][1];
        }
    }
}

void Board::generateKingMoves(int square, std::vector<Move>& moves) const {
    int piece = squares[square];
    int color = piece & 0x18;
    int movesOffsets[8][2] = {{-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,-1}, {1,0}, {1,1}};
    
    int rank = square / 8;
    int file = square % 8;
    
    // Normal moves
    for (auto& m : movesOffsets) {
        int r = rank + m[0];
        int f = file + m[1];
        if (r >= 0 && r < 8 && f >= 0 && f < 8) {
            int targetSquare = r * 8 + f;
            int targetPiece = squares[targetSquare];
            if (targetPiece == EMPTY || (targetPiece & 0x18) != color) {
                moves.emplace_back(square, targetSquare, piece, targetPiece);
            }
        }
    }
    
    // Castling
    // Note: We only check structural legality here (rights + path). 
    // "Through check" is handled by makeMove/isValidMove check.
    if (color == WHITE) {
        if (castlingRights & 1) { // WK
            if (squares[5] == EMPTY && squares[6] == EMPTY) {
                if (!isSquareAttacked(4, BLACK) && !isSquareAttacked(5, BLACK) && !isSquareAttacked(6, BLACK))
                    moves.emplace_back(square, 6, piece, 0, true);
            }
        }
        if (castlingRights & 2) { // WQ
            if (squares[1] == EMPTY && squares[2] == EMPTY && squares[3] == EMPTY) {
                if (!isSquareAttacked(4, BLACK) && !isSquareAttacked(3, BLACK) && !isSquareAttacked(2, BLACK))
                    moves.emplace_back(square, 2, piece, 0, true);
            }
        }
    } else {
        if (castlingRights & 4) { // BK
            if (squares[61] == EMPTY && squares[62] == EMPTY) {
                if (!isSquareAttacked(60, WHITE) && !isSquareAttacked(61, WHITE) && !isSquareAttacked(62, WHITE))
                    moves.emplace_back(square, 62, piece, 0, true);
            }
        }
        if (castlingRights & 8) { // BQ
            if (squares[57] == EMPTY && squares[58] == EMPTY && squares[59] == EMPTY) {
                if (!isSquareAttacked(60, WHITE) && !isSquareAttacked(59, WHITE) && !isSquareAttacked(58, WHITE))
                    moves.emplace_back(square, 58, piece, 0, true);
            }
        }
    }
}

std::vector<Move> Board::generateLegalMoves() {
    std::vector<Move> moves;
    moves.reserve(50); // Pre-allocate some space
    
    for (int i = 0; i < 64; ++i) {
        int piece = squares[i];
        if (piece != EMPTY && (piece & 0x18) == turn) {
            int type = piece & 0x7;
            switch (type) {
                case PAWN: generatePawnMoves(i, moves); break;
                case KNIGHT: generateKnightMoves(i, moves); break;
                case BISHOP: {
                    int dirs[4][2] = {{-1,-1}, {-1,1}, {1,-1}, {1,1}};
                    generateSlidingMoves(i, moves, dirs, 4);
                    break;
                }
                case ROOK: {
                    int dirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
                    generateSlidingMoves(i, moves, dirs, 4);
                    break;
                }
                case QUEEN: {
                    int dirs[8][2] = {{-1,-1}, {-1,1}, {1,-1}, {1,1}, {-1,0}, {1,0}, {0,-1}, {0,1}};
                    generateSlidingMoves(i, moves, dirs, 8);
                    break;
                }
                case KING: generateKingMoves(i, moves); break;
            }
        }
    }
    
    // Filter illegal moves (those that leave King in check)
    std::vector<Move> legalMoves;
    legalMoves.reserve(moves.size());
    
    for (const auto& move : moves) {
        if (makeMove(move)) {
            legalMoves.push_back(move);
            unmakeMove(move);
        }
    }
    
    return legalMoves;
}

// --- Move Execution ---

bool Board::makeMove(int from, int to) {
    // Legacy wrapper for UI
    // We need to construct a proper Move object to use the new system
    // This is inefficient but fine for UI
    std::vector<Move> moves = generateLegalMoves();
    for (const auto& m : moves) {
        if (m.from == from && m.to == to) {
            // Found a matching legal move.
            // If it's a promotion, we default to Queen for this simple UI
            if (m.isPromotion && m.promotionPiece != QUEEN) continue;
            
            makeMove(m);
            return true;
        }
    }
    log("Move not found in legal moves list.");
    return false;
}

bool Board::makeMove(const Move& move) {
    // Save state
    GameState state;
    state.enPassantSquare = enPassantSquare;
    state.castlingRights = castlingRights;
    state.capturedPiece = move.capturedPiece; // Note: For EP, this might need adjustment if we want strict history
    state.halfMoveClock = 0; // TODO
    history.push_back(state);

    // Apply move
    squares[move.to] = squares[move.from];
    squares[move.from] = EMPTY;
    
    // Captures
    if (move.capturedPiece != EMPTY) {
        // Normal capture handled by overwrite above, unless EP
    }
    
    // Special cases
    if (move.isPromotion) {
        squares[move.to] = makePiece((turn == WHITE ? WHITE : BLACK), (PieceType)move.promotionPiece);
    }
    
    if (move.isCastling) {
        // Move Rook
        if (move.to == 6) { squares[5] = squares[7]; squares[7] = EMPTY; } // WK
        else if (move.to == 2) { squares[3] = squares[0]; squares[0] = EMPTY; } // WQ
        else if (move.to == 62) { squares[61] = squares[63]; squares[63] = EMPTY; } // BK
        else if (move.to == 58) { squares[59] = squares[56]; squares[56] = EMPTY; } // BQ
    }
    
    // En Passant Capture
    // If we moved a pawn to the EP square, the captured pawn is "behind" the move.to
    if ((move.piece & 0x7) == PAWN && move.to == enPassantSquare) {
        int captureSquare = (turn == WHITE) ? (move.to - 8) : (move.to + 8);
        squares[captureSquare] = EMPTY;
    }
    
    // Update State
    // En Passant Target
    enPassantSquare = -1;
    if ((move.piece & 0x7) == PAWN && std::abs(move.to - move.from) == 16) {
        enPassantSquare = (move.from + move.to) / 2;
    }
    
    // Castling Rights
    if ((move.piece & 0x7) == KING) {
        if (turn == WHITE) castlingRights &= ~0x3;
        else castlingRights &= ~0xC;
    }
    // Rook moves or captures
    auto clearCorner = [&](int sq) {
        if (sq == 0) castlingRights &= ~2;
        if (sq == 7) castlingRights &= ~1;
        if (sq == 56) castlingRights &= ~8;
        if (sq == 63) castlingRights &= ~4;
    };
    clearCorner(move.from);
    clearCorner(move.to);
    
    // Check legality
    if (isInCheck(turn)) {
        // Illegal move!
        // We must undo manually since we modified state
        // But wait, unmakeMove relies on history.
        // We can just call unmakeMove.
        unmakeMove(move);
        return false;
    }
    
    turn = (turn == WHITE) ? BLACK : WHITE;
    return true;
}

void Board::unmakeMove(const Move& move) {
    if (history.empty()) return;
    
    GameState state = history.back();
    history.pop_back();
    
    turn = (turn == WHITE) ? BLACK : WHITE; // Switch back
    
    enPassantSquare = state.enPassantSquare;
    castlingRights = state.castlingRights;
    
    // Reverse move
    squares[move.from] = move.piece;
    squares[move.to] = EMPTY; // Will be filled by capture if any
    
    if (move.capturedPiece != EMPTY) {
        if ((move.piece & 0x7) == PAWN && move.to == state.enPassantSquare) {
            // En Passant capture restore
            int captureSquare = (turn == WHITE) ? (move.to - 8) : (move.to + 8);
            squares[captureSquare] = move.capturedPiece;
        } else {
            squares[move.to] = move.capturedPiece;
        }
    }
    
    if (move.isCastling) {
        // Move Rook back
        if (move.to == 6) { squares[7] = squares[5]; squares[5] = EMPTY; }
        else if (move.to == 2) { squares[0] = squares[3]; squares[3] = EMPTY; }
        else if (move.to == 62) { squares[63] = squares[61]; squares[61] = EMPTY; }
        else if (move.to == 58) { squares[56] = squares[59]; squares[59] = EMPTY; }
    }
}

// --- Evaluation ---

// --- Evaluation ---

// Piece-Square Tables (PST)
// Values are from White's perspective. Mirror for Black.
// Flip rank for Black (rank 0 -> rank 7)

const int pawnTable[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

const int knightTable[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

const int bishopTable[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

const int rookTable[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0
};

const int queenTable[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

const int kingTable[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

// Helper to get PST value
int getPstValue(int pieceType, int square, int color) {
    int rank = square / 8;
    int file = square % 8;
    
    if (color == BLACK) {
        rank = 7 - rank; // Mirror rank
        // File mirroring is not strictly necessary for symmetric tables, but good practice
        // Our tables are symmetric horizontally, so file mirroring doesn't change value
    }
    
    int index = rank * 8 + file;
    
    switch (pieceType) {
        case PAWN: return pawnTable[index];
        case KNIGHT: return knightTable[index];
        case BISHOP: return bishopTable[index];
        case ROOK: return rookTable[index];
        case QUEEN: return queenTable[index];
        case KING: return kingTable[index];
        default: return 0;
    }
}

int Board::evaluate() const {
    int score = 0;
    
    // Material values (Centipawns)
    const int pawnValue = 100;
    const int knightValue = 320;
    const int bishopValue = 330;
    const int rookValue = 500;
    const int queenValue = 900;
    const int kingValue = 20000;
    
    for (int i = 0; i < 64; ++i) {
        int piece = squares[i];
        if (piece == EMPTY) continue;
        
        int type = piece & 0x7;
        int color = piece & 0x18;
        
        int value = 0;
        switch (type) {
            case PAWN: value = pawnValue; break;
            case KNIGHT: value = knightValue; break;
            case BISHOP: value = bishopValue; break;
            case ROOK: value = rookValue; break;
            case QUEEN: value = queenValue; break;
            case KING: value = kingValue; break;
        }
        
        // Add PST value
        value += getPstValue(type, i, color);
        
        if (color == WHITE) {
            score += value;
        } else {
            score -= value;
        }
    }
    
    return score;
}
