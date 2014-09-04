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

#include "application.h"

#include "settings.h"

#include <TAccessLevel>
#include <TClientInfo>
#include <TCoreApplication>
#include <TeXSample>
#include <TGetServerStateReplyData>
#include <TGetServerStateRequestData>
#include <TGetUserConnectionInfoListReplyData>
#include <TGetUserConnectionInfoListRequestData>
#include <TLogReplyData>
#include <TLogRequestData>
#include <TNetworkClient>
#include <TOperation>
#include <TReply>
#include <TRequest>
#include <TServerState>
#include <TSetLatestAppVersionReplyData>
#include <TSetLatestAppVersionRequestData>
#include <TSetServerStateReplyData>
#include <TSetServerStateRequestData>
#include <TSubscribeReplyData>
#include <TSubscribeRequestData>
#include <TUserConnectionInfo>
#include <TUserConnectionInfoList>
#include <TUserInfo>

#include <BeQt>
#include <BLocationProvider>
#include <BLogger>
#include <BNetworkOperation>
#include <BSettingsNode>
#include <BTerminal>
#include <BTranslation>
#include <BUuid>

#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QMap>
#include <QObject>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <BTextTools>
#include <QUrl>
#include <QVariant>

/*============================================================================
================================ Application =================================
============================================================================*/

/*============================== Public constructors =======================*/

Application::Application(int &argc, char **argv, const QString &applicationName, const QString &organizationName) :
    TCoreApplication(argc, argv, applicationName, organizationName)
{
#if defined(BUILTIN_RESOURCES)
    Q_INIT_RESOURCE(texsample_console);
    Q_INIT_RESOURCE(texsample_console_translations);
#endif
    setApplicationVersion("2.0.0-beta");
    setOrganizationDomain("https://github.com/ololoepepe/TeXSample-Console");
    setApplicationCopyrightPeriod("2013-2014");
    BLocationProvider *prov = new BLocationProvider;
    prov->addLocation("logs");
    installLocationProvider(prov);
    installBeqtTranslator("qt");
    installBeqtTranslator("beqt");
    installBeqtTranslator("texsample");
    installBeqtTranslator("texsample-console");
    initTerminal();
    BTerminal::setTitle(Application::applicationName() + " v" + applicationVersion());
    bWriteLine(tr("This is") + " " + Application::applicationName() + " v" + applicationVersion());
    logger()->setDateTimeFormat("yyyy.MM.dd hh:mm:ss");
    updateLoggingMode();
    QString logfn = location(DataPath, UserResource) + "/logs/";
    logfn += QDateTime::currentDateTime().toString("yyyy.MM.dd-hh.mm.ss") + ".txt";
    logger()->setFileName(logfn);
    mclient = new TNetworkClient;
    mclient->installRequestHandler(TOperation::Log, &handleLogRequest);
    mclient->setPingInterval(5 * BeQt::Minute);
    bWriteLine(tr("Enter \"help --commands\" to see the list of available commands"));
    int c = Settings::Texsample::connectOnStartupMode();
    if (1 == c) {
        handleConnectCommand(QString(), QStringList());
    } else if (2 == c) {
        QString s = bReadLine(tr("Do you want to connect automatically? [Yes|no]:") + " ");
        if (s.isEmpty() || !s.compare("y", Qt::CaseInsensitive) || !s.compare("ye", Qt::CaseInsensitive)
                || !s.compare("yes", Qt::CaseInsensitive)) {
            handleConnectCommand(QString(), QStringList());
        }
    }
}

Application::~Application()
{
    delete mclient;
#if defined(BUILTIN_RESOURCES)
    Q_CLEANUP_RESOURCE(texsample_console);
    Q_CLEANUP_RESOURCE(texsample_console_translations);
#endif
}

/*============================== Public methods ============================*/

void Application::updateLoggingMode()
{
    int m = Settings::Log::loggingMode();
    if (m <= 0) {
        bLogger->setLogToConsoleEnabled(false);
        bLogger->setLogToFileEnabled(false);
    } else if (1 == m) {
        bLogger->setLogToConsoleEnabled(true);
        bLogger->setLogToFileEnabled(false);
    } else if (2 == m) {
        bLogger->setLogToConsoleEnabled(false);
        bLogger->setLogToFileEnabled(true);
    } else if (m >= 3) {
        bLogger->setLogToConsoleEnabled(true);
        bLogger->setLogToFileEnabled(true);
    }
}

