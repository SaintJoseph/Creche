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
        IDE/amainwindow.cpp \
        IDE/lightsdatabase.cpp \
        IDE/keysdatabase.cpp \
        IDE/qcolor_extension.cpp

HEADERS  += IDE/amainwindow.h \
         IDE/lightsdatabase.h \
         IDE/keysdatabase.h \
         IDE/settings.h \
         IDE/qcolor_extension.h \
         IDE/qlist_sort.h \
         IDE/autilities.h

FORMS    += IDE/amainwindow.ui
