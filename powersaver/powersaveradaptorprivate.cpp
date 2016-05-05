/*
* This file is part of timemanager
*
* Copyright (C) 2014 Beijing Yuan Xin Technology Co.,Ltd. All rights reserved.
*
* Authors:
*       support <support@syberos.com>
*
* This software, including documentation, is protected by copyright controlled
* by Beijing Yuan Xin Technology Co.,Ltd. All rights are reserved.
*/

#include "powersaveradaptorprivate.h"
#include <QDebug>
#include <QDBusConnection>
#include <QVariantMap>
#include <QDBusVariant>

#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>

//using namespace SYBEROS;

static const QString SERVICE_NAME      = "com.syberos.powersaver";
static const QString SERVICE_PATH      = "/com/syberos/powersaver";
static const QString SERVICE_INTERFACE = "com.syberos.powersaver.Interface";

PowerSaverAdaptorPrivate::PowerSaverAdaptorPrivate(QObject *parent):
    QObject(parent)
{
    m_level = S_OFF;
    initModules();
    m_powerStorage = new PowerSaverStorageJson(PowerSaverStorageJson::Json, this);
    if (m_powerStorage->isStrategyExist())
    {
        getStrategy();
        m_level = (PowerSaverLevel)m_strategy.value("level").toInt();
    }
    else
    {
        saveDefaultStrategy();
    }
    initFuncList();

    new PowerSaverAdaptor(this);

    if (!QDBusConnection::systemBus().registerService(SERVICE_NAME)) {
        qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "PowerSaverAdaptorPrivate::PowerSaverAdaptorPrivate Register dbus service failed! " ;

    }

    if (!QDBusConnection::systemBus().registerObject(SERVICE_PATH, this)) {
        qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "PowerSaverAdaptorPrivate::PowerSaverAdaptorPrivate Register dbus object failed! ";
    }

    // TODO: Add code
}

bool PowerSaverAdaptorPrivate::disablePowerSaving()
{
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to disable power saving!";

    return setPowerSaving(S_OFF);
}

bool PowerSaverAdaptorPrivate::enablePowerSaving(int level)
{
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to enable power saving: level " << level;

    if ((level >= S_OFF) && (level <= S_END))
    {
        return setPowerSaving((PowerSaverLevel)level);
    }
    else
    {
        qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO << "Invaild level value!\n";
        return false;
    }
}

int PowerSaverAdaptorPrivate::getPowerSavingLevel()
{
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to get power saving level: " << m_level;
    return m_level;
}

powerSaverStrategyList PowerSaverAdaptorPrivate::getPowerSavingStrategy()
{
    // handle method call com.syberos.powersaver.Interface.getPowerSavingStrategy
    powerSaverStrategyList ret;
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to get power saving strategy: ";
    return ret;
}

bool PowerSaverAdaptorPrivate::setPowerSavingStrategy(powerSaverStrategyList strategyList)
{
    bool ret = false;
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to set power saving strategy: ";
    Q_UNUSED(strategyList);
    return ret;
}

void PowerSaverAdaptorPrivate::saveDefaultStrategy()
{
    // First we construct the default strategy
    //m_strategy.clear();
    m_strategy.insert("level", QVariant(int(S_OFF)));
    m_strategy.insert("geoClue", QVariant(int(S0)));
    m_strategy.insert("blueTooth", QVariant(int(S0)));
    m_strategy.insert("wallpaper", QVariant(int(S0)));
    m_strategy.insert("ngfd", QVariant(int(S0)));
    m_strategy.insert("devMonitor", QVariant(int(S0)));
    m_strategy.insert("kernel", QVariant(int(S0)));
    m_strategy.insert("sysMgr", QVariant(int(S0)));
    m_strategy.insert("lockScreen", QVariant(int(S0)));
    m_strategy.insert("ofono", QVariant(int(S0)));
    m_strategy.insert("connman", QVariant(int(S0)));
    /* mce is a special module, it need change value for different level
       level is marked by the list's index
     */
    // TODO: We not know the level of backlight and sleeptime, we need Adjust the following numbers
    QList<QVariant> backlight;
    backlight.append(QVariant(0));
    backlight.append(QVariant(60));
    backlight.append(QVariant(123));
    backlight.append(QVariant(190));
    backlight.append(QVariant(255));
    m_strategy.insert("MCE_backlight", QVariant(backlight));
    QList<QVariant> sleeptime;
    sleeptime.append(QVariant(13));
    sleeptime.append(QVariant(28));
    sleeptime.append(QVariant(58));
    sleeptime.append(QVariant(298));
    sleeptime.append(QVariant(598));
    m_strategy.insert("MCE_sleeptime", QVariant(sleeptime));
    m_powerStorage->updateStrategy(m_strategy);
}

