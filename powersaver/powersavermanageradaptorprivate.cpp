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

#include "powersavermanageradaptorprivate.h"
#include <QDebug>
#include <QDBusConnection>
#include <QVariantMap>
#include <QDBusVariant>

#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>

//using namespace SYBEROS;

static const QString SERVICE_NAME      = "com.syberos.powersavermanager";
static const QString SERVICE_PATH      = "/com/syberos/powersavermanager";
static const QString SERVICE_INTERFACE = "com.syberos.powersavermanager.Interface";

PowerSaverManagerAdaptorPrivate::PowerSaverManagerAdaptorPrivate(QObject *parent):
    QObject(parent), m_level(S_OFF)
{
    // Init the member of PowerSaverAdaptorPrivate
    initModules();
    m_powerStorage = new PowerSaverManagerStorageJson(PowerSaverManagerStorageJson::Json, this);
    if (m_powerStorage->isSettingExist())
    {
        getPrevSetting();
    }
    if (m_powerStorage->isStrategyExist())
    {
        getStrategy();
//        m_level = (PowerSaverLevel)m_strategy.value("level").toInt();
    }
    else
    {
//        m_level = S_OFF;
        saveDefaultStrategy();
    }
    initFuncList();

    new PowerSaverManagerAdaptor(this);

    // Read the recorded level and apply
    applyPowerSaving((PowerSaverLevel)m_strategy.value("level").toInt());

    if (!QDBusConnection::systemBus().registerService(SERVICE_NAME)) {
        qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "PowerSaverAdaptorPrivate::PowerSaverAdaptorPrivate Register dbus service failed! " ;

    }
    if (!QDBusConnection::systemBus().registerObject(SERVICE_PATH, this)) {
        qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "PowerSaverAdaptorPrivate::PowerSaverAdaptorPrivate Register dbus object failed! ";
    }

    // TODO: Add code to check power change
}

void PowerSaverManagerAdaptorPrivate::getPrevSetting()
{
    m_preSetting = m_powerStorage->loadStorage(PowerSaverManagerStorageJson::PrevSetting);
    Q_ASSERT(!m_preSetting.isEmpty());
}

bool PowerSaverManagerAdaptorPrivate::disablePowerSaving()
{
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to disable power saving!";

    // We will revert the setting to its prev setting
    bool ret = applyPowerSaving(S_OFF);
    // TODO: delete the preSetting json file
    return ret;
}

bool PowerSaverManagerAdaptorPrivate::enablePowerSaving(int level)
{
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to enable power saving: level " << level;

    bool ret = false;
    if ((level >= S_OFF) && (level <= S_END))
    {
        // we will save the setting to m_preSetting
        ret = applyPowerSaving((PowerSaverLevel)level);
    }
    else
    {
        qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO << "Invaild level value!\n";
    }
    // TODO: save the m_preSetting to preSetting json file
    return ret;
}

int PowerSaverManagerAdaptorPrivate::getPowerSavingLevel()
{
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to get power saving level: " << m_level;
    return m_level;
}

powerSaverManagerStrategyList PowerSaverManagerAdaptorPrivate::getPowerSavingStrategy()
{
    // handle method call com.syberos.powersaver.Interface.getPowerSavingStrategy
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to get power saving strategy: ";
    QMap<QString, QVariant>::iterator i;
    int j = 0;
    for (i = m_strategy.begin(); i != m_strategy.end(); ++i)
    {
        if (i.value().type() == QVariant::List)
        {
            QList<QVariant> l = i.value().toList();
//            for
        }
        else
        {
            m_strategyList[j].mModuleName = i.key();
            m_strategyList[j].mLevel = i.value().toInt();
            m_strategyList[j].mValue = 0;
            j++;
        }
    }
    return m_strategyList;
}

