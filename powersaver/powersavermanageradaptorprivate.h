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

#ifndef POWERSAVERMANAGERADAPTORPRIVATE_H
#define POWERSAVERMANAGERADAPTORPRIVATE_H

#include <QObject>
#include <QDBusContext>
#include <QList>
#include "powersavermanageradaptor.h"
#include "powersavermanagerstorageJson.h"


class PowerSaverManagerAdaptorPrivate: public QObject,protected QDBusContext
{
    Q_OBJECT
public:
    enum PowerSaverLevel {
        S_OFF = 0,
        S0 = 1,
        S1,
        S2,
        S_END = 3
    };
    typedef void (PowerSaverManagerAdaptorPrivate::*pSaverFunc)(PowerSaverLevel level, PowerSaverLevel preLevel);
    explicit PowerSaverManagerAdaptorPrivate(QObject *parent = 0);

public slots:
    bool disablePowerSaving();
    bool enablePowerSaving(int level);
    int getPowerSavingLevel();
    powerSaverManagerStrategyList getPowerSavingStrategy();
    bool setPowerSavingStrategy(powerSaverManagerStrategyList strategyList);
signals:
    void powerSavingChanged(int level);

private:
    /*
     *  The init func will init the m_levelMap and m_funcMap
     *  When power saving level change, we will use m_funcMap to on/off app/service
     */
    void initModules(void);
    void initFuncList(void);
    void initStrategy();
    void getStrategy();
    void saveDefaultStrategy();
    bool applyPowerSaving(PowerSaverLevel level);
    void getPrevSetting();

    void geoClueSaver(PowerSaverLevel level, PowerSaverLevel preLevel);
    void blueToothSaver(PowerSaverLevel level, PowerSaverLevel preLevel);
    void wallpaperSaver(PowerSaverLevel level, PowerSaverLevel preLevel);
    void ngfdSaver(PowerSaverLevel level, PowerSaverLevel preLevel);
    void devMonitorSaver(PowerSaverLevel level, PowerSaverLevel preLevel);
    void kernelSaver(PowerSaverLevel level, PowerSaverLevel preLevel);
    void sysMgrSaver(PowerSaverLevel level, PowerSaverLevel preLevel);
    void lockScreenSaver(PowerSaverLevel level, PowerSaverLevel preLevel);
    void ofonoSaver(PowerSaverLevel level, PowerSaverLevel preLevel);
    void connmanCellularSaver(PowerSaverLevel level, PowerSaverLevel preLevel);
    void connmanWifiSaver(PowerSaverLevel level, PowerSaverLevel preLevel);
    void MCEBacklightSaver(PowerSaverLevel level, PowerSaverLevel preLevel);
    void MCESleepTimeSaver(PowerSaverLevel level, PowerSaverLevel preLevel);

private:
    // m_level record the level of powersaver
    // it is a int type when it be saved in JSON
    PowerSaverLevel m_level;
    // m_modules is used to record the module and saver function pair
    QMap<QString, pSaverFunc> m_modules;
    powerSaverManagerStrategyList m_strategyList;
    // m_strategy used to record the strategy of different modules
    // To those module like wifi, QVariant is a int type
    // To module like mce, it is a list:QList<QVariant>
    QMap<QString, QVariant> m_strategy;
    // m_levelFuncMap used to switch power saving level when required to switch m_level
    // we records multi value in it if the same module has value in different levels
    QMap<PowerSaverLevel, pSaverFunc> m_levelFuncMap;
    // m_prevSetting record the prev setting of every module
    QMap<QString, QVariant> m_preSetting;
    PowerSaverManagerStorageJson *m_powerStorage;
};

#endif // POWERSAVERMANAGERADAPTORPRIVATE_H

