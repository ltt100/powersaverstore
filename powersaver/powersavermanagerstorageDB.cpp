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

#include "powersavermanagerstorageDB.h"

#include <QDir>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>

#if 0 /* We will use following code to test database APIs */
#define POWERSAVER_DB_PATH "/data/systemservice/powersaver/"
#else
#define POWERSAVER_DB_PATH "/home/u4/powersaver/"
#endif
#define POWERSAVER_DB_NAME "powersaverstrategy.db"
#define POWERSAVER_CONN_NAME "syberospowersaver"
#define POWERSAVER_TABLE_NAME "powersaverstrategy"


#define CREATE_TABLE  \
    "CREATE TABLE IF NOT EXISTS %1(" \
    "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," \
    "module TEXT DEFAULT NULL," \
    "level INTERGER DEFAULT 0," \
    "value INTERGER DEFAULT 0)"

#define QUERY_TABLE "SELECT count(*) from sqlite_master where type='table' and name='%1';"
#define INSERT_RECORD "INSERT INTO '%s' ('module', 'level', 'value') VALUES '%s'"

#define UPDATE_RECORD_VALUE "UPDATE '%s' SET 'value' = '%d' WHERE module = '%s' AND level = '%d' "
#define UPDATE_RECORD_LEVEL "UPDATE '%s' SET 'level' = '%d' WHERE module = '%s' "

#define GET_RECORDS "SELECT * FROM %s"

PowerSaverMangerStorage::PowerSaverMangerStorage(QObject *parent) :
    QObject(parent)
{
    initDB();
}

bool PowerSaverMangerStorage::updateStrategy(const QMap<QString, QVariant> & strategy)
{
    bool ret = false;
    Q_UNUSED(strategy);
    return ret;
}

bool PowerSaverMangerStorage::isTableExist()
{
    QSqlQuery query(m_db);
    bool ret = false;
    return ret;
}

QMap<QString, QVariant> PowerSaverMangerStorage::loadStrategy()
{
    QMap<QString, QVariant> ret;
    return ret;
}

void PowerSaverMangerStorage::initDB()
{
    QDir dbDir(POWERSAVER_DB_PATH);
    if(!dbDir.exists()) {
        QDir tmp;
        if(!tmp.mkdir(POWERSAVER_DB_PATH)) {
            qDebug() <<  __FILE__  << Q_FUNC_INFO << __LINE__ <<"powersaver------------create database path failed!";
            return;
        }
    }

    QFile file(POWERSAVER_DB_NAME);
    if (!file.exists()) {
        file.open( QIODevice::ReadWrite | QIODevice::Text );
        file.close();
    }

    m_db  = QSqlDatabase::addDatabase("QSQLITE", POWERSAVER_CONN_NAME);
    if(!m_db.isValid()) {
        qDebug() <<  __FILE__  << Q_FUNC_INFO << __LINE__ << m_db.lastError().text();
        return;
    }

    QString baseName = QString(POWERSAVER_DB_PATH) + QString(POWERSAVER_DB_NAME);
    m_db.setDatabaseName(baseName);

    if(!m_db.open()) {
        qDebug() << __FILE__  << Q_FUNC_INFO << __LINE__ << m_db.lastError();
        qFatal( "Failed to connect." );
        return ;
    }

    qDebug() << "powersaver-----------end";
}

bool PowerSaverMangerStorage::createTable()
{
    QSqlQuery query(m_db);
    bool result = query.exec(QString(CREATE_TABLE).arg(POWERSAVER_TABLE_NAME));
    if(!result){
        qDebug() << "+++++++++++" << __FILE__  << Q_FUNC_INFO << __LINE__ << "FAILED:: can't creat table"
                << POWERSAVER_TABLE_NAME << m_db.lastError().text();
        return false;
    }
    else
    {
        return true;
    }
}

