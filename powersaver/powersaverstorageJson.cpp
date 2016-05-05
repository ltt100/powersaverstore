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

#ifdef TESTJSONTEXT
#define POWERSAVER_STORAGE_NAME_TEXT "powersaverstrategy.json"
#endif


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
/*
    if (!file.exists()) {
        file.open(QIODevice::ReadWrite);
        file.close();
    }
    else
    {
        m_jsonExist = true;
    }
*/
#ifdef TESTJSONTEXT
    QString ttext = POWERSAVER_STORAGE_PATH;
    ttext.append("/").append(POWERSAVER_STORAGE_NAME_TEXT);
    QFile filetext(ttext);
    if (!filetext.exists()) {
        filetext.open(QIODevice::ReadWrite | QIODevice::Text);
        filetext.close();
    }
#endif /* TESTJSONTEXT */
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
    m_jsonExist = true;

#ifdef TESTJSONTEXT
    QString ttext = POWERSAVER_STORAGE_PATH;
    ttext.append("/").append(POWERSAVER_STORAGE_NAME_TEXT);
    QFile saveFileText(ttext);
    if (!saveFileText.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "file open error:" << POWERSAVER_STORAGE_NAME_TEXT;
        return false;
    }
    QJsonDocument jsonDocText = QJsonDocument::fromVariant(strategy);
    if (!jsonDocText.isNull()) {
       qDebug() << "Now print the save text json:\n" << jsonDocText.toJson();
    }
    saveFileText.write(jsonDocText.toJson());
    saveFileText.close();
#endif /* TESTJSONTEXT */

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
#ifdef TESTJSONTEXT
    QString ttext = POWERSAVER_STORAGE_PATH;
    ttext.append("/").append(POWERSAVER_STORAGE_NAME_TEXT);
    QFile filetext(ttext);
    if (!filetext.open(QIODevice::ReadOnly))
    {
        qDebug() << "file read error:" << POWERSAVER_STORAGE_NAME_TEXT;
    }
    QByteArray byteArrayText = filetext.readAll();
    filetext.close();
#endif /* TESTJSONTEXT */

    QJsonDocument parseDoc = (m_format == Json
        ? QJsonDocument::fromJson(byteArray)
        : QJsonDocument::fromBinaryData(byteArray));
    qDebug() << "Now print the load json:\n" << parseDoc.toJson();
    QJsonObject jsonObj = parseDoc.object();
    ret = jsonObj.toVariantMap();

#ifdef TESTJSONTEXT
    QJsonParseError error;
    QJsonDocument parseDocText = QJsonDocument::fromJson(byteArrayText, &error);
    if (error.error == QJsonParseError::NoError)
    {
        QMap<QString, QVariant> retTemp;
        QJsonObject jsonObjText = parseDocText.object();
        retTemp = jsonObjText.toVariantMap();
    }
#endif /* TESTJSONTEXT */

    return ret;
}

