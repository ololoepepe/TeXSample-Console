#include "terminaliohandler.h"
#include "global.h"

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

B_DECLARE_TRANSLATE_FUNCTION

int main(int argc, char *argv[])
{
    tInit();
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("TeXSample Console");
    QCoreApplication::setApplicationVersion("1.0.0-pa1");
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
    BCoreApplication::setApplicationCopyrightPeriod("2013");
    Q_UNUSED(bapp);
    QString msg = QCoreApplication::applicationName() + " v" + QCoreApplication::applicationVersion();
    BTerminalIOHandler::setTerminalTitle(msg);
    bWriteLine(translate("main", "This is") + " " + msg);
    BDirTools::createUserLocation("logs");
    BCoreApplication::logger()->setDateTimeFormat("yyyy.MM.dd hh:mm:ss");
    Global::resetLoggingMode();
    QString logfn = BCoreApplication::location(BCoreApplication::DataPath, BCoreApplication::UserResources) + "/logs/";
    logfn += QDateTime::currentDateTime().toString("yyyy.MM.dd-hh.mm.ss") + ".txt";
    BCoreApplication::logger()->setFileName(logfn);
    TerminalIOHandler handler;
    bWriteLine(translate("main", "Enter \"help --commands\" to see the list of available commands"));
    QStringList args = QCoreApplication::arguments();
    if (args.size() > 1)
        handler.connectToHost(args.at(1));
    ret = app.exec();
#if defined(BUILTIN_RESOURCES)
    Q_CLEANUP_RESOURCE(texsample_console);
    Q_CLEANUP_RESOURCE(texsample_console_translations);
#endif
    tCleanup();
    return ret;
}
