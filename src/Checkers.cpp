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

bool Checkers::move( Move theMove ) {
    // check if in middle of double-jumping
    if ( pieceMoving.has_value() && pieceMoving != theMove.start ) return false;

    // validate indices
    if ( theMove.start.x < 0 || theMove.start.x >= 8 ) return false;
    if ( theMove.start.y < 0 || theMove.start.y >= 8 ) return false;
    if ( theMove.end.x < 0 || theMove.end.x >= 8 ) return false;
    if ( theMove.end.y < 0 || theMove.end.y >= 8 ) return false;

    // make sure end cell is on the right color square
    if (( theMove.end.x + theMove.end.y ) % 2 == 0 ) return false;

    CellState& startCell   = boardState_[ theMove.start.x ][ theMove.start.y ];
    CellState& endCell     = boardState_[ theMove.end.x ][ theMove.end.y ];
    CellState* captureCell = nullptr;

    // make sure moving from non-empty cell to empty cell
    if ( startCell == CellState::EMPTY ) return false;
    if ( endCell != CellState::EMPTY ) return false;

    // make sure player isn't moving out of turn
    if ( redTurn && startCell != CellState::RED && startCell != CellState::RED_KING ) return false;
    if ( !redTurn && startCell != CellState::BLACK && startCell != CellState::BLACK_KING ) return false;

    // make sure regular pieces aren't moving backwards
    if ( startCell == CellState::RED && theMove.end.y > theMove.start.y ) return false;
    if ( startCell == CellState::BLACK && theMove.end.y < theMove.start.y ) return false;

    // make sure piece isn't moving sideways
    if ( theMove.start.x == theMove.end.x || theMove.start.y == theMove.end.y ) return false;

    // make sure piece isn't moving too far
    if ( std::abs( theMove.start.x - theMove.end.x ) > 2 || std::abs( theMove.start.y - theMove.end.y ) > 2 )
        return false;
    if ( std::abs( theMove.start.x - theMove.end.x ) == 2 ) {
        int xCapture = std::max( theMove.start.x, theMove.end.x ) - 1;
        int yCapture = std::max( theMove.start.y, theMove.end.y ) - 1;
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
    if ( endCell == CellState::BLACK && theMove.end.y == 7 ) endCell = CellState::BLACK_KING;
    else if ( endCell == CellState::RED && theMove.end.y == 0 ) endCell = CellState::RED_KING;

    if ( !captureCell || !canJump( theMove.end )) {
        pieceMoving = std::nullopt;
        redTurn     = !redTurn;
    }
    else
        pieceMoving = theMove.end;

    return true;
}

bool Checkers::canJump( Position position ) {
    if ( position.x < 0 || position.x >= 8 ||
         position.y < 0 || position.y >= 8 )
        return false;

    const auto checkDirection = [ & ]( CellState cell, CellState nextCell ) {
        return (( redTurn && ( cell == CellState::BLACK || cell == CellState::BLACK_KING )) ||
                ( !redTurn && ( cell == CellState::RED || cell == CellState::RED_KING )))
               && nextCell == CellState::EMPTY;
    };

    switch ( boardState()[ position.x ][ position.y ] ) {
        case CellState::EMPTY: return false;
        case CellState::RED: {
            if ( position.y <= 1 ) return false;
            return ( position.x >= 2 &&
                     checkDirection( boardState()[ position.x - 1 ][ position.y - 1 ],
                                     boardState()[ position.x - 2 ][ position.y - 2 ] )) ||
                   ( position.x <= 5 &&
                     checkDirection( boardState()[ position.x + 1 ][ position.y - 1 ],
                                     boardState()[ position.x + 2 ][ position.y - 2 ] ));
        }
        case CellState::BLACK: {
            if ( position.y >= 6 ) return false;
            return ( position.x >= 2 &&
                     checkDirection( boardState()[ position.x - 1 ][ position.y + 1 ],
                                     boardState()[ position.x - 2 ][ position.y + 2 ] )) ||
                   ( position.x <= 5 &&
                     checkDirection( boardState()[ position.x + 1 ][ position.y + 1 ],
                                     boardState()[ position.x + 2 ][ position.y + 2 ] ));
        }
        case CellState::RED_KING: [[fallthrough]];
        case CellState::BLACK_KING: {
            return ( position.x >= 2 && position.y >= 2 &&
                     checkDirection( boardState()[ position.x - 1 ][ position.y - 1 ],
                                     boardState()[ position.x - 2 ][ position.y - 2 ] )) ||
                   ( position.x >= 2 && position.y <= 5 &&
                     checkDirection( boardState()[ position.x - 1 ][ position.y + 1 ],
                                     boardState()[ position.x - 2 ][ position.y + 2 ] )) ||
                   ( position.x <= 5 && position.y >= 2 &&
                     checkDirection( boardState()[ position.x + 1 ][ position.y - 1 ],
                                     boardState()[ position.x + 2 ][ position.y - 2 ] )) ||
                   ( position.x <= 5 && position.y <= 5 &&
                     checkDirection( boardState()[ position.x + 1 ][ position.y + 1 ],
                                     boardState()[ position.x + 2 ][ position.y + 2 ] ));
        }
    }
}
