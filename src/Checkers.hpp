#pragma once

#include <array>
#include <optional>

class Checkers {
public:
    enum class CellState {
        EMPTY, RED, BLACK, RED_KING, BLACK_KING
    };

    using BoardState = std::array<std::array<CellState, 8>, 8>;

    Checkers();

    Checkers( const Checkers& ) = default;

    Checkers( Checkers&& ) = default;

    Checkers& operator=( const Checkers& ) = default;

    Checkers& operator=( Checkers&& ) = default;

    bool move( std::pair<int, int> start, std::pair<int, int> end );

    [[nodiscard]] const BoardState& boardState() const { return boardState_; }

    [[nodiscard]]  bool isRedTurn() const { return redTurn; }

    [[nodiscard]]std::optional<std::pair<int, int>> movingPiece() const { return pieceMoving; }

private:
    [[nodiscard]] bool canJump( std::pair<int, int> position );

    BoardState                         boardState_;
    std::optional<std::pair<int, int>> pieceMoving = std::nullopt;
    bool                               redTurn     = true;
};



