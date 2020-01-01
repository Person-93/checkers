#include "Checkers.hpp"

Checkers::Checkers( bool forceCapture ) : boardState_{}, forceCapture{ forceCapture } {
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

    calculateLegalMoves();
}

bool Checkers::move( Move theMove ) {
    auto iter = legalMoves_.find( theMove );
    if ( iter == legalMoves_.end()) return false;
    if ( forceCapture && canCapture_ && iter->second == nullptr ) return false;

    CellState& startCell   = boardState_[ theMove.start.x ][ theMove.start.y ];
    CellState& endCell     = boardState_[ theMove.end.x ][ theMove.end.y ];
    CellState* captureCell = iter->second;

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

    calculateLegalMoves();
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

void Checkers::calculateLegalMoves() {
    legalMoves_.clear();

    if ( pieceMoving.has_value()) {
        calculateMovesForPosition( *pieceMoving );
        return;
    }

    canCapture_ = false;
    for ( int_fast8_t i = 0; i < 8; ++i ) {
        for ( int_fast8_t j = 0; j < 8; ++j ) {
            calculateMovesForPosition( { i, j } );
        }
    }
}

void Checkers::calculateMovesForPosition( Position position ) {
    switch ( boardState()[ position.x ][ position.y ] ) {
        case CellState::EMPTY: break;
        case CellState::RED:
            if ( isRedTurn())
                checkRedForwardMoves( position );
            break;
        case CellState::BLACK:
            if ( !isRedTurn())
                checkBlackForwardMoves( position );
            break;
        case CellState::RED_KING:
            if ( isRedTurn()) {
                checkRedForwardMoves( position );
                checkBlackForwardMoves( position );
            }
            break;
        case CellState::BLACK_KING:
            if ( !isRedTurn()) {
                checkRedForwardMoves( position );
                checkBlackForwardMoves( position );
            }
            break;
    }
}

void Checkers::checkRedForwardMoves( Position position ) {
    checkDirection( position,
                    { static_cast<int_fast8_t>(position.x - 1), static_cast<int_fast8_t>(position.y - 1 ) },
                    { static_cast<int_fast8_t>(position.x - 2), static_cast<int_fast8_t>(position.y - 2) } );
    checkDirection( position,
                    { static_cast<int_fast8_t>(position.x + 1), static_cast<int_fast8_t>(position.y - 1) },
                    { static_cast<int_fast8_t>(position.x + 2), static_cast<int_fast8_t>(position.y - 2 ) } );
}

void Checkers::checkBlackForwardMoves( Position position ) {
    checkDirection( position,
                    { static_cast<int_fast8_t>(position.x - 1), static_cast<int_fast8_t>(position.y + 1) },
                    { static_cast<int_fast8_t>(position.x - 2), static_cast<int_fast8_t>(position.y + 2) } );
    checkDirection( position,
                    { static_cast<int_fast8_t>(position.x + 1), static_cast<int_fast8_t>(position.y + 1) },
                    { static_cast<int_fast8_t>(position.x + 2), static_cast<int_fast8_t>(position.y + 2) } );
}

void Checkers::checkDirection( Position start, Position cell, Position nextCell ) {
    const auto validateCell = []( const Position& position ) -> bool {
        return position.x >= 0 && position.x < 8 && position.y >= 0 && position.y < 8;
    };

    CellState& cellState = boardState_[ cell.x ][ cell.y ];

    if ( validateCell( cell ) && cellState == CellState::EMPTY ) {
        legalMoves_.insert( {{ start, cell }, nullptr } );
        return;
    }

    if ( !validateCell( nextCell )) return;

    // can't jump own piece
    if ( bool isRedPiece = cellState == CellState::RED || cellState == CellState::RED_KING;
            isRedPiece == redTurn )
        return;

    if ( CellState nextCellState = boardState_[ nextCell.x ][ nextCell.y ];
            nextCellState == CellState::EMPTY ) {
        legalMoves_.insert( {{ start, nextCell }, &cellState } );
        canCapture_ = true;
    }
}
