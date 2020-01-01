#pragma once

#include "Checkers.hpp"
#include "gui/ImGuiWrapper.hpp"
#include "gui/Image.hpp"
#include "util/SoundSystem.hpp"
#include "util/Sound.hpp"

class CheckersWindow {
public:
    explicit CheckersWindow( gui::ImGuiWrapper& imGuiWrapper );

    void operator()();

    void reset( bool forceCapture );

    [[nodiscard]] bool gameOver() const { return checkers.legalMoves().empty(); }

    [[nodiscard]] bool isRedTurn() const { return checkers.isRedTurn(); }

private:
    void initializeValues();

    void drawPieces();

    void handleClickAndDrag();

    Checkers checkers{ true };
    gui::ImGuiWrapper& imGuiWrapper;
    gui::WindowConfig                 config;
    std::optional<Checkers::Position> pieceDragging;
    std::optional<Checkers::Position> squareHovered;
    gui::Image                        board;
    gui::Image                        blackPiece;
    gui::Image                        redPiece;
    gui::Image                        blackKing;
    gui::Image                        redKing;
    SoundSystem                       soundSystem;
    Sound                             knock;

    float xOffset;
    float yOffset;
    float xCellSize;
    float yCellSize;
    float xPieceSize;
    float yPieceSize;
};