// At present, we only support to set the level of every module
bool PowerSaverManagerAdaptorPrivate::setPowerSavingStrategy(powerSaverManagerStrategyList strategyList)
{
    bool ret = false;    
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to set power saving strategy: ";
    if (!strategyList.empty())
    {
        QList<powerSaverManagerStrategy>::iterator i;
        for (i = strategyList.begin(); i != strategyList.end(); ++i)
        {
            QVariant v = m_strategy[i->mModuleName];
            if (v.type() == QVariant::List) // power saver value change
            {
                // record the prev value in tmp
                int tmp = (v.toList())[i->mLevel].toInt();
                // Update the  strategy value
                (m_strategy[i->mModuleName].toList())[i->mLevel] = QVariant(i->mValue);
                // Apply the new strategy
                if ((tmp != i->mValue) && (m_level == i->mLevel) && (m_level > (int)S_OFF))
                {
                    pSaverFunc p = m_modules[i->mModuleName];
                    (this->*(p))(m_level, m_level);
                }
            }
            else // power saver level change
            {
                int level = v.toInt();
                if (level != i->mLevel)
                {
                    m_strategy[i->mModuleName] = QVariant(i->mLevel);
                    pSaverFunc p = m_modules[i->mModuleName];
                    if ((m_level > (int)S_OFF) && (level != m_level))
                    {
                        (this->*(p))(m_level, m_level);
                    }
                    // Adjust the m_levelFuncMap
                    {
                        QMap<PowerSaverLevel, pSaverFunc>::iterator it = m_levelFuncMap.begin();
                        while (it != m_levelFuncMap.end())
                        {
                            QMap<PowerSaverLevel, pSaverFunc>::iterator prev = it;
                            ++it;
                            if (prev.value() == p)
                            {
                                m_levelFuncMap.erase(prev);
                                break;
                            }
                        }
                        m_levelFuncMap.insertMulti((PowerSaverLevel)i->mLevel, p);
                    }
                }
            }
        }
    }
    return ret;
}

void PowerSaverManagerAdaptorPrivate::saveDefaultStrategy()
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
    m_strategy.insert("connmanCellular", QVariant(int(S0)));
    m_strategy.insert("connmanWifi", QVariant(int(S0)));
    /* mce is a special module, it need change value for different level
       level is marked by the list's index
     */
    // TODO: We not know the level of backlight and sleeptime, we need Adjust the following numbers
    QList<QVariant> backlight;
    backlight.append(QVariant(0));
    backlight.append(QVariant(60));
    backlight.append(QVariant(123));
//    backlight.append(QVariant(190));
    backlight.append(QVariant(255));
    m_strategy.insert("MCEBacklight", QVariant(backlight));
    QList<QVariant> sleeptime;
    sleeptime.append(QVariant(13));
    sleeptime.append(QVariant(28));
//    sleeptime.append(QVariant(58));
    sleeptime.append(QVariant(298));
    sleeptime.append(QVariant(598));
    m_strategy.insert("MCESleeptime", QVariant(sleeptime));
    m_powerStorage->updateStorage(m_strategy, PowerSaverManagerStorageJson::Strategy);
}

void PowerSaverManagerAdaptorPrivate::initModules(void)
{
//    pSaverFunc a = &PowerSaverAdaptorPrivate::geoClueSaver;
//    (this->*a)(m_level);
    // Init module and func map
    m_modules.insert("geoClue", &PowerSaverManagerAdaptorPrivate::geoClueSaver);
    m_modules.insert("blueTooth", &PowerSaverManagerAdaptorPrivate::blueToothSaver);
    m_modules.insert("wallpaper", &PowerSaverManagerAdaptorPrivate::wallpaperSaver);
    m_modules.insert("ngfd", &PowerSaverManagerAdaptorPrivate::ngfdSaver);
    m_modules.insert("devMonitor", &PowerSaverManagerAdaptorPrivate::devMonitorSaver);
    m_modules.insert("kernel", &PowerSaverManagerAdaptorPrivate::kernelSaver);
    m_modules.insert("sysMgr", &PowerSaverManagerAdaptorPrivate::sysMgrSaver);
    m_modules.insert("lockScreen", &PowerSaverManagerAdaptorPrivate::lockScreenSaver);
    m_modules.insert("ofono", &PowerSaverManagerAdaptorPrivate::ofonoSaver);
    m_modules.insert("MCEBacklight", &PowerSaverManagerAdaptorPrivate::MCEBacklightSaver);
    m_modules.insert("MCESleeptime", &PowerSaverManagerAdaptorPrivate::MCESleepTimeSaver);
    m_modules.insert("connmanCellular", &PowerSaverManagerAdaptorPrivate::connmanCellularSaver);
    m_modules.insert("connmanWifi", &PowerSaverManagerAdaptorPrivate::connmanWifiSaver);
}

