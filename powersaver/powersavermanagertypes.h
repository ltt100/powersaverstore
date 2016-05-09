#ifndef POWERSAVERMANAGERTYPE_H
#define POWERSAVERMANAGERTYPE_H

#include <QtCore/QString>
#include <QtCore/QMetaType>
#include <QDBusMetaType>
#include <QtDBus/QtDBus>
#include <QtDBus>

struct powerSaverManagerStrategy
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

Q_DECLARE_METATYPE (powerSaverManagerStrategy)
QDBusArgument &operator<<(QDBusArgument &argument, const powerSaverManagerStrategy &obj);
const QDBusArgument &operator>>(const QDBusArgument &argument, powerSaverManagerStrategy &obj);

typedef QList<powerSaverManagerStrategy> powerSaverManagerStrategyList;
Q_DECLARE_METATYPE (powerSaverManagerStrategyList)

inline void registerCommonDataTypes() {
    qDBusRegisterMetaType<powerSaverManagerStrategy>();
    qDBusRegisterMetaType<powerSaverManagerStrategyList>();
    qRegisterMetaType<powerSaverManagerStrategyList>("powerSaverManagerStrategyList");
}

#endif // POWERSAVERMANAGERTYPE_H

