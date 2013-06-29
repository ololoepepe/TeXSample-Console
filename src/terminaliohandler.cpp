#include "terminaliohandler.h"

#include <TeXSample>
#include <TOperationResult>
#include <TClientInfo>

#include <BNetworkConnection>
#include <BGenericSocket>
#include <BeQt>
#include <BNetworkOperation>
#include <BLogger>
#include <BCoreApplication>

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

#include <QDebug>

/*============================================================================
================================ SettingsItem ================================
============================================================================*/

class SettingsItem
{
public:
    explicit SettingsItem();
    explicit SettingsItem(const QString &key, QVariant::Type t = QVariant::String);
    SettingsItem(const SettingsItem &other);
public:
    void setKey(const QString &key);
    void setType(const QVariant::Type t);
    void setProperty(const QString &name, const QVariant &value = QVariant());
    void addChildItem(const SettingsItem &item);
    void addChildItem(const QString &key, QVariant::Type t = QVariant::String);
    void removeChildItem(const QString &key);
    QString key() const;
    QVariant::Type type() const;
    QVariant property(const QString &name) const;
    QList<SettingsItem> childItems() const;
    SettingsItem testPath(const QString &path, const QChar &separator = '.') const;
public:
    SettingsItem &operator =(const SettingsItem &other);
    bool operator ==(const SettingsItem &other) const;
private:
    QString mkey;
    QVariant::Type mtype;
    QVariantMap mproperties;
    QList<SettingsItem> mchildren;
};

/*============================================================================
================================ SettingsItem ================================
============================================================================*/

/*============================== Public constructors =======================*/

SettingsItem::SettingsItem()
{
    mtype = QVariant::Invalid;
}

SettingsItem::SettingsItem(const QString &key, QVariant::Type t)
{
    mkey = key;
    mtype = t;
}

SettingsItem::SettingsItem(const SettingsItem &other)
{
    *this = other;
}

/*============================== Public methods ============================*/

void SettingsItem::setKey(const QString &key)
{
    mkey = key;
}

void SettingsItem::setType(const QVariant::Type t)
{
    mtype = t;
}

void SettingsItem::setProperty(const QString &name, const QVariant &value)
{
    if (name.isEmpty())
        return;
    if (value.isValid())
        mproperties[name] = value;
    else
        mproperties.remove(name);
}

void SettingsItem::addChildItem(const SettingsItem &item)
{
    if (item.key().isEmpty() || QVariant::Invalid == item.type() || mchildren.contains(item))
        return;
    mchildren << item;
}

void SettingsItem::addChildItem(const QString &key, QVariant::Type t)
{
    addChildItem(SettingsItem(key, t));
}

void SettingsItem::removeChildItem(const QString &key)
{
    if (key.isEmpty())
        return;
    mchildren.removeAll(SettingsItem(key));
}

QString SettingsItem::key() const
{
    return mkey;
}

QVariant::Type SettingsItem::type() const
{
    return mtype;
}

QVariant SettingsItem::property(const QString &name) const
{
    return mproperties.value(name);
}

QList<SettingsItem> SettingsItem::childItems() const
{
    return mchildren;
}

SettingsItem SettingsItem::testPath(const QString &path, const QChar &separator) const
{
    if (path.isEmpty())
        return SettingsItem();
    if (mkey.isEmpty())
    {
        foreach (const SettingsItem &i, mchildren)
        {
            SettingsItem si = i.testPath(path, separator);
            if (QVariant::Invalid != si.type())
                return si;
        }
    }
    else
    {
        QStringList sl = path.split(!separator.isNull() ? separator : QChar('.'));
        if (sl.takeFirst() != mkey)
            return SettingsItem();
        QString spath = sl.join(QString(separator));
        if (spath.isEmpty())
            return *this;
        foreach (const SettingsItem &i, mchildren)
        {
            SettingsItem si = i.testPath(spath, separator);
            if (QVariant::Invalid != si.type())
                return si;
        }
    }
    return SettingsItem();
}

/*============================== Public operators ==========================*/

SettingsItem &SettingsItem::operator =(const SettingsItem &other)
{
    mkey = other.mkey;
    mtype = other.mtype;
    mproperties = other.mproperties;
    mchildren = other.mchildren;
    return *this;
}

