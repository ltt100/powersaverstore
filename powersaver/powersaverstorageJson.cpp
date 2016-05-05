#include "powersaverstorageJson.h"
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

#include "powersaverstorageJson.h"

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
#define POWERSAVER_STORAGE_NAME "powersaverstrategy"


PowerSaverStorageJson::PowerSaverStorageJson(PowerSaverStorageJson::SaveFormat format, QObject *parent) :
    QObject(parent), m_format(format)
{
    m_jsonExist = false;
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
    t.append(POWERSAVER_STORAGE_NAME).append(m_format == Json ? ".json" : ".dat");
    QFile file(t);
    if (file.exists())
    {
        m_jsonExist = true;
    }
}

bool PowerSaverStorageJson::isStrategyExist()
{
    return m_jsonExist;
}

bool PowerSaverStorageJson::updateStrategy(QMap<QString, QVariant> strategy)
{
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to update powersaver strategy!";

    bool ret = false;
    QString t = POWERSAVER_STORAGE_PATH;
    t.append(POWERSAVER_STORAGE_NAME).append(m_format == Json ? ".json" : ".dat");
    QFile saveFile(t);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qDebug() << "file save error:" << POWERSAVER_STORAGE_NAME;
        return false;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromVariant(strategy);
    if (!jsonDoc.isNull()) {
        ret = true;
        qDebug() << "Now print the save bin json:\n" << jsonDoc.toJson();
    }
    saveFile.write(m_format == Json
                   ? jsonDoc.toJson()
                   : jsonDoc.toBinaryData());
    saveFile.close();

    if (!m_jsonExist)
    {
        m_jsonExist = true;
    }

    return ret;
}

QMap<QString, QVariant> PowerSaverStorageJson::loadStrategy()
{
    qDebug() << __FILE__ << __LINE__ << Q_FUNC_INFO
                 << "going to load powersaver strategy!";

    QMap<QString, QVariant> ret;
    QString t = POWERSAVER_STORAGE_PATH;
    t.append(POWERSAVER_STORAGE_NAME).append(m_format == Json ? ".json" : ".dat");
    QFile file(t);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "file read error:" << POWERSAVER_STORAGE_NAME;
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