void PowerSaverAdaptorPrivate::initModules(void)
{
//    pSaverFunc a = &PowerSaverAdaptorPrivate::geoClueSaver;
//    (this->*a)(m_level);

    // Init module and func map
    m_modules.insert("geoClue", &PowerSaverAdaptorPrivate::geoClueSaver);
    m_modules.insert("blueTooth", &PowerSaverAdaptorPrivate::blueToothSaver);
    m_modules.insert("wallpaper", &PowerSaverAdaptorPrivate::wallpaperSaver);
    m_modules.insert("ngfd", &PowerSaverAdaptorPrivate::ngfdSaver);
    m_modules.insert("devMonitor", &PowerSaverAdaptorPrivate::devMonitorSaver);
    m_modules.insert("kernel", &PowerSaverAdaptorPrivate::kernelSaver);
    m_modules.insert("sysMgr", &PowerSaverAdaptorPrivate::sysMgrSaver);
    m_modules.insert("lockScreen", &PowerSaverAdaptorPrivate::lockScreenSaver);
    m_modules.insert("ofono", &PowerSaverAdaptorPrivate::ofonoSaver);
    m_modules.insert("connman", &PowerSaverAdaptorPrivate::connmanSaver);
    m_modules.insert("MCE", &PowerSaverAdaptorPrivate::MCESaver);
}

void PowerSaverAdaptorPrivate::initFuncList(void)
{
    // Init the level func map. We read the data from database
    m_levelFuncMap.insertMulti(S0, &PowerSaverAdaptorPrivate::geoClueSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverAdaptorPrivate::blueToothSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverAdaptorPrivate::wallpaperSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverAdaptorPrivate::ngfdSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverAdaptorPrivate::devMonitorSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverAdaptorPrivate::kernelSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverAdaptorPrivate::sysMgrSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverAdaptorPrivate::lockScreenSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverAdaptorPrivate::ofonoSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverAdaptorPrivate::connmanSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverAdaptorPrivate::MCESaver);
}

void PowerSaverAdaptorPrivate::initStrategy()
{

}

void PowerSaverAdaptorPrivate::getStrategy()
{
    m_strategy = m_powerStorage->loadStrategy();
    Q_ASSERT(!m_strategy.isEmpty());
}

bool PowerSaverAdaptorPrivate::setPowerSaving(PowerSaverLevel level)
{
    bool ret = true;
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO << "going to setPowerSaving " << level;

    int delta = level - m_level;
    QMutableMapIterator<PowerSaverLevel, pSaverFunc> it(m_levelFuncMap);

    if (delta == 0)
    {
        qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO << "The same level value!\n";
    }
    else
    {
        if (delta > 0)
        {
            for(int i = 1; i <= delta; i++)
            {
                QList<pSaverFunc> values = m_levelFuncMap.values((PowerSaverLevel)(i + m_level));
                for (int j = 0; j < values.size(); ++j)
                {
                    (this->*(values.at(j)))(level);
                }
            }
        }
        else
        {
            for(int i = 0; i > delta; i--)
            {
                QList<pSaverFunc> values = m_levelFuncMap.values((PowerSaverLevel)(i + m_level));
                for (int j = 0; j < values.size(); ++j)
                {
                    (this->*(values.at(j)))(level);
                }
            }
        }

        // We will send signal to inform the change of powersaver
    }

    return ret;
}

void PowerSaverAdaptorPrivate::geoClueSaver(PowerSaverLevel level)
{
    Q_UNUSED(level);
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to geoClueSaver!\n";
}

void PowerSaverAdaptorPrivate::blueToothSaver(PowerSaverLevel level)
{
    Q_UNUSED(level);
}

void PowerSaverAdaptorPrivate::wallpaperSaver(PowerSaverLevel level)
{
    Q_UNUSED(level);
}

void PowerSaverAdaptorPrivate::ngfdSaver(PowerSaverLevel level)
{
    Q_UNUSED(level);
}

void PowerSaverAdaptorPrivate::devMonitorSaver(PowerSaverLevel level)
{
    Q_UNUSED(level);
}

void PowerSaverAdaptorPrivate::kernelSaver(PowerSaverLevel level)
{
    Q_UNUSED(level);
}

void PowerSaverAdaptorPrivate::sysMgrSaver(PowerSaverLevel level)
{
    Q_UNUSED(level);
}

void PowerSaverAdaptorPrivate::lockScreenSaver(PowerSaverLevel level)
{
    Q_UNUSED(level);
}

void PowerSaverAdaptorPrivate::ofonoSaver(PowerSaverLevel level)
{
    Q_UNUSED(level);
}

void PowerSaverAdaptorPrivate::connmanSaver(PowerSaverLevel level)
{
    Q_UNUSED(level);
}

void PowerSaverAdaptorPrivate::MCESaver(PowerSaverLevel level)
{
    Q_UNUSED(level);
}