/*============================== Static private methods ====================*/

bool Application::checkParsingError(BTextTools::OptionsParsingError error, const QString &errorData)
{
    switch (error) {
    case BTextTools::InvalidParametersError:
        bWriteLine(tr("Internal parsing error", "error"));
        return false;
    case BTextTools::MalformedOptionError:
        bWriteLine(tr("Malformed option:", "error") + " " + errorData);
        return false;
    case BTextTools::MissingOptionError:
        bWriteLine(tr("Missing option:", "error") + " " + errorData);
        return false;
    case BTextTools::RepeatingOptionError:
        bWriteLine(tr("Repeating option:", "error") + " " + errorData);
        return false;
    case BTextTools::UnknownOptionError:
        bWriteLine(tr("Unknown option:", "error") + " " + errorData);
        return false;
    case BTextTools::UnknownOptionValueError:
        bWriteLine(tr("Unknown option value:", "error") + " " + errorData);
        return false;
    case BTextTools::NoError:
    default:
        return true;
    }
}

bool Application::handleConnectCommand(const QString &, const QStringList &args)
{
    if (args.size() > 1) {
        bWriteLine(tr("Invalid argument count. This command accepts 0-1 arguments", "error"));
        return false;
    }
    if (!bApp) {
        bWriteLine(tr("No Application instance", "error"));
        return false;
    }
    TNetworkClient *client = bApp->mclient;
    switch (client->state()) {
    case TNetworkClient::AuthorizedState:
    case TNetworkClient::ConnectedState: {
        bWriteLine(tr("The client is already connected to", "error") + " " + client->hostName());
        return false;
    }
    case TNetworkClient::ConnectingState: {
        bWriteLine(tr("The client is already connecting to", "error") + " " + client->hostName());
        return false;
    }
    case TNetworkClient::DisconnectedState: {
        QString hostName = !args.isEmpty() ? args.first() : QString();
        if (hostName.isEmpty())
            hostName = "texsample-server.no-ip.org";
        QString login = Settings::Texsample::login();
        QByteArray password = Settings::Texsample::password();
        if (!login.isEmpty() || !password.isEmpty()) {
            QString s = bReadLine(tr("Use stored login/password? [Yes|no]:") + " ");
            if (!s.isEmpty() && s.compare("y", Qt::CaseInsensitive) && s.compare("ye", Qt::CaseInsensitive)
                    && s.compare("yes", Qt::CaseInsensitive)) {
                login.clear();
                password.clear();
            }
        }
        QString error;
        if (login.isEmpty()) {
            login = bReadLine(tr("Enter login/e-mail:") + " ");
            if (!Texsample::testEmail(login, &error) && !Texsample::testLogin(login, &error)) {
                bWriteLine(error);
                return false;
            }
            Settings::Texsample::setLogin(login);
        }
        if (password.isEmpty()) {
            QString pwd = bReadLineSecure(tr("Enter password:") + " ");
            if (!Texsample::testPassword(pwd, &error)) {
                bWriteLine(error);
                return false;
            }
            password = Texsample::encryptPassword(pwd);
            Settings::Texsample::setPassword(password);
        }
        client->setHostName(hostName);
        client->setLogin(login);
        client->setPassword(password);
        bWriteLine(tr("Connecting to", "") + " " + hostName + "...");
        client->connectToServer();
        if (!client->isAuthorized())
            BeQt::waitNonBlocking(client, SIGNAL(authorizedChanged(bool)), 30 * BeQt::Second);
        if (!client->isAuthorized()) {
            bWriteLine(tr("Failed to connect to") + " " + hostName);
            client->disconnectFromServer();
            return false;
        }
        bWriteLine(tr("Connected to") + " " + hostName);
        if (client->userInfo().accessLevel().level() >= TAccessLevel::ModeratorLevel) {
            bWriteLine(tr("Subscribing..."));
            TSubscribeRequestData requestData;
            requestData.setSubscribedToLog(true);
            TReply reply = client->performOperation(TOperation::Subscribe, requestData);
            if (!reply.success())
                bWriteLine(tr("Failed to subscribe duse to the following error:") + "\n" + reply.message());
            bWriteLine(tr("Subscribed"));
        }
        return true;
    }
    case TNetworkClient::DisconnectingState: {
        bWriteLine(tr("The client is disconnecting from", "error") + " " + client->hostName());
        return false;
    }
    default: {
        return false;
    }
    }
}

