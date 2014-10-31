#-------------------------------------------------
#
# Project created by QtCreator 2014-09-27T21:35:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simplifiedDBBased
TEMPLATE = app

include(ComAndSavePart.pri)

SOURCES += main.cpp\
        amainwindow.cpp \
    lightsdatabase.cpp \
    keysdatabase.cpp \
    qcolor_extension.cpp

HEADERS  += amainwindow.h \
    lightsdatabase.h \
    keysdatabase.h \
    settings.h \
    qcolor_extension.h \
    qlist_sort.h \
    autilities.h

FORMS    += amainwindow.ui