void PowerSaverManagerAdaptorPrivate::initFuncList(void)
{
    // Init the level func map. This map used to quick find the saver function of module
    // At present, the data in m_modules and m_strategy can provide the data to
    QMapIterator<QString, pSaverFunc> i(m_modules);
    while(i.hasNext())
    {
        i.next();
        QVariant q = m_strategy.value(i.key());
        qDebug() << q.type();
        if (q.type() == QVariant::Int || q.type() == QVariant::Double)
        {
            m_levelFuncMap.insertMulti((PowerSaverLevel)q.toInt(), i.value());
        }        
        else if (q.type() == QVariant::List)
        {
            for (int l = (int)S0; l <= (int)S_END; l++)
            {
                m_levelFuncMap.insertMulti((PowerSaverLevel)l, i.value());
            }
        }
        else
        {
            qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO << "Wrong type of m_strategy :" << i.key() << i.value();
        }
    }
#if 0
    m_levelFuncMap.insertMulti(S0, &PowerSaverManagerAdaptorPrivate::geoClueSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverManagerAdaptorPrivate::blueToothSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverManagerAdaptorPrivate::wallpaperSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverManagerAdaptorPrivate::ngfdSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverManagerAdaptorPrivate::devMonitorSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverManagerAdaptorPrivate::kernelSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverManagerAdaptorPrivate::sysMgrSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverManagerAdaptorPrivate::lockScreenSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverManagerAdaptorPrivate::ofonoSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverManagerAdaptorPrivate::connmanSaver);
    m_levelFuncMap.insertMulti(S0, &PowerSaverManagerAdaptorPrivate::MCESaver);
#endif
}

void PowerSaverManagerAdaptorPrivate::initStrategy()
{

}

void PowerSaverManagerAdaptorPrivate::getStrategy()
{
    m_strategy = m_powerStorage->loadStorage(PowerSaverManagerStorageJson::Strategy);
    Q_ASSERT(!m_strategy.isEmpty());
}

bool PowerSaverManagerAdaptorPrivate::applyPowerSaving(PowerSaverLevel level)
{
    bool ret = true;
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO << "going to setPowerSaving " << level;

    int delta = level - m_level;

    if (delta == 0)
    {
        qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO << "The same level value!\n";
    }
    else
    {
        if (delta > 0)
        {
            // When level of power saver increase, we need rerecord the pre setting in every xxxSaver
            for(int i = 1; i <= delta; i++)
            {
                QList<pSaverFunc> values = m_levelFuncMap.values((PowerSaverLevel)(i + m_level));
                for (int j = 0; j < values.size(); ++j)
                {
                    (this->*(values.at(j)))(level, m_level);
                }
            }
        }
        else
        {
            // When level of power saver decrease, we need revert the pre setting in every xxxSaver
            for(int i = 0; i > delta; i--)
            {
                QList<pSaverFunc> values = m_levelFuncMap.values((PowerSaverLevel)(i + m_level));
                for (int j = 0; j < values.size(); ++j)
                {
                    (this->*(values.at(j)))(level, m_level);
                }
            }
        }

        if (m_level == S_OFF)
        {
            Q_ASSERT(m_preSetting.count() > 0);
            m_powerStorage->updateStorage(m_strategy, PowerSaverManagerStorageJson::PrevSetting);
        }
        if (level == S_OFF)
        {
            Q_ASSERT(m_preSetting.empty());
            m_powerStorage->deleteStorage(PowerSaverManagerStorageJson::PrevSetting);
        }
        // We will send signal to inform the change of powersaver
        m_level = level;
        // TODO: send signal
        m_strategy["level"] = QVariant(int(m_level));
        m_powerStorage->updateStorage(m_strategy, PowerSaverManagerStorageJson::Strategy);
    }

    return ret;
}

