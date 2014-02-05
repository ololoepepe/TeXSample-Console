#ifndef TERMINALIOHANDLER_H
#define TERMINALIOHANDLER_H

class BNetworkConnection;
class BNetworkOperation;

class QStringList;
class QVariant;

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
public slots:
    bool connectToHost(const QString &hostName);
    bool disconnectFromHost();
    bool showUptime();
    bool user(const QStringList &args);
    bool setAppVersion(const QStringList &args);
    bool startServer(const QString &address = QString());
    bool stopServer();
protected:
    bool handleCommand(const QString &command, const QStringList &arguments);
private:
    static QString msecsToString(qint64 msecs);
    static QString userPrefix(const QVariantMap &userData);
private:
    bool handleConnect(const QString &cmd, const QStringList &args);
    bool handleDisconnect(const QString &cmd, const QStringList &args);
    bool handleUptime(const QString &cmd, const QStringList &args);
    bool handleUser(const QString &cmd, const QStringList &args);
    bool handleSetAppVersion(const QString &cmd, const QStringList &args);
    bool handleStart(const QString &cmd, const QStringList &args);
    bool handleStop(const QString &cmd, const QStringList &args);
private slots:
    void disconnected();
    void error(QAbstractSocket::SocketError err);
private:
    BNetworkConnection *mremote;
    quint64 muserId;
};

#endif // TERMINALIOHANDLER_H
