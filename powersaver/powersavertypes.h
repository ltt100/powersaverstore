#ifndef POWERSAVERTYPE_H
#define POWERSAVERTYPE_H

#include <QtCore/QString>
#include <QtCore/QMetaType>
#include <QDBusMetaType>
#include <QtDBus/QtDBus>
#include <QtDBus>

struct powerSaverStrategy
{
    QString mModuleName;
    int mLevel;
    int mValue;
/*
    powerSaverStrategy() : mModuleName(""), mLevel(0), mValue(0)
    {}

    bool operator==(const powerSaverStrategy &other) const
    {
      return (mModuleName == other.mModuleName) && (mLevel == other.mLevel) && (mValue == other.mValue);
    }
*/
};

Q_DECLARE_METATYPE (powerSaverStrategy)
QDBusArgument &operator<<(QDBusArgument &argument, const powerSaverStrategy &obj);
const QDBusArgument &operator>>(const QDBusArgument &argument, powerSaverStrategy &obj);

typedef QList<powerSaverStrategy> powerSaverStrategyList;
Q_DECLARE_METATYPE (powerSaverStrategyList)

inline void registerCommonDataTypes() {
    qDBusRegisterMetaType<powerSaverStrategy>();
    qDBusRegisterMetaType<powerSaverStrategyList>();
    qRegisterMetaType<powerSaverStrategyList>("powerSaverStrategyList");
}

#endif // POWERSAVERTYPE_H

