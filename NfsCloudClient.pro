#-------------------------------------------------
#
# Project created by QtCreator 2016-01-20T10:00:38
#
#-------------------------------------------------

QT       += core gui network script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


#target folder
TARGET = cdos-desktop-cloud 
TEMPLATE = app
DESTDIR = bin/
UI_DIR = forms
RCC_DIR = tmp
MOC_DIR = tmp
OBJECTS_DIR = tmp

#lang
TRANSLATIONS += lang/lang_zh_CN.ts\
                lang/lang_en_US.ts
#source
SOURCES += main.cpp\
    homewindow.cpp \
    vmwidget.cpp \
    logindialog.cpp \
    common.cpp \
    mytitlebar.cpp \
    waitdialog.cpp \
    worker.cpp \
    detaildialog.cpp \
    helppage.cpp \
    updatedialog.cpp

HEADERS  += \
    homewindow.h \
    simple_log.h \
    vmwidget.h \
    logindialog.h \
    common.h \
    mytitlebar.h \
    waitdialog.h \
    worker.h \
    detaildialog.h \
    helppage.h \
    updatedialog.h

FORMS    += \
    vmwidget.ui \
    logindialog.ui \
    homewindow.ui \
    detaildialog.ui \
    helppage.ui \
    helppage.ui \
    updatedialog.ui

RESOURCES += \
    resource.qrc
RC_FILE = res.rc
#CONFIG +=c++11
CONFIG += static
QMAKE_CXXFLAGS_DEBUG += -std=c++11 -static-libgcc 
QMAKE_CXXFLAGS_RELEASE += -std=c++11 -static-libgcc 
DISTFILES +=
