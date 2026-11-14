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
#include <QCoreApplication>
#include <QDir>
#include <QFile>

/** @brief Returns the platform-specific user data folder for Ingnomia.
 *  In a test-only executable built with KNOMIRA_ISOLATED_TEST_DATA_ROOT,
 *  rejects an unmarked or unsafe temporary root rather than touching saves.
 *  Production retains its original Windows/Linux path.
 */
QString IO::getDataFolder()
{
    return Knomira::DataPaths::userDataFolder();
}

/** @brief Creates all required subdirectories (mods, save, screenshots, settings, tmp) in the
 *         user data folder, and copies default config files from the application content directory
 *         if they do not already exist.
 *  @return True if the save folder exists after creation.
 */
bool IO::createFolders()
{
	QString folder = getDataFolder() + "/";
	if ( !QDir( folder ).exists() )
	{
		QDir().mkpath( folder );
	}
	QString modFolder = folder + "mods/";
	if ( !QDir( modFolder ).exists() )
	{
		QDir().mkdir( modFolder );
	}
	QString saveFolder = folder + "save/";
	if ( !QDir( saveFolder ).exists() )
	{
		QDir().mkdir( saveFolder );
	}
	QString ssFolder = folder + "screenshots/";
	if ( !QDir( ssFolder ).exists() )
	{
		QDir().mkdir( ssFolder );
	}
	QString settingsFolder = folder + "settings/";
	if ( !QDir( settingsFolder ).exists() )
	{
		QDir().mkdir( settingsFolder );
	}
	QString tmpFolder = folder + "tmp/";
	if ( !QDir( tmpFolder ).exists() )
	{
		QDir().mkdir( tmpFolder );
	}

	QString exePath = QCoreApplication::applicationDirPath();

	if ( !QFile::exists( folder + "settings/profs.json" ) && QFile::exists( exePath + "/content/JSON/profs.json" ) )
	{
		QFile::copy( exePath + "/content/JSON/profs.json", folder + "settings/profs.json" );
	}
	if ( !QFile::exists( folder + "settings/config.json" ) && QFile::exists( exePath + "/content/JSON/config.json" ) )
	{
		QFile::copy( exePath + "/content/JSON/config.json", folder + "settings/config.json" );
	}
	if ( !QFile::exists( folder + "settings/newgame.json" ) && QFile::exists( exePath + "/content/JSON/newgame.json" ) )
	{
		QFile::copy( exePath + "/content/JSON/newgame.json", folder + "settings/newgame.json" );
	}
	/*
	if ( !QFile::exists( folder + "settings/keybindings.json" ) && QFile::exists( exePath + "/content/JSON/keybindings.json" ) )
	{
		QFile::copy( exePath + "/content/JSON/keybindings.json", folder + "settings/keybindings.json" );
	}
	*/

	folder = getDataFolder() + "/save/";

	return QDir( folder ).exists();
}

