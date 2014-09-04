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

class BSettingsNode;

#include "settings.h"

#include "application.h"

#include <TeXSample>

#include <BTerminal>

#include <QByteArray>
#include <QDebug>
#include <QSettings>
#include <QString>
#include <QVariant>

B_DECLARE_TRANSLATE_FUNCTION

namespace Settings
{

namespace Log
{

static const QString LoggingModePath = RootPath + "/" + LoggingModeSubpath;
static const QString LogNoopPath = RootPath + "/" + LogNoopSubpath;

int loggingMode()
{
    return bSettings->value(LoggingModePath, 2).toInt();
}

int logNoop()
{
    return bSettings->value(LogNoopPath, 0).toInt();
}

void setLoggingMode(int mode)
{
    bSettings->setValue(LoggingModePath, mode);
}

bool setLoggingMode(const BSettingsNode *, const QVariant &v)
{
    QString s = !v.isNull() ? v.toString() : bReadLine(translate("Settings::Log", "Enter logging mode:") + " ");
    if (s.isEmpty())
        return false;
    bool ok = false;
    int m = s.toInt(&ok);
    if (!ok)
        return false;
    setLoggingMode(m);
    bApp->updateLoggingMode();
    return true;
}

void setLogNoop(int mode)
{
    bSettings->setValue(LogNoopPath, mode);
}

}

namespace Texsample
{

static const QString ConnectOnStartupModePath = RootPath + "/" + ConnectOnStartupModeSubpath;
static const QString LoginPath = RootPath + "/" + LoginSubpath;
static const QString PasswordPath = RootPath + "/" + PasswordSubpath;
static const QString StorePasswordPath = RootPath + "/" + StorePasswordSubpath;

QByteArray mpassword;

int connectOnStartupMode()
{
    return bSettings->value(ConnectOnStartupModePath, 2).toInt();
}

QString login()
{
    return bSettings->value(LoginPath).toString();
}

QByteArray password()
{
    return storePassword() ? bSettings->value(PasswordPath).toByteArray() : mpassword;
}

void setConnectOnStartupMode(int mode)
{
    bSettings->setValue(ConnectOnStartupModePath, mode);
}

void setLogin(const QString &login)
{
    bSettings->setValue(LoginPath, login);
}

void setPassword(const QByteArray &password)
{
    if (storePassword())
        bSettings->setValue(PasswordPath, password);
    else
        mpassword = password;
}

bool setPassword(const BSettingsNode *, const QVariant &v)
{
    QString pwd = !v.isNull() ? v.toString() :
                                bReadLineSecure(translate("Settings::Texsample", "Enter TeXSample password:") + " ");
    if (!pwd.isEmpty())
        setPassword(::Texsample::encryptPassword(pwd));
    return !pwd.isEmpty();
}

void setStorePassword(bool store)
{
    bool bstore = storePassword();
    bSettings->setValue(StorePasswordPath, store);
    if (bstore != store) {
        if (store) {
            bSettings->setValue(PasswordPath, mpassword);
        } else {
            mpassword = bSettings->value(PasswordPath).toByteArray();
            bSettings->remove(PasswordPath);
        }
    }
}

bool setStorePassword(const BSettingsNode *, const QVariant &v)
{
    bool store = false;
    if (!v.isNull())
        store = v.toBool();
    else
        store = (bReadLine(translate("Settings::Texsample", "Enter value for \"store_password\" [true|false]:") + " ")
                 == "true");
    setStorePassword(store);
    return true;
}

bool showPassword(const BSettingsNode *, const QVariant &)
{
    bWriteLine(translate("Settings::Texsample",
                         "Password is stored as a hash (SHA-1). You may see it in the .conf file."));
    return true;
}

bool storePassword()
{
    return bSettings->value(StorePasswordPath, false).toBool();
}

}

}