bool SettingsItem::operator ==(const SettingsItem &other) const
{
    return mkey == other.mkey; //TODO
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
    mremote->setLogger(new BLogger);
    mremote->logger()->setLogToConsoleEnabled(false);
    connect(mremote, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(mremote, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    connect(mremote, SIGNAL(requestReceived(BNetworkOperation *)), this, SLOT(remoteRequest(BNetworkOperation *)));
    installHandler("quit", &BeQt::handleQuit);
    installHandler("exit", &BeQt::handleQuit);
    installHandler("connect", (InternalHandler) &TerminalIOHandler::handleConnect);
    installHandler("disconnect", (InternalHandler) &TerminalIOHandler::handleDisconnect);
    installHandler("remote-quit", (InternalHandler) &TerminalIOHandler::handleRemote);
    installHandler("remote-exit", (InternalHandler) &TerminalIOHandler::handleRemote);
    installHandler("user", (InternalHandler) &TerminalIOHandler::handleRemote);
    installHandler("uptime", (InternalHandler) &TerminalIOHandler::handleRemote);
    installHandler("set", (InternalHandler) &TerminalIOHandler::handleRemote);
    installHandler("start", (InternalHandler) &TerminalIOHandler::handleRemote);
    installHandler("stop", (InternalHandler) &TerminalIOHandler::handleRemote);
    installHandler("local-set", (InternalHandler) &TerminalIOHandler::handleSetLocal);
    installHandler("help", (InternalHandler) &TerminalIOHandler::handleHelp);
}

TerminalIOHandler::~TerminalIOHandler()
{
    //
}

/*============================== Public methods ============================*/

void TerminalIOHandler::connectToHost(const QString &hostName)
{
    if (hostName.isEmpty())
        return;
    writeLine(tr("Connecting to", "") + " " + hostName + "...");
    handleConnect("connect", QStringList() << hostName);
}

/*============================== Static private methods ====================*/

void TerminalIOHandler::writeHelpLine(const QString &command, const QString &description)
{
    QString s = "  " + command;
    if (s.length() > 28)
        s += "\n" + QString().fill(' ', 30);
    else
        s += QString().fill(' ', 30 - s.length());
    s += description;
    writeLine(s);
}

/*============================== Private methods ===========================*/

void TerminalIOHandler::handleConnect(const QString &, const QStringList &args)
{
    if (args.size() != 1)
        return writeLine(tr("Invalid usage", "") + "\nconnect <host>");
    QString login = readLine(tr("Enter login:", "") + " ");
    if (login.isEmpty())
        return writeLine(tr("Canceled", ""));
    write(tr("Enter password:", "") + " ");
    setStdinEchoEnabled(false);
    QString password = readLine();
    setStdinEchoEnabled(true);
    writeLine("");
    if (password.isEmpty())
        return writeLine(tr("Canceled", ""));
    QMetaObject::invokeMethod(this, "connectToHost", Qt::QueuedConnection, Q_ARG(QString, args.first()),
                              Q_ARG(QString, login), Q_ARG(QString, password));
}

void TerminalIOHandler::handleDisconnect(const QString &, const QStringList &)
{
    QMetaObject::invokeMethod(this, "disconnectFromHost", Qt::QueuedConnection);
}

void TerminalIOHandler::handleRemote(const QString &cmd, const QStringList &args)
{
    if (cmd.startsWith("remote-"))
        return handleRemote(cmd.mid(7), args);
    QMetaObject::invokeMethod(this, "sendCommand", Qt::QueuedConnection, Q_ARG(QString, cmd),
                              Q_ARG(QStringList, QStringList(args)));
}

void TerminalIOHandler::handleSetLocal(const QString &, const QStringList &args)
{
    init_once(SettingsItem, Settings, SettingsItem())
    {
        SettingsItem l("Log");
          l.addChildItem("noop", QVariant::Bool);
        Settings.addChildItem(l);
    }
    if (args.size() < 1 || args.size() > 2)
        return writeLine(tr("Invalid parameters", ""));
    QString path = args.first();
    SettingsItem si = Settings.testPath(path);
    if (QVariant::Invalid == si.type())
        return writeLine(tr("No such option", ""));
    path.replace('.', '/');
    QVariant v;
    if (args.size() == 2)
        v = args.last();
    else
        v = readLine(tr("Enter value for", "") + " \"" + path.split("/").last() + "\": ");
    if (!v.convert(si.type()))
        return writeLine(tr("Invalid value", ""));
    bSettings->setValue(path, v);
    writeLine(tr("OK", ""));
}

void TerminalIOHandler::handleHelp(const QString &, const QStringList &)
{
    writeLine(tr("The following commands are available:", "help"));
    writeHelpLine("help", tr("Show this Help", "help"));
    writeHelpLine("quit, exit", tr("Quit the application", "help"));
    writeHelpLine("remote-quit, remote-exit", tr("Quit the remote server application", "help"));
    writeHelpLine("connect", tr("Connect to a remote server", "help"));
    writeHelpLine("disconnect", tr("Disconnect from the remote server", "help"));
    writeHelpLine("uptime", tr("Show for how long the remote server application has been running", "help"));
    writeHelpLine("user [--list], user [--connected-at|--info|--kick|--uptime] <id|login>",
                  tr("Show information about the user(s) connected", "help"));
    writeHelpLine("set <key> [value]", tr("Set remote server configuration variable", "help"));
    writeHelpLine("local-set <key> [value]", tr("Set local configuration variable", "help"));
    writeHelpLine("start", tr("Start the main server and the registration server", "help"));
    writeHelpLine("stop", tr("Stop the main server and the registration server", "help"));
}

/*============================== Private slots =============================*/

void TerminalIOHandler::connectToHost(const QString &hostName, const QString &login, const QString &password)
{
    if (mremote->isConnected())
        return write(tr("Already connected to", "") + " " + mremote->peerAddress());
    mremote->connectToHost(hostName, 9041);
    if (!mremote->isConnected() && !mremote->waitForConnected())
    {
        mremote->close();
        writeLine(tr("Failed to connect to", "") + " " + hostName);
        return;
    }
    writeLine(tr("Connected to", "") + " " + hostName + ". " + tr("Authorizing...", ""));
    QVariantMap out;
    out.insert("login", login);
    out.insert("password", QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha1));
    out.insert("client_info", TClientInfo::createDefaultInfo());
    out.insert("subscription", true);
    BNetworkOperation *op = mremote->sendRequest(Texsample::AuthorizeRequest, out);
    if (!op->isFinished() && !op->isError() && !op->waitForFinished())
    {
        op->cancel();
        op->deleteLater();
        return writeLine(tr("Authorization timed out", ""));
    }
    op->deleteLater();
    if (op->isError())
    {
        mremote->close();
        return writeLine(tr("Operation error", ""));
    }
    QVariantMap in = op->variantData().toMap();
    TOperationResult r = in.value("operation_result").value<TOperationResult>();
    quint64 id = in.value("user_id").toULongLong();
    if (!r)
    {
        mremote->close();
        return writeLine(tr("Authorization failed. The following error occured:", "") + " " + r.errorString());
    }
    muserId = id;
    writeLine(tr("Authorized successfully with user id", "") + " " + QString::number(id));
}

void TerminalIOHandler::disconnectFromHost()
{
    if (!mremote->isConnected())
        return writeLine(tr("Not connected", ""));
    mremote->disconnectFromHost();
    if (mremote->isConnected() && !mremote->waitForDisconnected())
    {
        mremote->close();
        return writeLine(tr("Disconnect timeout, socket closed", ""));
    }
}

bool TerminalIOHandler::sendCommand(const QString &cmd, const QStringList &args)
{
    if (!muserId)
    {
        writeLine(tr("Not authoized", ""));
        return false;
    }
    QVariantMap out;
    out.insert("command", cmd);
    out.insert("arguments", args);
    BNetworkOperation *op = mremote->sendRequest(Texsample::ExecuteCommandRequest, out);
    if (!op->isFinished() && !op->isError() && !op->waitForFinished())
    {
        op->deleteLater();
        writeLine(tr("Operation error", ""));
        return false;
    }
    op->deleteLater();
    TOperationResult r = op->variantData().toMap().value("operation_result").value<TOperationResult>();
    if (!r)
        writeLine(tr("Failed to execute command. The following error occured:", "") + " " + r.errorString());
    return r;
}

void TerminalIOHandler::disconnected()
{
    muserId = 0;
    writeLine(tr("Disconnected", ""));
}

void TerminalIOHandler::error(QAbstractSocket::SocketError)
{
    writeLine(tr("Error:") + " " + mremote->errorString());
    muserId = 0;
}

void TerminalIOHandler::remoteRequest(BNetworkOperation *op)
{
    QVariantMap in = op->variantData().toMap();
    if (op->metaData().operation() == Texsample::LogRequest)
    {
        QString msg = in.value("log_text").toString();
        int ilvl = in.value("level").toInt();
        BLogger::Level lvl = bRangeD(BLogger::NoLevel, BLogger::CriticalLevel).contains(ilvl) ?
                    static_cast<BLogger::Level>(ilvl) : BLogger::NoLevel;
        BCoreApplication::log(msg, lvl);
    }
    else if (op->metaData().operation() == Texsample::WriteRequest)
    {
        QString text = in.value("text").toString();
        BTerminalIOHandler::write(text);
    }
    else if (op->metaData().operation() == "noop")
    {
        if (bSettings->value("Log/noop").toBool())
            bLogger->log(tr("Replying to connection test...", "log"));
    }
    mremote->sendReply(op);
    op->waitForFinished();
    op->deleteLater();
}
