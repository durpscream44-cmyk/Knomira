// Copyright (C) 2026 Knomira contributors.
// Only the legacy Noesis GUI calls this converter; the simulation does not.
#pragma once

#include <NsGui/InputEnums.h>
#include <Qt>

namespace NoesisKeyConverter {
Noesis::Key fromQt(Qt::Key key);
}
