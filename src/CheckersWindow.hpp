#pragma once

#include "Checkers.hpp"
#include "gui/ImGuiWrapper.hpp"
#include "gui/Image.hpp"

class CheckersWindow {
public:
    explicit CheckersWindow( gui::ImGuiWrapper& imGuiWrapper );

    void operator()();

private:
    void initializeValues();

    void drawPieces();

    void handleClickAndDrag();

    Checkers checkers{};
    gui::ImGuiWrapper& imGuiWrapper;
    gui::WindowConfig                  config;
    std::optional<std::pair<int, int>> pieceDragging;
    std::optional<std::pair<int, int>> squareHovered;
    gui::Image                         board;
    gui::Image                         blackPiece;
    gui::Image                         redPiece;
    gui::Image                         blackKing;
    gui::Image                         redKing;

    float xOffset;
    float yOffset;
    float xCellSize;
    float yCellSize;
    float xPieceSize;
    float yPieceSize;
};