bool Application::handleDisconnectCommand(const QString &, const QStringList &args)
{
    if (!args.isEmpty()) {
        bWriteLine(tr("Invalid argument count. This command does not accept any arguments", "error"));
        return false;
    }
    if (!bApp) {
        bWriteLine(tr("No Application instance", "error"));
        return false;
    }
    TNetworkClient *client = bApp->mclient;
    switch (client->state()) {
    case TNetworkClient::AuthorizedState:
    case TNetworkClient::ConnectedState:
    case TNetworkClient::ConnectingState:
        client->disconnectFromServer();
        bWriteLine(tr("OK", "message"));
        return true;
    case TNetworkClient::DisconnectedState:
        bWriteLine(tr("The client is not connected", "error"));
        return false;
    case TNetworkClient::DisconnectingState:
        bWriteLine(tr("The client is already disconnecting", "error"));
        return false;
    default:
        return false;
    }
}

bool Application::handleLogRequest(BNetworkOperation *op)
{
    TRequest request = op->variantData().value<TRequest>();
    TLogRequestData requestData = request.data().value<TLogRequestData>();
    bLog(requestData.text(), requestData.level());
    TReply reply;
    TLogReplyData replyData;
    reply.setData(replyData);
    reply.setSuccess(true);
    return op->reply(QVariant::fromValue(reply));
}

bool Application::handleServerStateCommand(const QString &, const QStringList &args)
{
    if (!args.isEmpty()) {
        bWriteLine(tr("Invalid argument count. This command does not accept any arguments", "error"));
        return false;
    }
    if (!bApp) {
        bWriteLine(tr("No Application instance", "error"));
        return false;
    }
    TNetworkClient *client = bApp->mclient;
    if (!client->isAuthorized()) {
        bWriteLine(tr("Not authorized", "error"));
        return false;
    }
    TGetServerStateRequestData requestData;
    TReply reply = client->performOperation(TOperation::GetServerState, requestData);
    if (!reply.success()) {
        bWriteLine(tr("Failed to get server state due to the following error:", "error") + "\n" + reply.message());
        return false;
    }
    TGetServerStateReplyData replyData = reply.data().value<TGetServerStateReplyData>();
    TServerState state = replyData.serverState();
    bWriteLine(state.listening() ? tr("The server is listening") : tr("The server is not listening"));
    bWriteLine(tr("Server uptime:") + " " + msecsToString(state.uptime()));
    return true;
}

