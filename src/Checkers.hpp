#pragma once

#include <array>
#include <optional>
#include <cstdint>

class Checkers {
public:
    enum class CellState {
        EMPTY, RED, BLACK, RED_KING, BLACK_KING
    };
    struct Position { int_fast8_t x, y; };
    struct Move { Position start, end; };
    using BoardState = std::array<std::array<CellState, 8>, 8>;

    Checkers();

    Checkers( const Checkers& ) = default;

    Checkers( Checkers&& ) = default;

    Checkers& operator=( const Checkers& ) = default;

    Checkers& operator=( Checkers&& ) = default;

    bool move( Move theMove );

    [[nodiscard]] const BoardState& boardState() const { return boardState_; }

    [[nodiscard]]  bool isRedTurn() const { return redTurn; }

    [[nodiscard]]std::optional<Position> movingPiece() const { return pieceMoving; }

private:
    [[nodiscard]] bool canJump( Position position );

    BoardState              boardState_;
    std::optional<Position> pieceMoving = std::nullopt;
    bool                    redTurn     = true;
};

inline bool operator==( const Checkers::Position& a, const Checkers::Position& b ) {
    return a.x == b.x && a.y == b.y;
}

inline bool operator!=( const Checkers::Position& a, const Checkers::Position& b ) {
    return !( a == b );
}