void PowerSaverManagerAdaptorPrivate::geoClueSaver(PowerSaverLevel level, PowerSaverLevel preLevel)
{
    Q_UNUSED(level);
    Q_UNUSED(preLevel);
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to geoClueSaver!\n";
}

void PowerSaverManagerAdaptorPrivate::blueToothSaver(PowerSaverLevel level, PowerSaverLevel preLevel)
{
    Q_UNUSED(level);
    Q_UNUSED(preLevel);
}

void PowerSaverManagerAdaptorPrivate::wallpaperSaver(PowerSaverLevel level, PowerSaverLevel preLevel)
{
    Q_UNUSED(level);
    Q_UNUSED(preLevel);
}

void PowerSaverManagerAdaptorPrivate::ngfdSaver(PowerSaverLevel level, PowerSaverLevel preLevel)
{
    Q_UNUSED(level);
    Q_UNUSED(preLevel);
}

void PowerSaverManagerAdaptorPrivate::devMonitorSaver(PowerSaverLevel level, PowerSaverLevel preLevel)
{
    Q_UNUSED(level);
    Q_UNUSED(preLevel);
}

void PowerSaverManagerAdaptorPrivate::kernelSaver(PowerSaverLevel level, PowerSaverLevel preLevel)
{
    Q_UNUSED(level);
    Q_UNUSED(preLevel);
}

void PowerSaverManagerAdaptorPrivate::sysMgrSaver(PowerSaverLevel level, PowerSaverLevel preLevel)
{
    Q_UNUSED(level);
    Q_UNUSED(preLevel);
}

void PowerSaverManagerAdaptorPrivate::lockScreenSaver(PowerSaverLevel level, PowerSaverLevel preLevel)
{
    Q_UNUSED(level);
    Q_UNUSED(preLevel);
}

void PowerSaverManagerAdaptorPrivate::ofonoSaver(PowerSaverLevel level, PowerSaverLevel preLevel)
{
    Q_UNUSED(level);
    Q_UNUSED(preLevel);
}

void PowerSaverManagerAdaptorPrivate::connmanWifiSaver(PowerSaverLevel level, PowerSaverLevel preLevel)
{
    QString sName  = "net.connman";
    QString sPath  ="/net/connman/technology/wifi";
    QString sInterface = "net.connman.Technology";

    // get the corrsponding level of wifi
    int l = (PowerSaverLevel)m_strategy["connmanWifi"].toInt();
    bool value = false;
    bool current = false;

    // 1. record current wifi on/off status
    {
        QDBusMessage msg = QDBusMessage::createMethodCall(
                               sName,
                               sPath,
                               sInterface,
                               QStringLiteral("GetProperties"));
        QDBusReply<QVariantMap>reply = QDBusConnection::systemBus().call(msg);
        if(!reply.isValid())
        {
            qDebug() << "------------------" << __FILE__ << __LINE__ << "get wifi status: " << reply.error();
        }
        else
        {
            current = reply.value().value("Powered").toBool();
            qDebug() << "------------------" << __FILE__ << __LINE__ << "get wifi status: " << current;
            if (preLevel == S_OFF)
            {
                m_preSetting["connmanWifi"] = reply.value().value("Powered");
            }
        }
    }

    // decide which value to set
    if (level >= l)
    {
        value = true;
    }
    else
    {
        value = m_preSetting["connmanWifi"].toBool();
    }
    // set value to wifi
    if (value != current)
    {
        QDBusMessage setMsg = QDBusMessage::createMethodCall(
                               sName,
                               sPath,
                               sInterface,
                               QStringLiteral("SetProperty"));
        QList<QVariant> argList;
        argList << QVariant::fromValue(QString("Powered"));
        argList << QVariant::fromValue(QDBusVariant(QVariant(value)));
        setMsg.setArguments(argList);
        QDBusMessage response = QDBusConnection::systemBus().call(setMsg);
        if(response.type() == QDBusMessage::ErrorMessage)
        {
            qDebug() << "------------------" << __FILE__ << __LINE__ << "set wifi status: " << response.errorMessage();
        }
    }
    // remove "connmanWifi" from m_preSetting
    if (level == S_OFF)
    {
        m_preSetting.remove("connmanWifi");
    }
}

