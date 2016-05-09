#include "powersavermanagerstorageJson.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonDocument>

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

#include "powersavermanagerstorageJson.h"

#include <QDir>
#include <QFile>
#include <QDebug>

// The macro used to test the JSON file write or not. When test end, we will use binary to record the json
//#define TESTJSONTEXT

#if 0 /* We will use following code to test database APIs */
#define POWERSAVER_STORAGE_PATH "/data/systemservice/powersaver/"
#else
#define POWERSAVER_STORAGE_PATH "/home/u4/powersaver/"
#endif
#define POWERSAVER_STRATEGY_NAME "powersaverstrategy"
#define POWERSAVER_PREVSETTING_NAME "powersaverprevsetting"


PowerSaverManagerStorageJson::PowerSaverManagerStorageJson(PowerSaverManagerStorageJson::SaveFormat format, QObject *parent) :
    QObject(parent), m_format(format)
{
    m_strategyExist = false;
    m_settingExist = false;
    QDir storageDir(POWERSAVER_STORAGE_PATH);
    if(!storageDir.exists()) {
        QDir tmp;
        if(!tmp.mkdir(POWERSAVER_STORAGE_PATH)) {
            qDebug() <<  __FILE__  << Q_FUNC_INFO << __LINE__ <<"powersaver------------create storage path failed!";
            return;
        }
    }

    // check json file exist or not. If not exist, save the default value to json.
    QString t = POWERSAVER_STORAGE_PATH;
    t.append(POWERSAVER_STRATEGY_NAME).append(m_format == Json ? ".json" : ".dat");
    QFile fileStrategy(t);
    if (fileStrategy.exists())
    {
        m_strategyExist = true;
    }

    QString s = POWERSAVER_STORAGE_PATH;
    s.append(POWERSAVER_PREVSETTING_NAME).append(m_format == Json ? ".json" : ".dat");
    QFile fileSetting(s);
    if (fileSetting.exists())
    {
        m_settingExist = true;
    }
}

bool PowerSaverManagerStorageJson::isStrategyExist()
{
    return m_strategyExist;
}

bool PowerSaverManagerStorageJson::isSettingExist()
{
    return m_settingExist;
}

bool PowerSaverManagerStorageJson::updateStorage(const QMap<QString, QVariant> & value, PowerSaverManagerStorageJson::Savetype type)
{
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to update powersaver strategy!";

    bool ret = false;
    QString t = POWERSAVER_STORAGE_PATH;
    t.append(type == Strategy ? POWERSAVER_STRATEGY_NAME: POWERSAVER_PREVSETTING_NAME).append(m_format == Json ? ".json" : ".dat");
    QFile saveFile(t);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qDebug() << "file save error:" << t;
        return false;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromVariant(value);
    if (!jsonDoc.isNull()) {
        ret = true;
        qDebug() << "Now print the save bin json:\n" << jsonDoc.toJson();
    }
    saveFile.write(m_format == Json
                   ? jsonDoc.toJson()
                   : jsonDoc.toBinaryData());
    saveFile.close();

    if (type == Strategy && !m_strategyExist)
    {
        m_strategyExist = true;
    }
    if (type == PrevSetting && !m_settingExist)
    {
        m_settingExist = true;
    }

    return ret;
}

QMap<QString, QVariant> PowerSaverManagerStorageJson::loadStorage(PowerSaverManagerStorageJson::Savetype type)
{
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to load powersaver storage!";

    QMap<QString, QVariant> ret;
    QString t = POWERSAVER_STORAGE_PATH;
    t.append(type == Strategy ? POWERSAVER_STRATEGY_NAME: POWERSAVER_PREVSETTING_NAME).append(m_format == Json ? ".json" : ".dat");
    QFile file(t);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "file read error:" << t;
    }
    QByteArray byteArray = file.readAll();
    file.close();

    QJsonDocument parseDoc = (m_format == Json
        ? QJsonDocument::fromJson(byteArray)
        : QJsonDocument::fromBinaryData(byteArray));
    qDebug() << "Now print the load json:\n" << parseDoc.toJson();
    QJsonObject jsonObj = parseDoc.object();
    qDebug() << "Now print the load json object:\n" << jsonObj;
    ret = jsonObj.toVariantMap();

    return ret;
}

bool PowerSaverManagerStorageJson::deleteStorage(PowerSaverManagerStorageJson::Savetype type)
{
    bool ret =  false;
    QString t = POWERSAVER_STORAGE_PATH;
    t.append(type == Strategy ? POWERSAVER_STRATEGY_NAME: POWERSAVER_PREVSETTING_NAME).append(m_format == Json ? ".json" : ".dat");
    QFile file(t);
    if (file.exists())
    {
        ret = file.remove();
    }
    if (ret)
    {
        (type == Strategy) ? m_strategyExist = false : m_settingExist = false;
    }
    return ret;
}
