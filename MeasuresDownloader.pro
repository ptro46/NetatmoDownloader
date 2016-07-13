QT += core network sql
QT -= gui

unix:DEFINES += USE_OPENSSL TARGET_DESKTOP

CONFIG += c++11

TARGET = MeasuresDownloader
CONFIG += console
CONFIG -= app_bundle
LIBS += -lz -lcrypto -lssl

TEMPLATE = app

SOURCES += main.cpp \
    BotLogs.cpp \
    Config.cpp \
    DeviceMeasures.cpp \
    SortFunctions.cpp \
    NetatmoAuthWS.cpp \
    NetatmoGetDeviceMeasuresWS.cpp \
    NetatmoGetModuleMeasuresWS.cpp \
    NetatmoGetPluviometrieWS.cpp \
    NetatmoGetStationsDataWS.cpp \
    TaskBotGetMeasures.cpp \
    TaskMainDeviceMeasures.cpp \
    TaskOutdoorModuleMeasures.cpp \
    TaskPluviometreMeasures.cpp \
    NetatmoGetWindWS.cpp \
    NetatmoGetModuleAdditionnelWS.cpp \
    TaskWindMeasures.cpp \
    TaskModuleAdditionnelMeasures.cpp \
    AbstractWS.cpp \
    NADevice.cpp \
    NAModule.cpp

HEADERS += \
    BotLogs.h \
    Config.h \
    DeviceMeasures.h \
    SortFunctions.h \
    NetatmoAuthWS.h \
    NetatmoGetDeviceMeasuresWS.h \
    NetatmoGetModuleMeasuresWS.h \
    NetatmoGetPluviometrieWS.h \
    NetatmoGetStationsDataWS.h \
    TaskBotGetMeasures.h \
    NetatmoGetWindWS.h \
    NetatmoGetModuleAdditionnelWS.h \
    AbstractWS.h \
    NADevice.h \
    NAModule.h

DISTFILES += \
    config.json \
    getStationsDatas.json \
    config_mycke.json \
    config_ptro.json \
    config_wind.json
