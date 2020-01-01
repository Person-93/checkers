#include <atomic>
#include <csignal>
#include <boost/exception/diagnostic_information.hpp>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include "version.hpp"
#include "util/configure_logging.hpp"
#include "gui/ImGuiWrapper.hpp"
#include "CheckersWindow.hpp"
#include "MainMenu.hpp"

std::atomic_bool shouldRun = true;

extern "C" void signalHandler( int ) { shouldRun = false; }

void gameOverWindow( gui::ImGuiWrapper& imGuiWrapper, bool isRedTurn );

int main() try {
    std::signal( SIGTERM, signalHandler );
    util::ConfigureLogging();
    LOG4CPLUS_DEBUG( log4cplus::Logger::getRoot(), "Running version: " << version::longVersion());

    gui::ImGuiWrapper imGuiWrapper{ "Checkers" };
    CheckersWindow    checkersWindow{ imGuiWrapper };
    MainMenu          mainMenu{ checkersWindow, imGuiWrapper };

    while ( shouldRun && !imGuiWrapper.shouldClose()) {
        auto f = imGuiWrapper.frame( 20 );
        mainMenu();
        checkersWindow();
        if ( checkersWindow.gameOver()) gameOverWindow( imGuiWrapper, checkersWindow.isRedTurn());
    }

    LOG4CPLUS_DEBUG( log4cplus::Logger::getRoot(), "Application exiting normally" );
    return 0;
}
catch ( ... ) {
    LOG4CPLUS_FATAL( log4cplus::Logger::getRoot(), boost::current_exception_diagnostic_information( true ));
    return -1;
}

void gameOverWindow( gui::ImGuiWrapper& imGuiWrapper, bool isRedTurn ) {
    static gui::WindowConfig config{ .title = "Game Over" };
    imGuiWrapper.window( config, [ & ]() {
        ImGui::Text( "%s wins!", isRedTurn ? "Black" : "Red" );
    } );
}
