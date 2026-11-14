/*
    This file is part of Ingnomia https://github.com/rschurade/Ingnomia
    Copyright (C) 2017-2020 Ralph Schurade, Ingnomia Team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.
*/
// Standalone implementation of the original IO data-folder entry point.
// Keeping it in a small translation unit allows a linked test to call the
// exact engine function without linking the entire save/load simulation.
#include "io.h"
#include "io_data_root.h"

/** @brief Returns the platform-specific user data folder for Ingnomia.
 *  In a test-only executable built with KNOMIRA_ISOLATED_TEST_DATA_ROOT,
 *  rejects an unmarked or unsafe temporary root rather than touching saves.
 *  Production retains its original Windows/Linux path.
 */
QString IO::getDataFolder()
{
    return Knomira::DataPaths::userDataFolder();
}
