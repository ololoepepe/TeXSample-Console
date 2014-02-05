#include "terminaliohandler.h"
#include "global.h"

#include <TeXSample>
#include <TOperationResult>
#include <TClientInfo>
#include <TMessage>
#include <TAccessLevel>

#include <BNetworkConnection>
#include <BGenericSocket>
#include <BeQt>
#include <BNetworkOperation>
#include <BLogger>
#include <BCoreApplication>
#include <BSettingsNode>
#include <BTranslation>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMetaObject>
#include <QVariantMap>
#include <QVariant>
#include <QByteArray>
#include <QCryptographicHash>
#include <QAbstractSocket>
#include <QSettings>
#include <QDateTime>

#include <QDebug>
#include <QThread>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_DECLARE_METATYPE(QUuid)
#endif

static bool handleNoopRequest(BNetworkOperation *op)
{
    if (bSettings->value("Log.noop").toInt() >= 1)
        bLog("Replying to connection test...");
    op->reply();
    return true;
}

static bool handleLogRequest(BNetworkOperation *op)
{
    QVariantMap in = op->variantData().toMap();
    op->reply();
    bLog(in.value("text").toString(), static_cast<BLogger::Level>(in.value("level").toInt()));
    return true;
}

static bool handleMessageRequest(BNetworkOperation *op)
{
    TMessage msg = op->variantData().toMap().value("message").toInt();
    op->reply();
    bWriteLine(msg.messageString());
    return true;
}

/*============================================================================
================================ TerminalIOHandler ===========================
============================================================================*/

/*============================== Public constructors =======================*/