bool Application::handleSetAppVersionCommand(const QString &, const QStringList &args)
{
    typedef QMap<QString, Texsample::ClientType> ClientMap;
    init_once(ClientMap, clientMap, ClientMap()) {
        clientMap.insert("cloudlab-client", Texsample::CloudlabClient);
        clientMap.insert("clab", Texsample::CloudlabClient);
        clientMap.insert("tex-creator", Texsample::TexCreator);
        clientMap.insert("tcrt", Texsample::TexCreator);
        clientMap.insert("texsample-console", Texsample::TexsampleConsole);
        clientMap.insert("tcsl", Texsample::TexsampleConsole);
    }
    typedef QMap<QString, BeQt::OSType> OsMap;
    init_once(OsMap, osMap, OsMap()) {
        osMap.insert("linux", BeQt::LinuxOS);
        osMap.insert("lin", BeQt::LinuxOS);
        osMap.insert("l", BeQt::LinuxOS);
        osMap.insert("macos", BeQt::MacOS);
        osMap.insert("mac", BeQt::MacOS);
        osMap.insert("m", BeQt::MacOS);
        osMap.insert("windows", BeQt::WindowsOS);
        osMap.insert("win", BeQt::WindowsOS);
        osMap.insert("w", BeQt::WindowsOS);
    }
    typedef QMap<QString, BeQt::ProcessorArchitecture> ArchMap;
    init_once(ArchMap, archMap, ArchMap()) {
        archMap.insert("alpha", BeQt::AlphaArchitecture);
        archMap.insert("amd64", BeQt::Amd64Architecture);
        archMap.insert("arm", BeQt::ArmArchitecture);
        archMap.insert("arm64", BeQt::Arm64Architecture);
        archMap.insert("blackfin", BeQt::BlackfinArchitecture);
        archMap.insert("convex", BeQt::ConvexArchitecture);
        archMap.insert("epiphany", BeQt::EpiphanyArchitecture);
        archMap.insert("risc", BeQt::HpPaRiscArchitecture);
        archMap.insert("x86", BeQt::IntelX86Architecture);
        archMap.insert("itanium", BeQt::IntelItaniumArchitecture);
        archMap.insert("motorola", BeQt::Motorola68kAArchitecture);
        archMap.insert("mips", BeQt::MipsArchitecture);
        archMap.insert("powerpc", BeQt::PowerPcArchitecture);
        archMap.insert("pyramid", BeQt::Pyramid9810Architecture);
        archMap.insert("rs6000", BeQt::Rs6000Architecture);
        archMap.insert("sparc", BeQt::SparcArchitecture);
        archMap.insert("superh", BeQt::SuperHArchitecture);
        archMap.insert("systemz", BeQt::SystemZArchitecture);
        archMap.insert("tms320", BeQt::Tms320Architecture);
        archMap.insert("tms470", BeQt::Tms470Architecture);
    }
    QMap<QString, QString> result;
    QString errorData;
    QString options = "client:-c|--client=" + QStringList(clientMap.keys()).join("|") + ",";
    options += "os:-o|--os=" + QStringList(osMap.keys()).join("|") + ",";
    options += "arch:-a|--arch=" + QStringList(archMap.keys()).join("|") + ",";
    options += "[portable:-p|--portable],version:-v|--version=,[url:-u|--url=]";
    BTextTools::OptionsParsingError error = BTextTools::parseOptions(args, options, result, errorData);
    if (!checkParsingError(error, errorData)) {
        return false;
    }
    Texsample::ClientType clientType = clientMap.value(result.value("client"));
    BeQt::OSType os = osMap.value(result.value("os"));
    BeQt::ProcessorArchitecture arch = archMap.value(result.value("arch"));
    BVersion version = BVersion(result.value("version"));
    QUrl url = QUrl::fromUserInput(result.value("url"));
    bool portable = result.contains("portable");
    if (!version.isValid()) {
        bWriteLine(tr("Invalid version", "error"));
        return false;
    }
    if (!result.value("url").isEmpty() && !url.isValid()) {
        bWriteLine(tr("Invalid url", "error"));
        return false;
    }
    if (!bApp) {
        bWriteLine(tr("No Application instance", "error"));
        return false;
    }
    TNetworkClient *client = bApp->mclient;
    if (!client->isAuthorized()) {
        bWriteLine(tr("Not authorized", "error"));
        return false;
    }
    TSetLatestAppVersionRequestData requestData;
    requestData.setClientType(clientType);
    requestData.setDownloadUrl(url);
    requestData.setOs(os);
    requestData.setPortable(portable);
    requestData.setProcessorArchitecture(arch);
    requestData.setVersion(version);
    TReply reply = client->performOperation(TOperation::SetLatestAppVersion, requestData);
    if (!reply.success()) {
        bWriteLine(tr("Failed to set app version due to the following error:", "error") + "\n" + reply.message());
        return false;
    }
    bWriteLine(tr("OK", "message"));
    return true;
}