void PowerSaverManagerAdaptorPrivate::connmanCellularSaver(PowerSaverLevel level, PowerSaverLevel preLevel)
{
    QString sName  = "net.connman";
    QString sPath  ="/net/connman/technology/cellular";
    QString sInterface = "net.connman.Technology";

    // get the corrsponding level of mobile newwork
    int l = (PowerSaverLevel)m_strategy["connmanCellular"].toInt();
    bool value = false;
    bool current = false;

    // 1. record current wifi on/off status
    {
        QDBusMessage msg = QDBusMessage::createMethodCall(
                               sName,
                               sPath,
                               sInterface,
                               QStringLiteral("GetProperties"));
        QDBusReply<QVariantMap>reply = QDBusConnection::systemBus().call(msg);
        if(!reply.isValid())
        {
            qDebug() << "------------------" << __FILE__ << __LINE__ << "get cellular status: " << reply.error();
        }
        else
        {
            current = reply.value().value("MobileEnable").toBool();
            qDebug() << "------------------" << __FILE__ << __LINE__ << "get cellular status: " << current;
            if (preLevel == S_OFF)
            {
                m_preSetting["connmanCellular"] = reply.value().value("MobileEnable");
            }
        }
    }

    // decide which value to set
    if (level >= l)
    {
        value = true;
    }
    else
    {
        value = m_preSetting["connmanCellular"].toBool();
    }
    // set value to cellular
    if (value != current)
    {
        QDBusMessage setMsg = QDBusMessage::createMethodCall(
                               sName,
                               sPath,
                               sInterface,
                               QStringLiteral("SetProperty"));
        QList<QVariant> argList;
        argList << QVariant::fromValue(QString("MobileNetwork"));
        argList << QVariant::fromValue(QDBusVariant(QVariant(value ? QString("11") : QString("10"))));
        setMsg.setArguments(argList);
        QDBusMessage response = QDBusConnection::systemBus().call(setMsg);
        if(response.type() == QDBusMessage::ErrorMessage)
        {
            qDebug() << "------------------" << __FILE__ << __LINE__ << "set cellular status: " << response.errorMessage();
        }
    }
    // remove "connmanWifi" from m_preSetting
    if (level == S_OFF)
    {
        m_preSetting.remove("connmanCellular");
    }
}

