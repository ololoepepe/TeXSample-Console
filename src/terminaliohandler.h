#ifndef TERMINALIOHANDLER_H
#define TERMINALIOHANDLER_H

class BNetworkConnection;
class BNetworkOperation;

class QStringList;

#include <BTerminalIOHandler>

#include <QObject>
#include <QString>
#include <QAbstractSocket>

/*============================================================================
================================ TerminalIOHandler ===========================
============================================================================*/

class TerminalIOHandler : public BTerminalIOHandler
{
    Q_OBJECT
public:
    explicit TerminalIOHandler(QObject *parent = 0);
    ~TerminalIOHandler();
public:
    void connectToHost(const QString &hostName);
private:
    static void writeHelpLine(const QString &command, const QString &description);
private:
    void handleConnect(const QString &cmd, const QStringList &args);
    void handleDisconnect(const QString &cmd, const QStringList &args);
    void handleRemote(const QString &cmd, const QStringList &args);
    void handleSetLocal(const QString &cmd, const QStringList &args);
    void handleHelp(const QString &cmd, const QStringList &args);
private slots:
    void connectToHost(const QString &hostName, const QString &login, const QString &password);
    void disconnectFromHost();
    bool sendCommand(const QString &cmd, const QStringList &args);
    void disconnected();
    void error(QAbstractSocket::SocketError err);
    void remoteRequest(BNetworkOperation *op);
private:
    BNetworkConnection *mremote;
    quint64 muserId;
};

#endif // TERMINALIOHANDLER_H
