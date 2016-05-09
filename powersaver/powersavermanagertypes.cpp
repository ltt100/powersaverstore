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

#include "powersavermanagertypes.h"

/* send the ObjectPathProperties data into a D-Bus argument */
QDBusArgument &operator<<(QDBusArgument &argument, const powerSaverManagerStrategy &obj)
{
    argument.beginStructure();
    argument << obj.mModuleName << obj.mLevel << obj.mValue;
    argument.endStructure();
    return argument;
}

/* Retrieve the ObjectPathProperties data from the D-Bus argument */
const QDBusArgument &operator>>(const QDBusArgument &argument, powerSaverManagerStrategy &obj)
{
    argument.beginStructure();
    argument >> obj.mModuleName >> obj.mLevel >> obj.mValue;
    argument.endStructure();
    return argument;
}

