add_library ( logging
              util/configure_logging.cpp
              util/configure_logging.hpp
              )
if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    target_link_libraries ( logging PUBLIC log4cplus )
else ()
    target_link_libraries ( logging PUBLIC log4cplusS )
endif ()
target_compile_definitions ( logging PUBLIC APPLICATION_LOG_LEVEL=${LOG_LEVEL}_LOG_LEVEL )

add_library ( imgui
              gui/ImGuiWrapper.hpp
              gui/ImGuiWrapper.cpp
              )
target_link_libraries ( imgui PUBLIC imgui_base logging )
target_compile_definitions ( imgui PUBLIC IMGUI_USER_CONFIG="${PROJECT_SOURCE_DIR}/src/gui/imgui_config.hpp" )

add_library ( image
              gui/Image.hpp
              gui/Image.cpp
              )
target_link_libraries ( image PUBLIC imgui )
target_include_directories ( image PRIVATE ../vendor/embed-resource )

add_library ( sound
              util/SoundSystem.hpp
              util/SoundSystem.cpp
              util/Sound.hpp
              util/Sound.cpp
              )
target_link_libraries ( sound PUBLIC alut logging )
target_include_directories ( sound PRIVATE ../vendor/embed-resource )

add_library ( version version.hpp ${version_file} )

add_library ( checkers
              Checkers.hpp
              Checkers.cpp
              )

add_library ( checkers_window
              CheckersWindow.hpp
              CheckersWindow.cpp
              )
target_link_libraries ( checkers_window PUBLIC imgui image images sound sounds checkers )
target_include_directories ( checkers_window PRIVATE ../vendor/embed-resource )

add_executable ( checkers_exe
                 main.cpp
                 )
target_link_libraries ( checkers_exe
                        checkers_window
                        logging
                        version
                        )
