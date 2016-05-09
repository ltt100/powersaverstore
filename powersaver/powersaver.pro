#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T14:10:28
#
#-------------------------------------------------

QT       += core dbus sql

QT       -= gui

system(qdbusxml2cpp -c PowerSaverManagerAdaptor -a powersavermanageradaptor.h:powersavermanageradaptor.cpp powersavermanager.xml -i powersavermanagertypes.h)

TARGET = powersavermanager
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
LIBS += -lrt
CONFIG += link_pkgconfig

SOURCES += main.cpp \
    powersavermanageradaptorprivate.cpp \
    powersavermanagertypes.cpp \
    powersavermanageradaptor.cpp \
    powersavermanagerstorageDB.cpp \
    powersavermanagerstorageJson.cpp

HEADERS += \
    powersavermanageradaptorprivate.h \
    powersavermanagertypes.h \
    powersavermanagerstorageJson.h \
    powersavermanageradaptor.h \
    powersavermanagerstorageDB.h

DISTFILES += \
    powersavermanager.xml