bool Application::handleStartCommand(const QString &, const QStringList &args)
{
    if (args.size() > 1) {
        bWriteLine(tr("Invalid argument count. This command does not accept any arguments", "error"));
        return false;
    }
    if (!bApp) {
        bWriteLine(tr("No Application instance", "error"));
        return false;
    }
    TNetworkClient *client = bApp->mclient;
    if (!client->isAuthorized()) {
        bWriteLine(tr("Not authorized", "error"));
        return false;
    }
    TSetServerStateRequestData requestData;
    requestData.setListening(true);
    requestData.setAddress(!args.isEmpty() ? args.first() : QString());
    TReply reply = client->performOperation(TOperation::SetServerState, requestData);
    if (!reply.success()) {
        bWriteLine(tr("Failed to start server due to the following error:", "error") + "\n" + reply.message());
        return false;
    }
    bWriteLine(tr("OK", "message"));
    return true;
}

bool Application::handleStopCommand(const QString &, const QStringList &args)
{
    if (!args.isEmpty()) {
        bWriteLine(tr("Invalid argument count. This command does not accept any arguments", "error"));
        return false;
    }
    if (!bApp) {
        bWriteLine(tr("No Application instance", "error"));
        return false;
    }
    TNetworkClient *client = bApp->mclient;
    if (!client->isAuthorized()) {
        bWriteLine(tr("Not authorized", "error"));
        return false;
    }
    TSetServerStateRequestData requestData;
    requestData.setListening(false);
    TReply reply = client->performOperation(TOperation::SetServerState, requestData);
    if (!reply.success()) {
        bWriteLine(tr("Failed to stop server due to the following error:", "error") + "\n" + reply.message());
        return false;
    }
    bWriteLine(tr("OK", "message"));
    return true;
}

bool Application::handleUnknownCommand(const QString &, const QStringList &)
{
    bWriteLine(tr("Unknown command. Enter \"help --commands\" to see the list of available commands"));
    return false;
}

bool Application::handleUserInfoCommand(const QString &, const QStringList &args)
{
    typedef QMap<QString, TGetUserConnectionInfoListRequestData::MatchType> IntMap;
    init_once(IntMap, matchTypeMap, IntMap()) {
        matchTypeMap.insert("login-and-unique-id", TGetUserConnectionInfoListRequestData::MatchLoginAndUniqueId);
        matchTypeMap.insert("a", TGetUserConnectionInfoListRequestData::MatchLoginAndUniqueId);
        matchTypeMap.insert("login", TGetUserConnectionInfoListRequestData::MatchLogin);
        matchTypeMap.insert("l", TGetUserConnectionInfoListRequestData::MatchLogin);
        matchTypeMap.insert("unique-id", TGetUserConnectionInfoListRequestData::MatchUniqueId);
        matchTypeMap.insert("u", TGetUserConnectionInfoListRequestData::MatchUniqueId);
    }
    QMap<QString, QString> result;
    QString errorData;
    QString options = "[type:--match-type|-m=login-and-unique-id|a|login|l|unique-id|u],pattern:--match-pattern|-p=";
    BTextTools::OptionsParsingError error = BTextTools::parseOptions(args, options, result, errorData);
    if (!checkParsingError(error, errorData))
        return false;
    TGetUserConnectionInfoListRequestData::MatchType matchType =
            result.contains("type") ? matchTypeMap.value(result.value("type")) :
                                      TGetUserConnectionInfoListRequestData::MatchLoginAndUniqueId;
    QString matchPattern = result.value("pattern");
    if (!QRegExp(matchPattern, Qt::CaseSensitive, QRegExp::WildcardUnix).isValid()) {
        bWriteLine(tr("Invalid pattern", "error"));
        return false;
    }
    if (!bApp) {
        bWriteLine(tr("No Application instance", "error"));
        return false;
    }
    TNetworkClient *client = bApp->mclient;
    if (!client->isAuthorized()) {
        bWriteLine(tr("Not authorized", "error"));
        return false;
    }
    TGetUserConnectionInfoListRequestData requestData;
    requestData.setMatchPattern(matchPattern);
    requestData.setMatchType(matchType);
    TReply reply = client->performOperation(TOperation::GetUserConnectionInfoList, requestData);
    if (!reply.success()) {
        bWriteLine(tr("Failed to get user info list due to the following error:", "error") + "\n" + reply.message());
        return false;
    }
    TGetUserConnectionInfoListReplyData replyData = reply.data().value<TGetUserConnectionInfoListReplyData>();
    TUserConnectionInfoList list = replyData.connectionInfoList();
    if (list.isEmpty()) {
        bWriteLine(tr("No user matched", "message"));
        return true;
    }
    bWriteLine(tr("Matched users:", "message") + " " + QString::number(list.size()));
    foreach (const TUserConnectionInfo &info, list) {
        QString s = "\n";
        s += "[" + info.userInfo().login() + "] [" + info.peerAddress() + "] [" + info.uniqueId().toString(true) + "]";
        s += "\n" + info.userInfo().accessLevel().toString() + "; " + info.clientInfo().os() + "\n";
        s += info.clientInfo().toString("%n v%v; TeXSample v%t; BeQt v%b; Qt v%q") + "\n";
        s += tr("Connected at:", "message") + " "
                + info.connectionDateTime().toLocalTime().toString("yyyy.MM.dd hh:mm:ss") + "\n";
        s += tr("Uptime:", "message") + " " + msecsToString(info.uptime());
        bWriteLine(s);
    }
    return true;
}

