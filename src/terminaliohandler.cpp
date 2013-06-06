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

#include <QDebug>

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
    writeLine(tr("Disconnected", ""));
}

void TerminalIOHandler::sendCommand(const QString &cmd, const QStringList &args)
{
    if (!muserId)
        return writeLine(tr("Not authoized", ""));
    QVariantMap out;
    out.insert("command", cmd);
    out.insert("arguments", args);
    BNetworkOperation *op = mremote->sendRequest(Texsample::ExecuteCommandRequest, out);
    if (!op->isFinished() && !op->isError() && !op->waitForFinished())
    {
        op->deleteLater();
        return writeLine(tr("Operation error", ""));
    }
    op->deleteLater();
    TOperationResult r = op->variantData().toMap().value("operation_result").value<TOperationResult>();
    if (!r)
        writeLine(tr("Failed to execute command. The following error occured:", "") + " " + r.errorString());
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
    mremote->sendReply(op, QByteArray());
    op->waitForFinished();
    op->deleteLater();
}
