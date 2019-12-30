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

    // make sure moving from non-empty cell to empty cell
    if ( boardState()[ start.first ][ start.second ] == CellState::EMPTY ) return false;
    if ( boardState()[ end.first ][ end.second ] != CellState::EMPTY ) return false;

    // TODO check if move is legal
    boardState_[ end.first ][ end.second ]     = boardState()[ start.first ][ start.second ];
    boardState_[ start.first ][ start.second ] = CellState::EMPTY;
    return true;
}