QString Application::msecsToString(qint64 msecs)
{
    QString days = QString::number(msecs / (24 * BeQt::Hour));
    msecs %= (24 * BeQt::Hour);
    QString hours = QString::number(msecs / BeQt::Hour);
    hours.prepend(QString().fill('0', 2 - hours.length()));
    msecs %= BeQt::Hour;
    QString minutes = QString::number(msecs / BeQt::Minute);
    minutes.prepend(QString().fill('0', 2 - minutes.length()));
    msecs %= BeQt::Minute;
    QString seconds = QString::number(msecs / BeQt::Second);
    seconds.prepend(QString().fill('0', 2 - seconds.length()));
    return days + " " + tr("days") + " " + hours + ":" + minutes + ":" + seconds;
}

/*============================== Private methods ===========================*/

void Application::initTerminal()
{
    BTerminal::setMode(BTerminal::StandardMode);
    BTerminal::installHandler(BTerminal::QuitCommand);
    BTerminal::installHandler(BTerminal::SetCommand);
    BTerminal::installHandler(BTerminal::HelpCommand);
    BTerminal::installHandler(BTerminal::LastCommand);
    BTerminal::installDefaultHandler(&handleUnknownCommand);
    BTerminal::installHandler("connect", &handleConnectCommand);
    BTerminal::installHandler("disconnect", &handleDisconnectCommand);
    BTerminal::installHandler("server-state", &handleServerStateCommand);
    BTerminal::installHandler("set-app-version", &handleSetAppVersionCommand);
    BTerminal::installHandler("start", &handleStartCommand);
    BTerminal::installHandler("stop", &handleStopCommand);
    BTerminal::installHandler("user-info", &handleUserInfoCommand);
    BSettingsNode *root = new BSettingsNode;
    BTerminal::createBeQtSettingsNode(root);
    BSettingsNode *n = new BSettingsNode(Settings::Log::RootPath, root);
    BSettingsNode *nn = new BSettingsNode(QVariant::Int, Settings::Log::LoggingModeSubpath, n);
    nn->setUserSetFunction(&Settings::Log::setLoggingMode);
    nn->setDescription(BTranslation::translate("Application", "Logging mode. Possible values:\n"
                                               "  0 or less - don't log\n"
                                               "  1 - log to console only\n"
                                               "  2 - log to file only\n"
                                               "  3 and more - log to console and file\n"
                                               "  The default is 2"));
    nn = new BSettingsNode(QVariant::Int, Settings::Log::LogNoopSubpath, n);
    nn->setDescription(BTranslation::translate("Application", "Logging the \"keep alive\" operations. "
                                               "Possible values:\n"
                                               "  0 or less - don't log\n"
                                               "  1 - log locally\n"
                                               "  2 and more - log loaclly and remotely\n"
                                               "  The default is 0"));
    n = new BSettingsNode(Settings::Texsample::RootPath, root);
    nn = new BSettingsNode(QVariant::Int, Settings::Texsample::ConnectOnStartupModeSubpath, n);
    nn->setDescription(BTranslation::translate("Application", "Connect on startup. "
                                               "Possible values:\n"
                                               "  0 or less - don't connect\n"
                                               "  1 - try to connect\n"
                                               "  2 and more - prompt user\n"
                                               "  The default is 2"));
    nn = new BSettingsNode(Settings::Texsample::LoginSubpath, n);
    nn->setDescription(BTranslation::translate("Application", "Identifier used to log into the TeXSample server."));
    nn = new BSettingsNode(Settings::Texsample::PasswordSubpath, n);
    nn->setUserSetFunction(&Settings::Texsample::setPassword);
    nn->setUserShowFunction(&Settings::Texsample::showPassword);
    nn->setDescription(BTranslation::translate("Application", "Password used to log into the TeXSample server."));
    nn = new BSettingsNode(Settings::Texsample::StorePasswordSubpath, n);
    nn->setUserSetFunction(&Settings::Texsample::setStorePassword);
    nn->setDescription(BTranslation::translate("Application", "Determines wether your password is stored on disk."));
    BTerminal::setRootSettingsNode(root);
    BTerminal::setHelpDescription(BTranslation::translate("Application",
        "This is TeXSample Console. Enter \"help --all\" to see full Help"));
    BTerminal::CommandHelp ch;
    ch.usage = "connect [address]";
    ch.description = BTranslation::translate("Application", "Connect to a remote server. "
                                             "If [address] is not specified, texsample-server.no-ip.org is used.");
    BTerminal::setCommandHelp("connect", ch);
    ch.usage = "disconnect";
    ch.description = BTranslation::translate("Application", "Disconnect from the remote server");
    BTerminal::setCommandHelp("disconnect", ch);
    ch.usage = "server-state";
    ch.description = BTranslation::translate("Application",
                                             "Show information about the server state (uptime and listening state).");
    BTerminal::setCommandHelp("server-state", ch);
    ch.usage = "set-app-version <parameters>";
    ch.description = BTranslation::translate("Application",
        "Set the latest version of an application along with the download URL.\n"
        "The parameters are:\n"
        "  --client|-c=<client>, where <client> must be one of the following:\n"
        "    cloudlab-client|clab\n"
        "    tex-creator|tcrt\n"
        "    texsample-console|tcsl\n"
        "  --os|o=<os>, where <os> must be one of the following:\n"
        "    linux|lin|l\n"
        "    macos|mac|m\n"
        "    windows|win|w\n"
        "  --arch|-a=<arch>, where <arch> must be one of the following:\n"
        "    alpha, amd64, arm, arm64, blackfin, convex, epiphany,\n"
        "    risc, x86, itanium, motorola, mips, powerpc, pyramid,\n"
        "    rs6000, sparc, superh, systemz, tms320, tms470\n"
        "  --version|-v=<version>, where <version> must be in the following format:\n"
        "    <major>[.<minor>[.<patch>]][-<status>[extra]]\n"
        "  --url|-u<url> (optional), where <url> must be a url (schema may be omitted)\n"
        "  --portable|-p (optional)\n"
        "Example:\n"
        "  set-app-version -c=tex-creator -o=windows -a=x86 -p -v=3.5.0-beta2 -u=site.com/dl/install.exe");
    BTerminal::setCommandHelp("set-app-version", ch);
    ch.usage = "start [address]";
    ch.description = BTranslation::translate("Application", "Start the server. "
                                             "If [address] is passed, the server will only listen on that address, "
                                             "otherwise it will listen on all available addresses.");
    BTerminal::setCommandHelp("start", ch);
    ch.usage = "stop";
    ch.description = BTranslation::translate("Application", "Stop the server.\nNote: Users are not disconnected.");
    BTerminal::setCommandHelp("stop", ch);
    ch.usage = "user-info [--match-type|-m=<match_type>] --match-pattern|-p=<match_pattern>";
    ch.description = BTranslation::translate("Application",
        "Show information about connected users matching <match_pattern>, which is to be a wildcard.\n"
        "<match_type> may be one of the following:\n"
        "  login-and-unique-id|a - attempt to match both login and uinque id (default)\n"
        "  login|l - match login only\n"
        "  unique-id|u - match unique id only");
    BTerminal::setCommandHelp("user-info", ch);
}
