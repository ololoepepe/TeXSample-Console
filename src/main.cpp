#include "terminaliohandler.h"

#include <TeXSampleGlobal>

#include <BCoreApplication>
#include <BDirTools>
#include <BTranslator>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QCoreApplication>
#include <QDateTime>

#include <QDebug>

QString translate(const char *context, const char *key, const char *disambiguation = 0)
{
    return QCoreApplication::translate(context, key, disambiguation);
}

int main(int argc, char *argv[])
{
    tRegister();
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("TeXSample Console");
    QCoreApplication::setApplicationVersion("0.1.1");
    QCoreApplication::setOrganizationName("TeXSample Team");
    QCoreApplication::setOrganizationDomain("https://github.com/TeXSample-Team/TeXSample-Console");
#if defined(BUILTIN_RESOURCES)
    Q_INIT_RESOURCE(texsample_console);
    Q_INIT_RESOURCE(texsample_console_translations);
#endif
    int ret = 0;
    BCoreApplication bapp;
    BCoreApplication::installTranslator(new BTranslator("qt"));
    BCoreApplication::installTranslator(new BTranslator("beqt"));
    BCoreApplication::installTranslator(new BTranslator("texsample"));
    BCoreApplication::installTranslator(new BTranslator("texsample-console"));
    Q_UNUSED(bapp);
    TerminalIOHandler::writeLine(translate("main", "This is", "") + " " + QCoreApplication::applicationName() +
                                 " v" + QCoreApplication::applicationVersion());
    BDirTools::createUserLocation("logs");
    BCoreApplication::logger()->setDateTimeFormat("yyyy.MM.dd hh:mm:ss");
    QString logfn = BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::UserResources) + "/logs/";
    logfn += QDateTime::currentDateTime().toString("yyyy.MM.dd-hh.mm.ss") + ".txt";
    BCoreApplication::logger()->setFileName(logfn);
    TerminalIOHandler handler;
    QStringList args = QCoreApplication::arguments();
    if (args.size() > 1)
        handler.connectToHost(args.at(1));
    TerminalIOHandler::writeLine(translate("main", "Enter \"help\" to see commands list"));
    ret = app.exec();
    return ret;
}
