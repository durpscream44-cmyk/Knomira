/*
    This file is part of Ingnomia https://github.com/rschurade/Ingnomia
    Copyright (C) 2017-2020 Ralph Schurade, Ingnomia Team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
// Configuration JSON I/O split out of the larger save/load implementation.
// Source behavior is unchanged; this lets real Config link in isolation.
#include "io.h"
#include "config.h"
#include "global.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QVariantMap>

/** @brief Saves the current global configuration to config.json in the settings folder.
 *  @return True on success.
 */
bool IO::saveConfig()
{
	QString folder = getDataFolder() + "/settings/";

	QVariantMap cm   = Global::cfg->object();
	QJsonDocument jd = QJsonDocument::fromVariant( cm );

	IO::saveFile( folder + "config.json", jd );

	return true;
}

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

/** @brief Saves a QJsonObject to a file by wrapping it in a QJsonDocument.
 *  @param url File path to write to.
 *  @param jo JSON object to save.
 *  @return True on success, false if the file could not be opened.
 */
bool IO::saveFile( QString url, const QJsonObject& jo )
{
	QJsonDocument saveDoc( jo );
	return saveFile( url, saveDoc );
}

/** @brief Saves a QJsonArray to a file by wrapping it in a QJsonDocument.
 *  @param url File path to write to.
 *  @param ja JSON array to save.
 *  @return True on success, false if the file could not be opened.
 */
bool IO::saveFile( QString url, const QJsonArray& ja )
{
	QJsonDocument saveDoc( ja );
	return saveFile( url, saveDoc );
}

/** @brief Saves a QJsonDocument to a file as formatted JSON text.
 *  @param url File path to write to.
 *  @param jd JSON document to save.
 *  @return True on success, false if the file could not be opened.
 */
bool IO::saveFile( QString url, const QJsonDocument& jd )
{
	QFile file( url );

	if ( !file.open( QIODevice::WriteOnly ) )
	{
		qWarning( "Couldn't open save file." );
		return false;
	}

	file.write( jd.toJson() );
	file.close();
	return true;
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
