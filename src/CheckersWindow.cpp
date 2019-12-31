#include "CheckersWindow.hpp"
#include "Resource.h"
#include "util/define_logger.hpp"
#include "Exception.hpp"

DEFINE_LOGGER( CheckersWindow )

CheckersWindow::CheckersWindow( gui::ImGuiWrapper& imGuiWrapper ) :
        imGuiWrapper{ imGuiWrapper },
        config{
                .title = "Game",
                .flags = ImGuiWindowFlags_NoDecoration
        },
        board( imGuiWrapper, LOAD_RESOURCE( CheckersBoard_jpg )),
        blackPiece( imGuiWrapper, LOAD_RESOURCE( BlackPiece_png )),
        redPiece( imGuiWrapper, LOAD_RESOURCE( RedPiece_png )),
        blackKing( imGuiWrapper, LOAD_RESOURCE( BlackKing_png )),
        redKing( imGuiWrapper, LOAD_RESOURCE( RedKing_png )),
        knock( soundSystem.makeSound( LOAD_RESOURCE( knock_wav ))) {}

void CheckersWindow::operator()() {
    auto displaySize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos( { displaySize.x * .5f, displaySize.y * .5f }, 0, { .5f, .5f } );
    ImGui::SetNextWindowSize( { 720, 720 } );
    imGuiWrapper.window( config, [ & ] {
        ImGui::SetCursorPos( { 0, 0 } );
        ImGui::Image( reinterpret_cast<void*> (board.texture()), { 720, 720 } );
        initializeValues();
        drawPieces();
        handleClickAndDrag();
    } );


}

void CheckersWindow::initializeValues() {
    constexpr int imgSize     = 2048;
    constexpr int borderSize  = 138;
    constexpr int imgCellSize = 222;

    const auto windowSize = ImGui::GetWindowSize();
    xOffset    = windowSize.x * borderSize / imgSize;
    yOffset    = windowSize.y * borderSize / imgSize;
    xCellSize  = windowSize.x * imgCellSize / imgSize;
    yCellSize  = windowSize.y * imgCellSize / imgSize;
    xPieceSize = windowSize.x / 10;
    yPieceSize = windowSize.y / 10;

    const auto   mousePos      = ImGui::GetIO().MousePos;
    const auto   windowPos     = ImGui::GetWindowPos();
    const ImVec2 mouseLocalPos = { mousePos.x - windowPos.x, mousePos.y - windowPos.y };
    const ImVec2 mouseBoardPos = { mouseLocalPos.x - borderSize * windowSize.x / imgSize,
                                   mouseLocalPos.y - borderSize * windowSize.y / imgSize };
    const ImVec2 scaledCellSize{ imgCellSize * windowSize.x / imgSize, imgCellSize * windowSize.y / imgSize };
    squareHovered     = { mouseBoardPos.x / scaledCellSize.x, mouseBoardPos.y / scaledCellSize.y };
    if ( squareHovered->first < 0 || squareHovered->first >= 8 ||
         squareHovered->second < 0 || squareHovered->second >= 8 )
        squareHovered = std::nullopt;

    ImGui::Begin( "Debugging Info" );
    ImGui::Text( "Hovered: %d, %d", squareHovered ? squareHovered->first : -1,
                 squareHovered ? squareHovered->second : -1 );
    ImGui::Text( "Dragging: %d, %d", pieceDragging ? pieceDragging->first : -1,
                 pieceDragging ? pieceDragging->second : -1 );
    ImGui::Text( "Mouse: %.0f, %.0f", mouseBoardPos.x, mouseBoardPos.y );
    const auto piece = checkers.movingPiece();
    ImGui::Text( "Moving: %d, %d", piece ? piece->first : -1, piece ? piece->second : -1 );
    ImGui::End();
}

void CheckersWindow::drawPieces() {
    ImGui::PushStyleColor( ImGuiCol_Button, { 0, 0, 0, 0 } );
    struct Finally {
        ~Finally() {
            ImGui::PopStyleColor();
        }
    } finally;

    for ( int i = 0; i < 8; ++i ) {
        for ( int j = 0; j < 8; ++j ) {
            // skip piece currently being moved
            if ( pieceDragging.has_value() && pieceDragging->first == i && pieceDragging->second == j ) continue;

            gui::Image* image;
            switch ( checkers.boardState()[ i ][ j ] ) {
                case Checkers::CellState::EMPTY: continue;
                case Checkers::CellState::RED: image = &redPiece;
                    break;
                case Checkers::CellState::BLACK: image = &blackPiece;
                    break;
                case Checkers::CellState::RED_KING: image = &redKing;
                    break;
                case Checkers::CellState::BLACK_KING: image = &blackKing;
                    break;
            }

            ImGui::SetCursorPos( { xOffset + (float) i * xCellSize,
                                   yOffset + (float) j * yCellSize } );
            ImGui::Image( reinterpret_cast<void*>(image->texture()), { xPieceSize, yPieceSize } );
        }
    }


    // draw the piece currently being moved
    if ( pieceDragging.has_value()) {
        gui::Image* image;
        switch ( checkers.boardState().at( pieceDragging->first ).at( pieceDragging->second )) {
            case Checkers::CellState::EMPTY: THROW_SIMPLE_EXCEPTION( "Empty piece is moving" );
            case Checkers::CellState::RED: image = &redPiece;
                break;
            case Checkers::CellState::BLACK: image = &blackPiece;
                break;
            case Checkers::CellState::RED_KING: image = &redKing;
                break;
            case Checkers::CellState::BLACK_KING: image = &blackKing;
                break;
        }

        const auto   mousePos = ImGui::GetIO().MousePos;
        const ImVec2 pos{ mousePos.x - .5f * xPieceSize, mousePos.y - .5f * yPieceSize };
        ImGui::SetCursorScreenPos( pos );
        ImGui::Image( reinterpret_cast<void*>(image->texture()), { xPieceSize, yPieceSize } );
    }
}

void CheckersWindow::handleClickAndDrag() {
    if ( ImGui::IsMouseDragging( 0 )) {
        if ( !pieceDragging.has_value() && squareHovered.has_value()) {
            auto cell = checkers.boardState()[ squareHovered->first ][ squareHovered->second ];
            if ( cell == Checkers::CellState::EMPTY ) return;

            bool isRedTurn  = checkers.isRedTurn();
            bool isRedPiece = cell == Checkers::CellState::RED || cell == Checkers::CellState::RED_KING;

            if ( isRedPiece == isRedTurn )
                pieceDragging = squareHovered;
        }
    }
    else if ( pieceDragging.has_value()) {
        if ( squareHovered.has_value() && pieceDragging != squareHovered )
            if ( checkers.move( *pieceDragging, *squareHovered ))
                knock.play();
        pieceDragging = std::nullopt;
    }
}
