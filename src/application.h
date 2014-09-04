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

#ifndef APPLICATION_H
#define APPLICATION_H

class TNetworkClient;

class BNetworkOperation;

class QString;
class QStringList;

#include <TCoreApplication>

#include <BApplicationBase>
#include <BTextTools>

#include <QObject>

#if defined(bApp)
#   undef bApp
#endif
#define bApp static_cast<Application *>(BApplicationBase::binstance())

/*============================================================================
================================ Application =================================
============================================================================*/

class Application : public TCoreApplication
{
    Q_OBJECT
private:
    TNetworkClient *mclient;
public:
    explicit Application(int &argc, char **argv, const QString &applicationName, const QString &organizationName);
    ~Application();
public:
    void updateLoggingMode();
private:
    static bool checkParsingError(BTextTools::OptionsParsingError error, const QString &errorData);
    static bool handleConnectCommand(const QString &cmd, const QStringList &args);
    static bool handleDisconnectCommand(const QString &cmd, const QStringList &args);
    static bool handleLogRequest(BNetworkOperation *op);
    static bool handleServerStateCommand(const QString &cmd, const QStringList &args);
    static bool handleSetAppVersionCommand(const QString &cmd, const QStringList &args);
    static bool handleStartCommand(const QString &cmd, const QStringList &args);
    static bool handleStopCommand(const QString &cmd, const QStringList &args);
    static bool handleUnknownCommand(const QString &command, const QStringList &args);
    static bool handleUserInfoCommand(const QString &cmd, const QStringList &args);
    static QString msecsToString(qint64 msecs);
private:
    void initTerminal();
};

#endif // APPLICATION_H
