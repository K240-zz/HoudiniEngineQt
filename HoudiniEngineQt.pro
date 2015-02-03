#-------------------------------------------------
#
# Project created by QtCreator 2014-08-22T20:37:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HoudiniEngine
TEMPLATE = app


SOURCES += main.cpp mainwindow.cpp HAPI_cpp.cpp \
    parameters.cpp \
    parametersview.cpp \
    fileselector.cpp

HEADERS  += mainwindow.h \
    HAPI_cpp.h \
    parameters.h \
    parametersview.h \
    fileselector.h

FORMS    += mainwindow.ui

INCLUDEPATH += "$$(HOUDINI_ROOT)/toolkit/include"
LIBS += "$$(HOUDINI_ROOT)/custom/houdini/dsolib/libHAPI.a"

