#ifndef GLOBAL_H
#define GLOBAL_H

class BSettingsNode;

class QVariant;

namespace Global
{

bool setLoggingMode(const BSettingsNode *, const QVariant &v);
void resetLoggingMode();

}

#endif // GLOBAL_H
