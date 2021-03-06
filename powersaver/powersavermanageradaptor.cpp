/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c PowerSaverAdaptor -a powersaveradaptor.h:powersaveradaptor.cpp powersaver.xml -i powersavertypes.h
 *
 * qdbusxml2cpp is Copyright (C) 2015 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "powersaveradaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class PowerSaverAdaptor
 */

PowerSaverAdaptor::PowerSaverAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

PowerSaverAdaptor::~PowerSaverAdaptor()
{
    // destructor
}

QStringList PowerSaverAdaptor::users() const
{
    // get the value of property users
    return qvariant_cast< QStringList >(parent()->property("users"));
}

bool PowerSaverAdaptor::disablePowerSaving()
{
    // handle method call com.syberos.powersaver.Interface.disablePowerSaving
    bool retval;
    QMetaObject::invokeMethod(parent(), "disablePowerSaving", Q_RETURN_ARG(bool, retval));
    return retval;
}

bool PowerSaverAdaptor::enablePowerSaving(int level)
{
    // handle method call com.syberos.powersaver.Interface.enablePowerSaving
    bool retval;
    QMetaObject::invokeMethod(parent(), "enablePowerSaving", Q_RETURN_ARG(bool, retval), Q_ARG(int, level));
    return retval;
}

int PowerSaverAdaptor::getPowerSavingLevel()
{
    // handle method call com.syberos.powersaver.Interface.getPowerSavingLevel
    int retval;
    QMetaObject::invokeMethod(parent(), "getPowerSavingLevel", Q_RETURN_ARG(int, retval));
    return retval;
}

powerSaverStrategyList PowerSaverAdaptor::getPowerSavingStrategy()
{
    // handle method call com.syberos.powersaver.Interface.getPowerSavingStrategy
    powerSaverStrategyList retval;
    QMetaObject::invokeMethod(parent(), "getPowerSavingStrategy", Q_RETURN_ARG(powerSaverStrategyList, retval));
    return retval;
}

bool PowerSaverAdaptor::setPowerSavingStrategy(powerSaverStrategyList strategyList)
{
    // handle method call com.syberos.powersaver.Interface.setPowerSavingStrategy
    bool retval;
    QMetaObject::invokeMethod(parent(), "setPowerSavingStrategy", Q_RETURN_ARG(bool, retval), Q_ARG(powerSaverStrategyList, strategyList));
    return retval;
}

