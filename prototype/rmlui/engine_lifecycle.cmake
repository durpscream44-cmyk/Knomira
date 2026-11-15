# Test-only link of the original simulation. Never reuse these objects in a
# shipping executable: every IO entry point is compiled with the temp-root guard.
find_package(OpenAL REQUIRED)
add_subdirectory("../../3rdparty/exprtk" "engine-deps/exprtk")
add_subdirectory("../../3rdparty/fastnoise" "engine-deps/fastnoise")

file(GLOB_RECURSE lifecycle_engine_sources CONFIGURE_DEPENDS
    "../../src/base/*.cpp" "../../src/base/*.h"
    "../../src/game/*.cpp" "../../src/game/*.h"
    "../../src/gfx/*.cpp" "../../src/gfx/*.h")
# Crash reporting belongs to the desktop entry point and requires its SDK.
list(FILTER lifecycle_engine_sources EXCLUDE REGEX "/crashhandler\\.(cpp|h)$")
file(GLOB lifecycle_aggregator_sources CONFIGURE_DEPENDS
    "../../src/gui/aggregator*.cpp" "../../src/gui/aggregator*.h")
add_library(knomira_lifecycle_engine STATIC
    ${lifecycle_engine_sources} ${lifecycle_aggregator_sources}
    ../../src/gui/eventconnector.cpp ../../src/gui/eventconnector.h
    ../../src/gui/strings.cpp ../../src/gui/strings.h
    ../../src/gui/openalwrapper.cpp ../../src/gui/openalwrapper.h)
set_target_properties(knomira_lifecycle_engine PROPERTIES
    CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON AUTOMOC ON)
# Original sources assume common Qt headers. This contains no legacy game PCH,
# Noesis headers, replacement simulation objects, or stub aggregators.
target_precompile_headers(knomira_lifecycle_engine PRIVATE <QtCore> <QWidget>)
target_compile_definitions(knomira_lifecycle_engine PUBLIC KNOMIRA_ISOLATED_TEST_DATA_ROOT=1)
target_include_directories(knomira_lifecycle_engine PRIVATE ../../3rdparty/glad/include)
target_link_libraries(knomira_lifecycle_engine PUBLIC
    Qt6::Core Qt6::Gui Qt6::Xml Qt6::Widgets Qt6::Sql
    PRIVATE OpenAL::OpenAL fastnoise exprtk)
if(MSVC)
    target_compile_options(knomira_lifecycle_engine PRIVATE /bigobj)
endif()

add_executable(knomira_engine_lifecycle_test engine_lifecycle_test.cpp)
target_link_libraries(knomira_engine_lifecycle_test PRIVATE knomira_lifecycle_engine)
set_target_properties(knomira_engine_lifecycle_test PROPERTIES CXX_STANDARD 23)
add_custom_command(TARGET knomira_engine_lifecycle_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:knomira_engine_lifecycle_test>/content/db"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/../../content/db/ingnomia.db.sql"
        "$<TARGET_FILE_DIR:knomira_engine_lifecycle_test>/content/db/ingnomia.db.sql"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/../../content/JSON"
        "$<TARGET_FILE_DIR:knomira_engine_lifecycle_test>/content/JSON"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/../../content/ai"
        "$<TARGET_FILE_DIR:knomira_engine_lifecycle_test>/content/ai"
    VERBATIM)
