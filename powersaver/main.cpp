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

#include <QCoreApplication>
#include <powersavermanageradaptorprivate.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    PowerSaverManagerAdaptorPrivate * tmAdaptorPrivate = new PowerSaverManagerAdaptorPrivate();
    Q_UNUSED(tmAdaptorPrivate);
    return a.exec();
}
