#pragma once

#include <array>
#include <optional>
#include <cstdint>
#include <unordered_map>
#include <boost/container_hash/hash.hpp>

class Checkers {
public:
    enum class CellState {
        EMPTY, RED, BLACK, RED_KING, BLACK_KING
    };
    struct Position { int_fast8_t x, y; };
    struct Move { Position start, end; };
    using BoardState = std::array<std::array<CellState, 8>, 8>;

private:
    struct MoveHash {
        size_t operator()( const Move& move ) const {
            size_t seed{ 0 };
            boost::hash_combine( seed, move.start.x );
            boost::hash_combine( seed, move.start.y );
            boost::hash_combine( seed, move.end.x );
            boost::hash_combine( seed, move.end.y );
            return seed;
        }
    };

public:
    // use the move as the key, and have a pointer to the captured cell as the value
    using LegalMoves = std::unordered_map<Move, CellState*, MoveHash>;

    Checkers();

    Checkers( const Checkers& ) = default;

    Checkers( Checkers&& ) = default;

    Checkers& operator=( const Checkers& ) = default;

    Checkers& operator=( Checkers&& ) = default;

    bool move( Move theMove );

    [[nodiscard]] const BoardState& boardState() const { return boardState_; }

    [[nodiscard]] bool isRedTurn() const { return redTurn; }

    [[nodiscard]] std::optional<Position> movingPiece() const { return pieceMoving; }

    [[nodiscard]] bool canCapture() const { return canCapture_; }

    [[nodiscard]] const LegalMoves& legalMoves() const { return legalMoves_; }

private:
    [[nodiscard]] bool canJump( Position position );

    void calculateLegalMoves();

    void calculateMovesForPosition( Position position );

    void checkRedForwardMoves( Position position );

    void checkBlackForwardMoves( Position position );

    void checkDirection( Position start, Position cell, Position nextCell );

    BoardState              boardState_;
    LegalMoves              legalMoves_;
    std::optional<Position> pieceMoving = std::nullopt;
    bool                    redTurn     = true;
    bool                    canCapture_ = false;
};

inline bool operator==( const Checkers::Position& a, const Checkers::Position& b ) {
    return a.x == b.x && a.y == b.y;
}

inline bool operator!=( const Checkers::Position& a, const Checkers::Position& b ) {
    return !( a == b );
}

inline bool operator==( const Checkers::Move& a, const Checkers::Move& b ) {
    return a.start == b.start && a.end == b.end;
}

inline bool operator!=( const Checkers::Move& a, const Checkers::Move& b ) {
    return !( a == b );
}
