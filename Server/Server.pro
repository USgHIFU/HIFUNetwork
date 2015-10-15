#-------------------------------------------------
#
# Project created by QtCreator 2015-08-20T13:05:30
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = Server
TEMPLATE = lib

DEFINES += SERVER_LIBRARY

INCLUDEPATH += ../lib/common

SOURCES += server.cpp

HEADERS += server.h\
        server_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