TerminalIOHandler::TerminalIOHandler(QObject *parent) :
    BTerminalIOHandler(parent)
{
    muserId = 0;
    mremote = new BNetworkConnection(BGenericSocket::TcpSocket, this);
    mremote->installRequestHandler(BNetworkConnection::operation(BNetworkConnection::NoopOperation),
                                   &handleNoopRequest);
    mremote->installRequestHandler(Texsample::LogRequest, &handleLogRequest);
    mremote->installRequestHandler(Texsample::MessageRequest, &handleMessageRequest);
    connect(mremote, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(mremote, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    installHandler(QuitCommand);
    installHandler(SetCommand);
    installHandler(HelpCommand);
    installHandler("connect", (InternalHandler) &TerminalIOHandler::handleConnect);
    installHandler("disconnect", (InternalHandler) &TerminalIOHandler::handleDisconnect);
    installHandler("uptime", (InternalHandler) &TerminalIOHandler::handleUptime);
    installHandler("user", (InternalHandler) &TerminalIOHandler::handleUser);
    installHandler("set-app-version", (InternalHandler) &TerminalIOHandler::handleSetAppVersion);
    installHandler("start", (InternalHandler) &TerminalIOHandler::handleStart);
    installHandler("stop", (InternalHandler) &TerminalIOHandler::handleStop);
    BSettingsNode *root = new BSettingsNode;
      BSettingsNode *n = new BSettingsNode("Log", root);
        BSettingsNode *nn = new BSettingsNode("mode", n);
          nn->setUserSetFunction(&Global::setLoggingMode);
          nn->setDescription(BTranslation::translate("BSettingsNode", "Logging mode. Possible values:\n"
                                                     "0 or less - don't log\n"
                                                     "1 - log to console only\n"
                                                     "2 - log to file only\n"
                                                     "3 and more - log to console and file\n"
                                                     "The default is 2"));
        nn = new BSettingsNode("noop", n);
          nn->setDescription(BTranslation::translate("BSettingsNode", "Logging the \"keep alive\" operations. "
                                                     "Possible values:\n"
                                                     "0 or less - don't log\n"
                                                     "1 - log\n"
                                                     "The default is 0"));
    setRootSettingsNode(root);
    setHelpDescription(BTranslation::translate("BTerminalIOHandler", "This is TeXSample Console. "
                                               "Enter \"help --all\" to see full Help"));
    CommandHelpList chl;
    CommandHelp ch;
    ch.usage = "connect [address]";
    ch.description = BTranslation::translate("BTerminalIOHandler", "Connect to a remote server. "
                                             "If address is not specified, texsample-server.no-ip.org is used");
    setCommandHelp("connect", ch);
    ch.usage = "disconnect";
    ch.description = BTranslation::translate("BTerminalIOHandler", "Disconnect from the remote server");
    setCommandHelp("connect", ch);
    ch.usage = "uptime";
    ch.description = BTranslation::translate("BTerminalIOHandler",
                                             "Show for how long the server has been running");
    setCommandHelp("uptime", ch);
    ch.usage = "user [--list]";
    ch.description = BTranslation::translate("BTerminalIOHandler", "Show connected user count or list them all");
    chl << ch;
    ch.usage = "user --connected-at|--info|--uptime <id|login>";
    ch.description = BTranslation::translate("BTerminalIOHandler", "Show information about the user. "
                                             "The user may be specified by id or by login. Options:\n"
                                             "  --connected-at - time when the user connected\n"
                                             "  --info - detailed user information\n"
                                             "  --uptime - for how long the user has been connected");
    chl << ch;
    ch.usage = "user --kick <id|login>";
    ch.description = BTranslation::translate("BTerminalIOHandler", "Disconnect the specified user. "
                                             "If login is specified, all connections of this user will be closed");
    chl << ch;
    setCommandHelp("user", chl);
    ch.usage = "start [address]";
    ch.description = BTranslation::translate("BTerminalIOHandler", "Start the server. "
                                             "If address is specified, the server will only listen on that address, "
                                             "otherwise it will listen on available all addresses.");
    setCommandHelp("start", ch);
    ch.usage = "stop";
    ch.description = BTranslation::translate("BTerminalIOHandler", "Stop the server. Users are NOT disconnected");
    setCommandHelp("stop", ch);
    ch.usage = "set-app-version cloudlab-client|tex-creator|texsample-console lin|mac|win <version> <url>";
    ch.description = BTranslation::translate("BTerminalIOHandler",
                                             "Set the latest version of an application along with the download URL");
    setCommandHelp("set-app-version", ch);
}

TerminalIOHandler::~TerminalIOHandler()
{
    //
}

/*============================== Public slots =============================*/

bool TerminalIOHandler::connectToHost(const QString &hostName)
{
    if (mremote->isConnected())
    {
        write(tr("Already connected to") + " " + mremote->peerAddress());
        return false;
    }
    QString hn = !hostName.isEmpty() ? hostName : QString("texsample-server.no-ip.org");
    QString login = readLine(tr("Enter login:") + " ");
    if (login.isEmpty())
    {
        writeLine(tr("Operation aborted"));
        return false;
    }
    QString pwd = readLineSecure(tr("Enter password:") + " ");
    if (pwd.isEmpty())
    {
        writeLine(tr("Operation aborted"));
        return false;
    }
    writeLine(tr("Connecting to", "") + " " + hn + "...");
    mremote->connectToHost(hn, Texsample::MainPort);
    if (!mremote->isConnected() && !mremote->waitForConnected())
    {
        mremote->close();
        writeLine(tr("Failed to connect to") + " " + hn);
        return false;
    }
    writeLine(tr("Connected to") + " " + hn + ". " + tr("Authorizing..."));
    QVariantMap out;
    out.insert("login", login);
    out.insert("password", QCryptographicHash::hash(pwd.toUtf8(), QCryptographicHash::Sha1));
    out.insert("client_info", TClientInfo::createInfo());
    out.insert("subscription", true);
    BNetworkOperation *op = mremote->sendRequest(Texsample::AuthorizeRequest, out);
    if (!op->isFinished() && !op->isError() && !op->waitForFinished())
    {
        op->cancel();
        op->deleteLater();
        writeLine(tr("Authorization timed out"));
        return false;
    }
    op->deleteLater();
    if (op->isError())
    {
        mremote->close();
        writeLine(tr("Operation error"));
        return false;
    }
    QVariantMap in = op->variantData().toMap();
    TOperationResult r = in.value("operation_result").value<TOperationResult>();
    quint64 id = in.value("user_id").toULongLong();
    if (!r)
    {
        mremote->close();
        writeLine(tr("Authorization failed. The following error occured:") + " " + r.messageString());
        return false;
    }
    muserId = id;
    writeLine(tr("Authorized successfully with user id") + " " + QString::number(id));
    return true;
}

bool TerminalIOHandler::disconnectFromHost()
{
    if (!mremote->isConnected())
    {
        writeLine(tr("Not connected"));
        return false;
    }
    mremote->disconnectFromHost();
    if (mremote->isConnected() && !mremote->waitForDisconnected())
    {
        mremote->close();
        writeLine(tr("Disconnect timeout, socket closed"));
    }
    return true;
}

bool TerminalIOHandler::showUptime()
{
    if (!muserId)
    {
        writeLine(tr("Not authoized"));
        return false;
    }
    BNetworkOperation *op = mremote->sendRequest(Texsample::UptimeRequest);
    if (!op->isFinished() && !op->isError() && !op->waitForFinished())
    {
        op->deleteLater();
        writeLine(tr("Operation error"));
        return false;
    }
    op->deleteLater();
    QVariantMap in = op->variantData().toMap();
    TOperationResult r = in.value("operation_result").value<TOperationResult>();
    if (r)
        writeLine(tr("Uptime:") + " " + msecsToString(in.value("msecs").toLongLong()));
    else
        writeLine(tr("Failed to show uptime. The following error occured:") + " " + r.messageString());
    return r;
}

bool TerminalIOHandler::user(const QStringList &args)
{
    if (!muserId)
    {
        writeLine(tr("Not authoized"));
        return false;
    }
    QVariantMap out;
    if (!args.isEmpty())
        out.insert("arguments", args);
    BNetworkOperation *op = mremote->sendRequest(Texsample::UserRequest, out);
    if (!op->isFinished() && !op->isError() && !op->waitForFinished())
    {
        op->deleteLater();
        writeLine(tr("Operation error"));
        return false;
    }
    op->deleteLater();
    QVariantMap in = op->variantData().toMap();
    TOperationResult r = in.value("operation_result").value<TOperationResult>();
    if (r)
    {
        QVariant result = in.value("result");
        if (args.isEmpty())
        {
            writeLine(tr("Connected user count:") + " " + QString::number(result.toInt()));
        }
        else if (args.first() == "--list")
        {
            QVariantList l = result.toList();
            int sz = l.size();
            if (sz)
                writeLine(tr("Listing connected users") + " (" + QString::number(sz) + "):");
            else
                writeLine(tr("There are no connected users"));
            foreach (const QVariant &v, l)
                writeLine(userPrefix(v.toMap()));
        }
        else if (args.size() == 2)
        {
            if (args.first() == "--kick")
            {
                writeLine(r.messageString());
            }
            else if (args.first() == "--info")
            {
                foreach (const QVariant &v, result.toList())
                {
                    QVariantMap m = v.toMap();
                    QString f = "[%u] [%p] [%i]\n%a; %o [%l]\n%c v%v; TeXSample v%t; BeQt v%b; Qt v%q";
                    f.replace("%l", m.value("locale").toLocale().name());
                    QString s = m.value("client_info").value<TClientInfo>().toString(f);
                    s.replace("%d", QString::number(m.value("user_id").toULongLong()));
                    s.replace("%u", m.value("login").toString());
                    s.replace("%p", m.value("address").toString());
                    s.replace("%i", BeQt::pureUuidText(m.value("unique_id").value<QUuid>()));
                    s.replace("%a", m.value("access_level").value<TAccessLevel>().toStringNoTr());
                    writeLine(s);
                }
            }
            else if (args.first() == "--uptime")
            {
                foreach (const QVariant &v, result.toList())
                    writeLine(tr("Uptime of") + " " + userPrefix(v.toMap()) + " "
                              + msecsToString(v.toMap().value("uptime").toLongLong()));
            }
            else if (args.first() == "--connected-at")
            {
                foreach (const QVariant &v, result.toList())
                {
                    QDateTime dt = v.toMap().value("connection_dt").toDateTime().toLocalTime();
                    writeLine(tr("Connection time of") + " " + userPrefix(v.toMap()) + " "
                              + dt.toString(bLogger->dateTimeFormat()));
                }
            }
        }
    }
    else
    {
        writeLine(tr("Failed to execute request. The following error occured:") + " " + r.messageString());
    }
    return r;
}

bool TerminalIOHandler::setAppVersion(const QStringList &args)
{
    if (!muserId)
    {
        writeLine(tr("Not authoized"));
        return false;
    }
    QVariantMap out;
    if (!args.isEmpty())
        out.insert("arguments", args);
    BNetworkOperation *op = mremote->sendRequest(Texsample::SetLatestAppVersionRequest, out);
    if (!op->isFinished() && !op->isError() && !op->waitForFinished())
    {
        op->deleteLater();
        writeLine(tr("Operation error"));
        return false;
    }
    op->deleteLater();
    TOperationResult r = op->variantData().toMap().value("operation_result").value<TOperationResult>();
    if (!r)
        writeLine(tr("Failed to set app version. The following error occured:") + " " + r.messageString());
    else
        writeLine(r.messageString());
    return r;
}

bool TerminalIOHandler::startServer(const QString &address)
{
    if (!muserId)
    {
        writeLine(tr("Not authoized"));
        return false;
    }
    QVariantMap out;
    if (!address.isEmpty())
        out.insert("address", address);
    BNetworkOperation *op = mremote->sendRequest(Texsample::StartServerRequest, out);
    if (!op->isFinished() && !op->isError() && !op->waitForFinished())
    {
        op->deleteLater();
        writeLine(tr("Operation error"));
        return false;
    }
    op->deleteLater();
    TOperationResult r = op->variantData().toMap().value("operation_result").value<TOperationResult>();
    if (!r)
        writeLine(tr("Failed to start server. The following error occured:") + " " + r.messageString());
    else
        writeLine(r.messageString());
    return r;
}

bool TerminalIOHandler::stopServer()
{
    if (!muserId)
    {
        writeLine(tr("Not authoized"));
        return false;
    }
    BNetworkOperation *op = mremote->sendRequest(Texsample::StopServerRequest);
    if (!op->isFinished() && !op->isError() && !op->waitForFinished())
    {
        op->deleteLater();
        writeLine(tr("Operation error"));
        return false;
    }
    op->deleteLater();
    TOperationResult r = op->variantData().toMap().value("operation_result").value<TOperationResult>();
    if (!r)
        writeLine(tr("Failed to stop server. The following error occured:") + " " + r.messageString());
    else
        writeLine(r.messageString());
    return r;
}

/*============================== Protected methods =========================*/

bool TerminalIOHandler::handleCommand(const QString &, const QStringList &)
{
    writeLine(tr("Unknown command. Enter \"help --commands\" to see the list of available commands"));
    return false;
}

/*============================== Static private methods ====================*/

QString TerminalIOHandler::msecsToString(qint64 msecs)
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

QString TerminalIOHandler::userPrefix(const QVariantMap &m)
{
    return "[" + m.value("login").toString() + "] [" + m.value("address").toString() + "] ["
            + BeQt::pureUuidText(m.value("unique_id").value<QUuid>()) + "]";
}

/*============================== Private methods ===========================*/

bool TerminalIOHandler::handleConnect(const QString &, const QStringList &args)
{
    return connectToHost(!args.isEmpty() ? args.first() : QString());
}

bool TerminalIOHandler::handleDisconnect(const QString &, const QStringList &)
{
    return disconnectFromHost();
}

bool TerminalIOHandler::handleUptime(const QString &, const QStringList &)
{
    return showUptime();
}

bool TerminalIOHandler::handleUser(const QString &, const QStringList &args)
{
    return user(args);
}

bool TerminalIOHandler::handleSetAppVersion(const QString &, const QStringList &args)
{
    return setAppVersion(args);
}

bool TerminalIOHandler::handleStart(const QString &, const QStringList &args)
{
    return startServer(!args.isEmpty() ? args.first() : QString());
}

bool TerminalIOHandler::handleStop(const QString &, const QStringList &)
{
    return stopServer();
}

/*============================== Private slots =============================*/

void TerminalIOHandler::disconnected()
{
    muserId = 0;
    writeLine(tr("Disconnected"));
}

void TerminalIOHandler::error(QAbstractSocket::SocketError)
{
    writeLine(tr("Error:") + " " + mremote->errorString());
    muserId = 0;
}
