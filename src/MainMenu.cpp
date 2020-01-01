#include "MainMenu.hpp"
#include "gui/ImGuiWrapper.hpp"
#include "CheckersWindow.hpp"

MainMenu::MainMenu( CheckersWindow& checkersWindow, gui::ImGuiWrapper& imGuiWrapper ) :
        checkersWindow{ checkersWindow }, imGuiWrapper{ imGuiWrapper } {}

void MainMenu::operator()() {
    imGuiWrapper.mainMenu( [ & ] {
        imGuiWrapper.menu( "Game", true, [ this ] {
            imGuiWrapper.menuItem( "New Game", false, true, [ this ] {
                checkersWindow.reset( forceCapture );
            } );
        } );

        imGuiWrapper.menu( "Settings", true, [ this ] {
            imGuiWrapper.menuItem( "Force Capture", forceCapture, true, [ this ] {
                forceCapture = !forceCapture;
                checkersWindow.reset( forceCapture );
            } );
        } );
    } );
}
