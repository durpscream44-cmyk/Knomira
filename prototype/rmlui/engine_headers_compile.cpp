// Phase 4 build gate: critical engine command/lifecycle headers must parse
// independently of the proprietary Noesis SDK and without a precompiled header.
#include "../../src/base/config.h"
#include "../../src/base/global.h"
#include "../../src/game/newgamesettings.h"
#include "../../src/game/gamemanager.h"
#include "../../src/gui/eventconnector.h"

static_assert(sizeof(GameManager*) == sizeof(void*), "GameManager must be visible");
static_assert(sizeof(EventConnector*) == sizeof(void*), "EventConnector must be visible");
static_assert(sizeof(NewGameSettings*) == sizeof(void*), "NewGameSettings must be visible");
