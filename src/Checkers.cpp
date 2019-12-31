#include "Checkers.hpp"

Checkers::Checkers() : boardState_{} {
    for ( int i = 1; i < boardState().size(); i += 2 ) {
        boardState_[ i ][ 0 ] = CellState::BLACK;
    }
    for ( int i = 0; i < boardState().size(); i += 2 ) {
        boardState_[ i ][ 1 ] = CellState::BLACK;
    }
    for ( int i = 1; i < boardState().size(); i += 2 ) {
        boardState_[ i ][ 2 ] = CellState::BLACK;
    }

    for ( int i = 0; i < boardState().size(); i += 2 ) {
        boardState_[ i ][ 7 ] = CellState::RED;
    }
    for ( int i = 1; i < boardState().size(); i += 2 ) {
        boardState_[ i ][ 6 ] = CellState::RED;
    }
    for ( int i = 0; i < boardState().size(); i += 2 ) {
        boardState_[ i ][ 5 ] = CellState::RED;
    }
}

bool Checkers::move( std::pair<int, int> start, std::pair<int, int> end ) {
    // validate indices
    if ( start.first < 0 || start.first >= 8 ) return false;
    if ( start.second < 0 || start.second >= 8 ) return false;
    if ( end.first < 0 || end.first >= 8 ) return false;
    if ( end.second < 0 || end.second >= 8 ) return false;

    // make sure end cell is on the right color square
    if (( end.first + end.second ) % 2 == 0 ) return false;

    CellState& startCell   = boardState_[ start.first ][ start.second ];
    CellState& endCell     = boardState_[ end.first ][ end.second ];
    CellState* captureCell = nullptr;

    // make sure moving from non-empty cell to empty cell
    if ( startCell == CellState::EMPTY ) return false;
    if ( endCell != CellState::EMPTY ) return false;

    // make sure player isn't moving out of turn
    if ( redTurn && startCell != CellState::RED && startCell != CellState::RED_KING ) return false;
    if ( !redTurn && startCell != CellState::BLACK && startCell != CellState::BLACK_KING ) return false;

    // make sure regular pieces aren't moving backwards
    if ( startCell == CellState::RED && end.second > start.second ) return false;
    if ( startCell == CellState::BLACK && end.second < start.second ) return false;

    // make sure piece isn't moving sideways
    if ( start.first == end.first || start.second == end.second ) return false;

    // make sure piece isn't moving too far
    if ( std::abs( start.first - end.first ) > 2 || std::abs( start.second - end.second ) > 2 ) return false;
    if ( std::abs( start.first - end.first ) == 2 ) {
        int xCapture = std::max( start.first, end.first ) - 1;
        int yCapture = std::max( start.second, end.second ) - 1;
        captureCell = &boardState_[ xCapture ][ yCapture ];
        switch ( *captureCell ) {
            case CellState::EMPTY: return false;
            case CellState::RED: [[fallthrough]];
            case CellState::RED_KING: if ( redTurn ) return false;
                break;
            case CellState::BLACK: [[fallthrough]];
            case CellState::BLACK_KING: if ( !redTurn ) return false;
                break;
        }
    }

    // change state of cells and capture piece if applicable
    endCell   = startCell;
    startCell = CellState::EMPTY;
    if ( captureCell ) *captureCell = CellState::EMPTY;

    // promote piece to king if applicable
    if ( endCell == CellState::BLACK && end.second == 7 ) endCell = CellState::BLACK_KING;
    else if ( endCell == CellState::RED && end.second == 0 ) endCell = CellState::RED_KING;

    redTurn = !redTurn;

    return true;
}
