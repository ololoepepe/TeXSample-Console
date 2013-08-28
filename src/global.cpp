class BSettingsNode;

#include "global.h"

#include <BTerminalIOHandler>
#include <BCoreApplication>

#include <QVariant>
#include <QString>
#include <QSettings>

B_DECLARE_TRANSLATE_FUNCTION

namespace Global
{

bool setLoggingMode(const BSettingsNode *, const QVariant &v)
{
    QString s = !v.isNull() ? v.toString() : bReadLine(translate("Global", "Enter logging mode:") + " ");
    if (s.isEmpty())
        return false;
    bool ok = false;
    int m = s.toInt(&ok);
    if (!ok)
        return false;
    bSettings->setValue("Log/mode", m);
    resetLoggingMode();
    return true;
}

void resetLoggingMode()
{
    int m = bSettings->value("Log/mode", 2).toInt();
    if (m <= 0)
    {
        bLogger->setLogToConsoleEnabled(false);
        bLogger->setLogToFileEnabled(false);
    }
    else if (1 == m)
    {
        bLogger->setLogToConsoleEnabled(true);
        bLogger->setLogToFileEnabled(false);
    }
    else if (2 == m)
    {
        bLogger->setLogToConsoleEnabled(false);
        bLogger->setLogToFileEnabled(true);
    }
    else if (m >= 3)
    {
        bLogger->setLogToConsoleEnabled(true);
        bLogger->setLogToFileEnabled(true);
    }
}

}
