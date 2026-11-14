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

# Noesis-free executable: Qt + RmlUi + original save metadata discovery.
# A separate executable to protect the verified Qt/Xbox prototype.
option(KNOMIRA_BUILD_GAME_SHELL "Build the next Noesis-free game shell milestone" OFF)
if(KNOMIRA_BUILD_GAME_SHELL)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Xml)
    add_executable(knomira_game_shell
        qt_host.cpp
        save_catalog.cpp
        save_catalog.h
        input_bindings.cpp
        input_bindings.h
        "${rmlui_SOURCE_DIR}/Backends/RmlUi_Renderer_GL3.cpp"
    )
    target_compile_definitions(knomira_game_shell PRIVATE KNOMIRA_GAME_SHELL=1)
    target_include_directories(knomira_game_shell PRIVATE
        "${rmlui_SOURCE_DIR}/Backends"
    )
    target_link_libraries(knomira_game_shell PRIVATE
        RmlUi::RmlUi Qt6::Core Qt6::Gui
    )
    add_custom_command(TARGET knomira_game_shell POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/game_shell.rml"
            "$<TARGET_FILE_DIR:knomira_game_shell>/game_shell.rml"
        # The original DB::init expects <appDir>/content/db/ingnomia.db.sql.
        # Stage only the original read-only database asset at this milestone.
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:knomira_game_shell>/content/db"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/../../content/db/ingnomia.db.sql"
            "$<TARGET_FILE_DIR:knomira_game_shell>/content/db/ingnomia.db.sql"
        # The original Global::loadBehaviorTrees consumes content/ai;
        # NewGameSettings falls back to content/JSON/newgame.json.
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/../../content/ai"
            "$<TARGET_FILE_DIR:knomira_game_shell>/content/ai"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/../../content/JSON"
            "$<TARGET_FILE_DIR:knomira_game_shell>/content/JSON"
        VERBATIM
    )
    if(MSVC)
        target_compile_options(knomira_game_shell PRIVATE /W4)
    endif()
    add_executable(knomira_save_catalog_test
        save_catalog_test.cpp
        save_catalog.cpp
        save_catalog.h
    )
    target_link_libraries(knomira_save_catalog_test PRIVATE Qt6::Core)

    add_executable(knomira_input_bindings_test
        input_bindings_test.cpp
        input_bindings.cpp
        input_bindings.h
    )
    target_link_libraries(knomira_input_bindings_test PRIVATE Qt6::Core Qt6::Gui)

    # UI geometry regression test: uses the real RmlUi layout engine and
    # a no-op renderer, so it runs in CI without a window or OpenGL context.
    add_executable(knomira_rmlui_layout_test controls_layout_test.cpp)
    target_link_libraries(knomira_rmlui_layout_test PRIVATE RmlUi::RmlUi)

    # The game's world renderer header must not drag in NoesisGUI transitively.
    # Compile-only test: the actual engine implementation is not linked here.
    add_library(knomira_world_renderer_header_compile OBJECT
        world_renderer_header_compile.cpp
    )
    target_include_directories(knomira_world_renderer_header_compile PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/glad/include"
    )
    target_link_libraries(knomira_world_renderer_header_compile PRIVATE
        Qt6::Core Qt6::Gui Qt6::Xml
    )

    # First real C++ simulation-source compile gate. Unlike the existing
    # public-header test, this compiles selected original engine .cpp files
    # independently from Noesis and the legacy game's forced PCH. This does
    # not link GameManager or run a colony yet.
    find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Sql)
    add_library(knomira_simulation_source_compile OBJECT
        "../../src/game/game.cpp"
        "../../src/base/config.cpp"
        "../../src/base/config_persistence.cpp"
        "../../src/base/db.cpp"
        "../../src/base/gamestate.cpp"
        "../../src/base/global.cpp"
        "../../src/base/io.cpp"
        "../../src/base/io_data_root.cpp"
        "../../src/base/io_settings_file.cpp"
        "../../src/game/gamemanager.cpp"
        "../../src/gui/strings.cpp"
        "../../src/game/newgamesettings.cpp"
        "../../src/game/techtree.cpp"
        "../../src/base/pathfinder.cpp"
        "../../src/gui/eventconnector.cpp"
    )
    # The original game's executable compiles its sources as C++23.
    # Compile the real Qt signal/slot meta-object code alongside the game's
    # sources; otherwise a compile-only gate can conceal moc errors until link.
    set_target_properties(knomira_simulation_source_compile PROPERTIES
        CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON AUTOMOC ON
    )
    # This is a compile-only integration/test target, never the shipping game.
    # Ensure any future smoke executable reusing these objects cannot silently
    # create files in Documents/My Games/Ingnomia.
    target_compile_definitions(knomira_simulation_source_compile PRIVATE
        KNOMIRA_ISOLATED_TEST_DATA_ROOT=1
    )
    target_include_directories(knomira_simulation_source_compile PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/glad/include"
    )
    target_link_libraries(knomira_simulation_source_compile PRIVATE
        Qt6::Core Qt6::Gui Qt6::Xml Qt6::Widgets Qt6::Sql
    )

    # A linkable, read-only game-data bootstrap test. Executes the actual
    # game's SQL asset in SQLite :memory:, checking the tables needed for
    # gameplay initialization without creating config files or save folders.
    add_executable(knomira_engine_db_bootstrap_test
        engine_database_smoke.cpp
    )
    target_link_libraries(knomira_engine_db_bootstrap_test PRIVATE
        Qt6::Core Qt6::Sql Qt6::Xml
    )

    # First linked original Config constructor test. It links only actual
    # Config/IO startup translation units and must always run with a marked
    # temporary root. Never attach this macro to shipping targets.
    add_executable(knomira_engine_config_bootstrap_test
        engine_config_bootstrap_test.cpp
        "../../src/base/config.cpp"
        "../../src/base/io_data_root.cpp"
        "../../src/base/io_settings_file.cpp"
    )
    target_compile_definitions(knomira_engine_config_bootstrap_test PRIVATE
        KNOMIRA_ISOLATED_TEST_DATA_ROOT=1
    )
    target_link_libraries(knomira_engine_config_bootstrap_test PRIVATE Qt6::Core)

    # Test the same data-folder resolver used by IO::getDataFolder, but with
    # test-only compile guard. Production keeps Documents/My Games/Ingnomia.
    add_executable(knomira_io_data_root_test
        io_data_root_test.cpp
        "../../src/base/io_data_root.cpp"
    )
    target_compile_definitions(knomira_io_data_root_test PRIVATE
        KNOMIRA_ISOLATED_TEST_DATA_ROOT=1
    )
    target_link_libraries(knomira_io_data_root_test PRIVATE Qt6::Core)

    # Second compilation of the same test with NO test-only macro verifies
    # the shipping default ignores KNOMIRA_TEST_DATA_ROOT.
    add_executable(knomira_io_data_root_production_test
        io_data_root_test.cpp
        "../../src/base/io_data_root.cpp"
    )
    target_link_libraries(knomira_io_data_root_production_test PRIVATE Qt6::Core)

    # Compile-only check for real game startup and command interfaces.
    # No proprietary GUI libraries or precompiled headers.
    add_library(knomira_engine_headers_compile OBJECT
        engine_headers_compile.cpp
    )
    target_link_libraries(knomira_engine_headers_compile PRIVATE
        Qt6::Core Qt6::Gui Qt6::Xml
    )
endif()
