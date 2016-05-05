#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T14:10:28
#
#-------------------------------------------------

QT       += core dbus sql

QT       -= gui

system(qdbusxml2cpp -c PowerSaverAdaptor -a powersaveradaptor.h:powersaveradaptor.cpp powersaver.xml -i powersavertypes.h)

TARGET = powersaver
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
LIBS += -lrt
CONFIG += link_pkgconfig

SOURCES += main.cpp \
    powersaveradaptor.cpp \
    powersaveradaptorprivate.cpp \
    powersavertypes.cpp \
    powersaverstorageDB.cpp \
    powersaverstorageJson.cpp

HEADERS += \
    powersaveradaptor.h \
    powersaveradaptorprivate.h \
    powersavertypes.h \
    powersaverstorageDB.h \
    powersaverstorageJson.h

DISTFILES += \
    powersaver.xml