void PowerSaverManagerAdaptorPrivate::MCEBacklightSaver(PowerSaverLevel level, PowerSaverLevel preLevel)
{
    QString sName  = "com.syberos.mce";
    QString sPath  ="/com/syberos/mce/request";
    QString sInterface = "com.syberos.mce.request";

    // get the corrsponding value of level
    QList<QVariant> l = m_strategy["MCEBacklight"].toList();
    int value = int(l[(int)level].toDouble());
    bool adjust = false;

    if (level > preLevel) // open power saver
    {
        // Record the prev value before power saver enable
        if (preLevel == S_OFF)
        {
            QList<QVariant> preSetting;
            // 1. get backlight dimming status, it record in presetting[0]
            {
                QDBusMessage msg = QDBusMessage::createMethodCall(
                                       sName,
                                       sPath,
                                       sInterface,
                                       QStringLiteral("get_config"));
                QList<QVariant> argList;
                argList << QVariant::fromValue(QDBusObjectPath("/system/osso/dsm/display/use_adaptive_display_dimming"));
                msg.setArguments(argList);
                QDBusReply<QVariant> r = QDBusConnection::systemBus().call(msg);
                if (!r.isValid())
                {
                    qDebug() <<"------------------"<<__FILE__ << __LINE__ << " get MCE display dimming Fail:" << r.error();
                }
                else
                {
                    preSetting[0] = r.value();
                }
            }
            // 2. Off backlight dimming
            {
                QDBusMessage msg = QDBusMessage::createMethodCall(
                                       sName,
                                       sPath,
                                       sInterface,
                                       QStringLiteral("set_config"));
                QList<QVariant> argList;
                argList << QVariant::fromValue(QDBusObjectPath("/system/osso/dsm/display/use_adaptive_display_dimming"));
                argList << QVariant::fromValue(QDBusVariant(false));
                msg.setArguments(argList);
                QDBusReply<QVariant> r = QDBusConnection::systemBus().call(msg);
                if (!r.isValid())
                {
                    qDebug() <<"------------------"<<__FILE__ << __LINE__ << " off MCE display dimming Fail:" << r.error();
                }
            }
            // 3. record current brightness. This is the max value we can set in power saver. It is in preSetting[1]
            {
                QDBusMessage msg = QDBusMessage::createMethodCall(
                                       sName,
                                       sPath,
                                       sInterface,
                                       QStringLiteral("get_config"));
                QList<QVariant> argList;
                argList << QVariant::fromValue(QDBusObjectPath("/system/osso/dsm/display/display_brightness"));
                msg.setArguments(argList);
                QDBusReply<QVariant> r = QDBusConnection::systemBus().call(msg);
                if (r.isValid())
                {
                    preSetting[1] = r.value();
                    m_preSetting["MCEBacklight"] = QVariant(preSetting);
                }
                else
                {
                    qDebug() <<"------------------"<<__FILE__ << __LINE__ << " get MCE brightness Fail:" << r.error();
                }
            }
        }
        QList<QVariant> tmplist = m_preSetting["MCEBacklight"].toList();
        int tmp = tmplist[1].toInt();
        if (value < tmp)
        {
            adjust = true;
        }
    }
    else
    {
        // Revert the prev value after power saver disable
        QList<QVariant> tmplist = m_preSetting["MCEBacklight"].toList();
        int tmp = tmplist[1].toInt();
        adjust = true;
        if (value > tmp)
        {
            value = tmp;
        }
        if (level == S_OFF)
        {
            value = tmp;
        }
    }
    if (adjust)
    {
        QDBusMessage blMsg = QDBusMessage::createMethodCall(
                                  sName,
                                  sPath,
                                  sInterface,
                                  QStringLiteral("set_config"));
        QList<QVariant> argList;
        argList << QVariant::fromValue(QDBusObjectPath("/system/osso/dsm/display/display_brightness"));
        argList << QVariant::fromValue(QDBusVariant(value));
        blMsg.setArguments(argList);
        QDBusReply<QVariant> reply = QDBusConnection::systemBus().call(blMsg);

        if(reply.isValid())
        {
            qDebug() <<"------------------"<<__FILE__ << __LINE__ << " set MCE brightness" << reply.value().toInt();
        }
        else
        {
            qDebug() <<"------------------"<<__FILE__ << __LINE__ << " set MCE brightness FAIL:" << reply.error();
        }
    }
    // recover dimming
    QList<QVariant> tmplist = m_preSetting["MCEBacklight"].toList();
    bool tmp = tmplist[0].toBool();
    if (level == S_OFF && tmp)
    {
        QDBusMessage msg = QDBusMessage::createMethodCall(
                               sName,
                               sPath,
                               sInterface,
                               QStringLiteral("set_config"));
        QList<QVariant> argList;
        argList << QVariant::fromValue(QDBusObjectPath("/system/osso/dsm/display/use_adaptive_display_dimming"));
        argList << QVariant::fromValue(QDBusVariant(tmp));
        msg.setArguments(argList);
        QDBusReply<QVariant> r = QDBusConnection::systemBus().call(msg);
        if (!r.isValid())
        {
            qDebug() <<"------------------"<<__FILE__ << __LINE__ << " revert MCE display dimming Fail:" << r.error();
        }
    }
    // remove "MCE_backlight" from m_preSetting
    if (level == S_OFF)
    {
        m_preSetting.remove("MCEBacklight");
    }
}

