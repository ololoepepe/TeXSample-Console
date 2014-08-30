/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of TeXSample Console.
**
** TeXSample Console is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** TeXSample Console is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with TeXSample Console.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

class BSettingsNode;

#include <QString>
#include <QVariant>

namespace Settings
{

namespace Log
{

const QString RootPath = "Log";
const QString LoggingModeSubpath = "mode";
const QString LogNoopSubpath = "noop";

int loggingMode();
int logNoop();
void setLoggingMode(int mode);
bool setLoggingMode(const BSettingsNode *n, const QVariant &v = QVariant());
void setLogNoop(int mode);

}

}

#endif // SETTINGS_H
