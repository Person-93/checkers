#pragma once

class CheckersWindow;
namespace gui {
    struct ImGuiWrapper;
}

class MainMenu {
public:
    MainMenu( CheckersWindow& checkersWindow, gui::ImGuiWrapper& imGuiWrapper );

    void operator()();

private:
    CheckersWindow   & checkersWindow;
    gui::ImGuiWrapper& imGuiWrapper;
    bool forceCapture = true;
};



