cmake_minimum_required(VERSION 3.24)
project(KnomiraRmlUiPrototype VERSION 0.1 LANGUAGES CXX)

# Standalone experiment: no dependency on the game's Noesis-based CMake.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include(FetchContent)

set(RMLUI_SAMPLES OFF CACHE BOOL "" FORCE)
set(RMLUI_SHELL ON CACHE BOOL "Enable upstream example backends" FORCE)
set(RMLUI_BACKEND GLFW_GL3 CACHE STRING "" FORCE)
set(RMLUI_FONT_ENGINE freetype CACHE STRING "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Static RmlUi prototype" FORCE)

find_package(Freetype REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

FetchContent_Declare(rmlui
    GIT_REPOSITORY https://github.com/mikke89/RmlUi.git
    GIT_TAG 6.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(rmlui)

add_executable(knomira_rmlui_prototype main.cpp)
target_link_libraries(knomira_rmlui_prototype PRIVATE
    RmlUi::RmlUi
    rmlui_backend_GLFW_GL3
)
# Copy the RML document to the runtime output directory for both CI and local builds.
add_custom_command(TARGET knomira_rmlui_prototype POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/main_menu.rml"
        "$<TARGET_FILE_DIR:knomira_rmlui_prototype>/main_menu.rml"
    VERBATIM
)
if(MSVC)
    target_compile_options(knomira_rmlui_prototype PRIVATE /W4)
else()
    target_compile_options(knomira_rmlui_prototype PRIVATE -Wall -Wextra)
endif()

# Opt-in next-stage test: host RmlUi in a real Qt QWindow/QOpenGLContext.
# This verifies the game's windowing approach without altering the game.
option(KNOMIRA_BUILD_QT_HOST "Build Qt-hosted RmlUi integration proof of concept" OFF)
if(KNOMIRA_BUILD_QT_HOST)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui)
    add_executable(knomira_rmlui_qt_host
        qt_host.cpp
        "${rmlui_SOURCE_DIR}/Backends/RmlUi_Renderer_GL3.cpp"
    )
    target_include_directories(knomira_rmlui_qt_host PRIVATE
        "${rmlui_SOURCE_DIR}/Backends"
    )
    target_link_libraries(knomira_rmlui_qt_host PRIVATE
        RmlUi::RmlUi Qt6::Core Qt6::Gui
    )
    # XInput loads dynamically from Windows; no SDK import-library link needed.
    add_custom_command(TARGET knomira_rmlui_qt_host POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/main_menu.rml"
            "$<TARGET_FILE_DIR:knomira_rmlui_qt_host>/main_menu.rml"
        VERBATIM
    )
    if(MSVC)
        target_compile_options(knomira_rmlui_qt_host PRIVATE /W4)
    endif()

    # Compile the exact adapter used by the real MainWindow integration,
    # even without the proprietary NoesisSDK. Keep this as a compile-only
    # static library to catch Qt/RmlUi/GLAD header errors on Windows CI.
    add_library(knomira_rmlui_engine_adapter_compile STATIC
        engine_overlay.cpp
        "${rmlui_SOURCE_DIR}/Backends/RmlUi_Renderer_GL3.cpp"
    )
    target_include_directories(knomira_rmlui_engine_adapter_compile PRIVATE
        "${rmlui_SOURCE_DIR}/Backends"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/glad/include"
    )
    target_compile_definitions(knomira_rmlui_engine_adapter_compile PRIVATE
        "RMLUI_GL3_CUSTOM_LOADER=<glad/gl.h>"
    )
    target_link_libraries(knomira_rmlui_engine_adapter_compile PRIVATE
        RmlUi::RmlUi Qt6::Core Qt6::Gui
    )
endif()
