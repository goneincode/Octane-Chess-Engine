#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <iostream>

enum PieceType {
    EMPTY = 0,
    PAWN = 1,
    KNIGHT = 2,
    BISHOP = 3,
    ROOK = 4,
    QUEEN = 5,
    KING = 6
};

enum PieceColor {
    NONE = 0,
    WHITE = 8,
    BLACK = 16
};

// Helper to combine color and type
inline int makePiece(PieceColor color, PieceType type) {
    return (int)color | (int)type;
}

struct Move {
    int from;
    int to;
    int piece;
    int capturedPiece;
    bool isCastling;
    bool isPromotion;
    int promotionPiece; // PieceType to promote to

    Move() : from(-1), to(-1), piece(0), capturedPiece(0), isCastling(false), isPromotion(false), promotionPiece(0) {}
    Move(int f, int t, int p, int cp = 0, bool castle = false, bool promo = false, int pp = 0) 
        : from(f), to(t), piece(p), capturedPiece(cp), isCastling(castle), isPromotion(promo), promotionPiece(pp) {}
};

class Board {
public:
    Board();
    ~Board();

    void setVerbose(bool verbose);
    void printBoard() const;
    void setupStandardPosition();
    
    // Move logic
    bool makeMove(int from, int to); // User friendly wrapper
    bool makeMove(const Move& move); // Engine friendly
    void unmakeMove(const Move& move);
    
    std::vector<Move> generateLegalMoves();

    bool isValidMove(int from, int to) const;
    
    // Evaluation
    int evaluate() const;
    
    // Piece specific logic helpers
    bool isPathClear(int from, int to) const;
    
    // Getters
    int getPieceAt(int square) const;
    int getTurn() const;
    bool isSquareAttacked(int square, int attackerColor) const;
    bool isInCheck(int color) const;

private:
    int squares[64];
    int turn; // WHITE or BLACK
    bool verbose;
    
    // State for special moves
    int enPassantSquare; // -1 if none. Points to the square a pawn can move to for EP capture.
    int castlingRights;  // Bitmask: 1=WK, 2=WQ, 4=BK, 8=BQ
    
    // History for unmake
    struct GameState {
        int enPassantSquare;
        int castlingRights;
        int capturedPiece;
        int halfMoveClock; // For 50 move rule (TODO)
    };
    std::vector<GameState> history;

    // Internal helpers
    void log(const std::string& message) const;
    bool validatePawnMove(int from, int to, int piece, int target) const;
    
    // Move Generation Helpers
    void generatePawnMoves(int square, std::vector<Move>& moves) const;
    void generateKnightMoves(int square, std::vector<Move>& moves) const;
    void generateSlidingMoves(int square, std::vector<Move>& moves, const int directions[][2], int numDirs) const;
    void generateKingMoves(int square, std::vector<Move>& moves) const;
    
    // Low level move execution without validation (for makeMove/unmakeMove)
    void applyMove(const Move& move);
    void undoApplyMove(const Move& move);
    bool validateKnightMove(int from, int to) const;
    bool validateBishopMove(int from, int to) const;
    bool validateRookMove(int from, int to) const;
    bool validateQueenMove(int from, int to) const;
    bool validateKingMove(int from, int to) const;
};

#endif // BOARD_H
