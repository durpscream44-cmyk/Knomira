/*
    This file is part of Ingnomia https://github.com/rschurade/Ingnomia
    Copyright (C) 2017-2020 Ralph Schurade, Ingnomia Team

    Licensed under the GNU Affero General Public License v3 or later.
*/
// Keep configuration mutations separate from startup construction so the
// original Config constructor can be tested without linking world save code.
#include "config.h"
#include "io.h"
#include <QDebug>
#include <QMutexLocker>

/**
 * @brief Sets a configuration value and persists to disk.
 *
 * Only writes if the value actually changed. Logs the change to debug output.
 * @param key The setting name to set.
 * @param value The new value.
 */
void Config::set( QString key, QVariant value )
{
	QMutexLocker lock( &m_mutex );
	const auto oldValue = m_settings[key];
	if (oldValue != value)
	{
		m_settings[key] = value;
		IO::saveConfig();

		qDebug() << "Update config" << key << "=" << value << "(was" << oldValue << ")";
	}
}
