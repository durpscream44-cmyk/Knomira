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
