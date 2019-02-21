#-------------------------------------------------
#
# Project created by QtCreator 2017-09-26T12:11:49
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TradeTool
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    LoginErrorDialog.cpp \
    LoginWindow.cpp \
    MdSpi.cpp \
    RiskCtrlThread.cpp \
    RiskCtrlWin.cpp \
    ShareOperation.cpp \
    ToolWin.cpp \
    TradeSpi.cpp \
    TradeWindow.cpp \
    Main.cpp \
    UpdateRiskCtrlWinUI.cpp \
    WatchDog.cpp \
    QueryPosiThread.cpp \
    MonitorPosiThread.cpp \
    MonitorWinThread.cpp

HEADERS += \
    ctpapi/ThostFtdcMdApi.h \
    ctpapi/ThostFtdcTraderApi.h \
    ctpapi/ThostFtdcUserApiDataType.h \
    ctpapi/ThostFtdcUserApiStruct.h \
    LoginErrorDialog.h \
    LoginWindow.h \
    MdSpi.h \
    RiskCtrlThread.h \
    RiskCtrlWin.h \
    ShareOperation.h \
    ToolWin.h \
    TradeSpi.h \
    TradeWindow.h \
    TypeDef.h \
    UpdateRiskCtrlWinUI.h \
    WatchDog.h \
    QueryPosiThread.h \
    MonitorPosiThread.h \
    ExitThread.h

FORMS += \
    LoginErrorDialog.ui \
    LoginWindow.ui \
    RiskCtrlWin.ui \
    ToolWin.ui \
    TradeWindow.ui

win32: LIBS += -L$$PWD/ctpapi/ -lthostmduserapi
win32: LIBS += -L$$PWD/ctpapi/ -lthosttraderapi

INCLUDEPATH += $$PWD/ctpapi
DEPENDPATH += $$PWD/ctpapi

RC_FILE += \
    icon.rc
