QT += core network
QT -= gui

CONFIG += c++11

TARGET = PocketSphinxTCPServer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

target.path = /usr/local/bin/pstcpserver
INSTALLS += target

INCLUDEPATH += /usr/local/include/sphinxbase/
INCLUDEPATH += /usr/local/include/pocketsphinx/

LIBS += -lsphinxbase
LIBS += -lpocketsphinx

SOURCES += main.cpp \
    maintcpserver.cpp \
    recthread.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    maintcpserver.h \
    recthread.h
