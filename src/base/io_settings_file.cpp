/*
    This file is part of Ingnomia https://github.com/rschurade/Ingnomia
    Copyright (C) 2017-2020 Ralph Schurade, Ingnomia Team

    Licensed under the GNU Affero General Public License v3 or later.
*/
// Original settings-file and fallback loaders split from monolithic IO.cpp
// so a linked Config startup smoke can exercise real IO without loading the
// world, save serialization, or player data.
#include "io.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>

/** @brief Loads the original (shipped) config.json from the application content directory.
 *  @param jd Output parameter receiving the parsed JSON document.
 *  @return True if the file was loaded and parsed successfully.
 */
bool IO::loadOriginalConfig( QJsonDocument& jd )
{
	qDebug() << "load standard config";
	QString exePath = QCoreApplication::applicationDirPath();
	return IO::loadFile( exePath + "/content/JSON/config.json", jd );
}

/** @brief Loads and parses a JSON file from disk.
 *  @param url File path to read from.
 *  @param ja Output parameter receiving the parsed JSON document.
 *  @return True if parsing succeeded, false on JSON parse error (logged to debug output).
 */
bool IO::loadFile( QString url, QJsonDocument& ja )
{
	QFile file( url );
	file.open( QIODevice::ReadOnly | QIODevice::Text );
	QString val = file.readAll();
	file.close();

	QJsonParseError error;
	ja = QJsonDocument::fromJson( val.toUtf8(), &error );

	if ( error.error == QJsonParseError::NoError )
	{
		return true;
	}

	qDebug() << "json parse error in" << url << error.offset;

	return false;
}
