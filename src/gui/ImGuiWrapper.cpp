#include "ImGuiWrapper.hpp" // IWYU pragma: associated
#include <GL/glcorearb.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>
#include "Exception.hpp"
#include "imgui_internal.h"
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>
#include "util/define_logger.hpp"


using namespace gui;

DEFINE_LOGGER( ImGuiWrapper );

namespace {
    GLFWwindow* glfwWindow = nullptr;
    constexpr int width  = 1280;
    constexpr int height = 720;
    ImVec4* clear_color = nullptr;

    struct {
        int error;
        const char* description;
    } err;

    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    TimePoint frameStart;
}

ImGuiWrapper::ImGuiWrapper( std::string_view title ) {
    TRACE;

    if ( glfwWindow ) {
        using namespace exception;
        THROW_EXCEPTION( Exception{} << Because( "There can only be one instance of ImGuiWrapper" ));
    }
    if ( !clear_color ) clear_color = new ImVec4{ 0.45f, 0.55f, 0.60f, 1.00f };

    // Setup window
    glfwSetErrorCallback( []( int error, const char* description ) {
        err.error       = error;
        err.description = description;
    } );

    if ( !glfwInit()) {
        std::ostringstream ss;
        ss << "GLFW Error " << err.error << ": " << err.description;

        using namespace exception;
        THROW_EXCEPTION( Exception{} << Because( ss.str()));
    }

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint( GLFW_SAMPLES, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
    glfwWindowHint( GLFW_MAXIMIZED, GLFW_TRUE );

    // Create window with graphics context
    glfwWindow = glfwCreateWindow( width, height, title.begin(), nullptr, nullptr );
    if ( glfwWindow == nullptr ) {
        using namespace exception;
        THROW_EXCEPTION( Exception{} << Because( "Unable to create GLFW window" ));
    }

    glfwMakeContextCurrent( glfwWindow );
    glfwSwapInterval( 1 ); // Enable vsync

    if ( gl3wInit()) {
        static struct {
            int major, minor;
        } version;
        glGetIntegerv( GL_MAJOR_VERSION, &version.major );
        glGetIntegerv( GL_MINOR_VERSION, &version.minor );
        std::ostringstream ss;
        ss << "Failed to initialize OpenGL loader. Version is: " << version.major << '.' << version.minor;

        using namespace exception;
        THROW_EXCEPTION( Exception{} << Because( ss.str()));
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL( glfwWindow, true );
    ImGui_ImplOpenGL3_Init( glsl_version );
}

ImGuiWrapper::~ImGuiWrapper() {
    TRACE;

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow( glfwWindow );
    glfwTerminate();
    glfwWindow = nullptr;
}

ImGuiWrapper::Frame ImGuiWrapper::frame( int fps ) {
    TRACE;
    return ImGuiWrapper::Frame( fps );
}

bool inFrame = false;

ImGuiWrapper::Frame::Frame( int fps ) : fps{ fps } {
    TRACE;

    frameStart = TimePoint{};

    if ( inFrame ) {
        using namespace exception;
        THROW_EXCEPTION( Exception{} << Because( "Cannot start new frame until previous frame is completed" ));
    }
    inFrame = true;

    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

ImGuiWrapper::Frame::~Frame() {
    TRACE;

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwMakeContextCurrent( glfwWindow );
    glfwGetFramebufferSize( glfwWindow, &display_w, &display_h );
    glViewport( 0, 0, display_w, display_h );
    glClearColor( clear_color->x, clear_color->y, clear_color->z, clear_color->w );
    glClear( GL_COLOR_BUFFER_BIT );
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData());

    glfwMakeContextCurrent( glfwWindow );
    glfwSwapBuffers( glfwWindow );


    std::this_thread::sleep_until( frameStart + std::chrono::seconds{ 1 } / fps );

    inFrame = false;
}

bool ImGuiWrapper::shouldClose() {
    TRACE;
    return bool( glfwWindowShouldClose( glfwWindow ));
}

void ImGuiWrapper::setWindowTitle( std::string_view title ) {
    TRACE;
    LOG4CPLUS_INFO( GetLogger(), "Window title was set to " << title );
    glfwSetWindowTitle( glfwWindow, title.begin());
}

ImGuiWrapper::DisableControls ImGuiWrapper::disableControls( bool disable ) {
    TRACE;
    return ImGuiWrapper::DisableControls( disable );
}

ImGuiWrapper::DisableControls::DisableControls( bool disable ) : disabled( false ) {
    TRACE;
    if ( disable ) this->disable();
}

ImGuiWrapper::DisableControls::~DisableControls() {
    TRACE;
    enable();
}

void ImGuiWrapper::DisableControls::disable() {
    TRACE;
    if ( !disabled ) {
        ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
        ImGui::PushStyleVar( ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f );
        disabled = true;
    }
}

void ImGuiWrapper::DisableControls::enable() {
    TRACE;
    if ( disabled ) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
        disabled = false;
    }
}

#include "imgui_config.hpp"

log4cplus::Logger& gui::detail::GetLogger() { return ::GetLogger(); }
