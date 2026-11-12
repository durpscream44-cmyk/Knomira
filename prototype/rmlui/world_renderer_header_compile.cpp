// Compile-only regression check for the real game world renderer's public API.
// No proprietary Noesis SDK should be required by this header.
#include "../../src/gui/mainwindowrenderer.h"

static_assert(sizeof(MainWindowRenderer*) == sizeof(void*), "World renderer pointer must compile");
