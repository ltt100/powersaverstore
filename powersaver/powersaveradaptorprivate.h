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

#ifndef POWERSAVERADAPTORPRIVATE_H
#define POWERSAVERADAPTORPRIVATE_H

#include <QObject>
#include <QDBusContext>
#include <QList>
#include "powersaveradaptor.h"
#include "powersaveradaptorprivate.h"
#include "powersaverstorageJson.h"


class PowerSaverAdaptorPrivate: public QObject,protected QDBusContext
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
    typedef void (PowerSaverAdaptorPrivate::*pSaverFunc)(PowerSaverLevel level);
    explicit PowerSaverAdaptorPrivate(QObject *parent = 0);

public slots:
    bool disablePowerSaving();
    bool enablePowerSaving(int level);
    int getPowerSavingLevel();
    powerSaverStrategyList getPowerSavingStrategy();
    bool setPowerSavingStrategy(powerSaverStrategyList strategyList);
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

    void geoClueSaver(PowerSaverLevel level);
    void blueToothSaver(PowerSaverLevel level);
    void wallpaperSaver(PowerSaverLevel level);
    void ngfdSaver(PowerSaverLevel level);
    void devMonitorSaver(PowerSaverLevel level);
    void kernelSaver(PowerSaverLevel level);
    void sysMgrSaver(PowerSaverLevel level);
    void lockScreenSaver(PowerSaverLevel level);
    void ofonoSaver(PowerSaverLevel level);
    void connmanSaver(PowerSaverLevel level);
    void MCEBacklightSaver(PowerSaverLevel level);
    void MCESleepTimeSaver(PowerSaverLevel level);

private:
    // m_level record the level of powersaver
    // it is a int type when it be saved in JSON
    PowerSaverLevel m_level;
    // m_modules is used to record the module and saver function pair
    QMap<QString, pSaverFunc> m_modules;
    // m_strategy used to record the strategy of different modules
    // To those module like wifi, QVariant is a int type
    // To module like mce, it is a list:QList<QVariant>
    QMap<QString, QVariant> m_strategy;
    // m_levelFuncMap used to switch power saving level when required to switch m_level
    // we records multi value in it if the same module has value in different levels
    QMap<PowerSaverLevel, pSaverFunc> m_levelFuncMap;
    PowerSaverStorageJson *m_powerStorage;
};

#endif // POWERSAVERADAPTORPRIVATE_H