void PowerSaverManagerAdaptorPrivate::MCESleepTimeSaver(PowerSaverLevel level, PowerSaverLevel preLevel)
{
    QString sName  = "com.syberos.mce";
    QString sPath  ="/com/syberos/mce/request";
    QString sInterface = "com.syberos.mce.request";

    // get the corrsponding value of level
    QList<QVariant> l = m_strategy["MCESleeptime"].toList();
    int value = int(l[(int)level].toInt());
    bool adjust = false;

    if (level > preLevel) // open power saver
    {
        // Record the prev value before power saver enable
        if (preLevel == S_OFF)
        {
            // 1. record current sleeptime. This is the max value we can set in power saver. It is in preSetting[1]
            {
                QDBusMessage msg = QDBusMessage::createMethodCall(
                                       sName,
                                       sPath,
                                       sInterface,
                                       QStringLiteral("get_config"));
                QList<QVariant> argList;
                argList << QVariant::fromValue(QDBusObjectPath("/system/osso/dsm/display/display_dim_timeout"));
                msg.setArguments(argList);
                QDBusReply<QVariant> r = QDBusConnection::systemBus().call(msg);
                if (r.isValid())
                {
                    m_preSetting["MCESleeptime"] = r;
                    qDebug() <<"------------------"<<__FILE__ << __LINE__ << " get MCE sleep time :" << r.value().toInt();
                }
                else
                {
                    qDebug() <<"------------------"<<__FILE__ << __LINE__ << " get MCE sleep time Fail:" << r.error();
                }
            }
        }
        int tmp = m_preSetting["MCESleeptime"].toInt();
        if (value < tmp)
        {
            adjust = true;
        }
    }
    else
    {
        // Revert the prev value after power saver disable
        int tmp = m_preSetting["MCESleeptime"].toInt();
        adjust = true;
        if (value > tmp)
        {
            value = tmp;
        }
        if (level == S_OFF)
        {
            value = tmp;
        }
    }
    if (adjust)
    {
        QDBusMessage blMsg = QDBusMessage::createMethodCall(
                                  sName,
                                  sPath,
                                  sInterface,
                                  QStringLiteral("set_config"));
        QList<QVariant> argList;
        argList << QVariant::fromValue(QDBusObjectPath("/system/osso/dsm/display/display_dim_timeout"));
        argList << QVariant::fromValue(QDBusVariant(value));
        blMsg.setArguments(argList);
        QDBusReply<QVariant> reply = QDBusConnection::systemBus().call(blMsg);

        if(reply.isValid())
        {
            qDebug() <<"------------------"<<__FILE__ << __LINE__ << " set MCE sleep time" << reply.value().toInt();
        }
        else
        {
            qDebug() <<"------------------"<<__FILE__ << __LINE__ << " set MCE sleep time FAIL:" << reply.error();
        }
    }
    // remove "MCE_sleeptime" from m_preSetting
    if (level == S_OFF)
    {
        m_preSetting.remove("MCESleeptime");
    }
}



