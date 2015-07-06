#-------------------------------------------------
#
# Project created by QtCreator 2011-08-05T04:40:32
#
#-------------------------------------------------

QT       += core gui
QT += widgets

TARGET = Campaign_Creator
TEMPLATE = app

SOURCES += main.cpp\
	widget.cpp\
../mission_generator/mission_generator.cpp

HEADERS  += widget.h\
../mission_generator/mission_generator.h

FORMS    += widget.ui

RESOURCES += \
    resourceFiles.qrc
