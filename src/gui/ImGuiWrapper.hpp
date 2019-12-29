#pragma once

#include <string_view>
#include <functional>
#include <string>
#include "imgui.h" // IWYU pragma: export
#include "util/define_logger.hpp"

#define IMGUI_WRAPPER_WIDGET( widget, close, AlwaysClose ) \
class widget {                               \
public:                                      \
    CONSTRUCTORS                             \
    ~widget() {                              \
        TRACE;                               \
        if( AlwaysClose || opened ) close(); \
    }                                        \
    operator bool() & { return opened; }     \
private:                                     \
    bool opened;                             \
}

namespace gui {
    struct WindowConfig {
        std::string title;
        bool* open = nullptr;
        ImGuiWindowFlags flags = 0;
    };

    struct ChildWindowConfig {
        std::string      str_id;
        ImVec2           size;
        bool             border;
        ImGuiWindowFlags flags;
    };

    /**
     * An RAII class to initialize and shutdown ImGui
     */
    struct ImGuiWrapper {
        explicit ImGuiWrapper( std::string_view title = "" );

        ~ImGuiWrapper();

        bool shouldClose();

        void setWindowTitle( std::string_view title );

        /**
         * An RAII class to start and end a frame in ImGui
         */
        class Frame {
            explicit Frame( int fps );

            friend ImGuiWrapper;
        public:
            ~Frame();

            int fps;
        };

        Frame frame( int fps = 60 );

        template< typename Function >
        bool window( WindowConfig& config, Function function ) {
            static_assert( std::is_invocable_v<Function> );

            using detail::GetLogger;
#define CONSTRUCTORS Window(std::string_view title, bool* open, ImGuiWindowFlags flags) { \
    TRACE;                                             \
    opened = ImGui::Begin(title.begin(), open, flags); \
}
            IMGUI_WRAPPER_WIDGET( Window, ImGui::End, true );
#undef CONSTRUCTORS

            TRACE;

            Window w( config.title.c_str(), config.open, config.flags );
            if ( w ) function();
            return w;
        }


        template< typename Function >
        bool childWindow( ChildWindowConfig& config, Function function ) {
            static_assert( std::is_invocable_v<Function> );

            using detail::GetLogger;
#define CONSTRUCTORS ChildWindow(std::string_view str_id, const ImVec2& size, bool border, ImGuiWindowFlags flags) { \
    TRACE;                                                             \
    opened = ImGui::BeginChild( str_id.begin(), size, border, flags ); \
}
            IMGUI_WRAPPER_WIDGET( ChildWindow, ImGui::EndChild, true );
#undef CONSTRUCTORS

            TRACE;

            ChildWindow w( config.str_id.c_str(), config.size, config.border, config.flags );
            if ( w ) function();
            return w;
        }

        class DisableControls {
        private:
            friend ImGuiWrapper;

            explicit DisableControls( bool disable );

        public:
            void disable();

            void enable();

            ~DisableControls();

        private:
            bool disabled;
        };

        DisableControls disableControls( bool disable = true );

        template< typename Function >
        bool mainMenu( Function function ) {
            static_assert( std::is_invocable_v<Function> );

            using detail::GetLogger;
#define CONSTRUCTORS MainMenu() {       \
    TRACE;                              \
    opened = ImGui::BeginMainMenuBar(); \
}
            IMGUI_WRAPPER_WIDGET( MainMenu, ImGui::EndMainMenuBar, false );
#undef CONSTRUCTORS

            TRACE;
            MainMenu m;
            if ( m ) function();
            return m;
        }

        template< typename Function >
        void menu( std::string_view label, bool enabled, Function function ) {
            static_assert( std::is_invocable_v<Function> );

            using detail::GetLogger;
#define CONSTRUCTORS Menu( std::string_view label, bool enabled ) { \
    TRACE;                                                          \
    opened = ImGui::BeginMenu( label.begin(), enabled );            \
}
            IMGUI_WRAPPER_WIDGET( Menu, ImGui::EndMenu, false );
#undef CONSTRUCTORS

            TRACE;
            if ( Menu m( label, enabled ); m ) function();
        }

        template< typename Function >
        void menuItem( std::string_view label, bool selected, bool enabled, Function function ) {
            static_assert( std::is_invocable_v<Function> );
            using detail::GetLogger;
            TRACE;
            if ( ImGui::MenuItem( label.begin(), nullptr, selected, enabled )) function();
        }
    };
}

#undef IMGUI_WRAPPER_WIDGET
