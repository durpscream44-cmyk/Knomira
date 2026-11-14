/*
    This file is part of Ingnomia https://github.com/rschurade/Ingnomia
    Copyright (C) 2017-2020 Ralph Schurade, Ingnomia Team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
// Original JSON save overloads separated from world serialization so a
// real new-game settings bootstrap can link and exercise them safely.
#include "io.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

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
