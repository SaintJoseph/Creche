INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT       += xml

CONFIG( debug, debug|release ) {
    DEFINES += DEBUG_COMANDSAVE
}


PUBLIC_HEADERS         += $$PWD/src/qextserialport.h \
                          $$PWD/src/qextserialenumerator.h \
                          $$PWD/src/qextserialport_global.h\
                          $$PWD/src/hled.h \
                          $$PWD/src/Compile.h \
                          $$PWD/src/newmodedialog.h \
                          $$PWD/src/ComArduinoDock.h \
                          $$PWD/src/savexmlfile.h \
                          $$PWD/src/application.h

HEADERS                += $$PUBLIC_HEADERS \
                          $$PWD/src/qextserialport_p.h \
                          $$PWD/src/qextserialenumerator_p.h \
                          $$PWD/src/horodateur.h \
                          $$PWD/func_name.h \
                          $$PWD/src/convertisseurdechexa.h

SOURCES                += $$PWD/src/qextserialport.cpp \
                          $$PWD/src/qextserialenumerator.cpp\
                          $$PWD/src/hled.cpp \
                          $$PWD/src/Compile.cpp \
                          $$PWD/src/newmodedialog.cpp \
                          $$PWD/src/ComArduinoDock.cpp \
                          $$PWD/src/savexmlfile.cpp \
                          $$PWD/src/horodateur.cpp \
                          $$PWD/src/application.cpp \
                          $$PWD/src/convertisseurdechexa.cpp

unix {
    SOURCES            += $$PWD/src/qextserialport_unix.cpp
    linux* {
        SOURCES        += $$PWD/src/qextserialenumerator_linux.cpp
    } else:macx {
        SOURCES        += $$PWD/src/qextserialenumerator_osx.cpp
    } else {
        SOURCES        += $$PWD/src/qextserialenumerator_unix.cpp
    }
}
win32:SOURCES          += $$PWD/src/qextserialport_win.cpp \
                          $$PWD/src/qextserialenumerator_win.cpp

linux*{
    !qesp_linux_udev:DEFINES += QESP_NO_UDEV
    qesp_linux_udev: LIBS += -ludev
}

macx:LIBS              += -framework IOKit -framework CoreFoundation
win32:LIBS             += -lsetupapi -ladvapi32 -luser32

FORMS    += \
    $$PWD/src/newmodedialog.ui

# moc doesn't detect Q_OS_LINUX correctly, so add this to make it work
linux*:DEFINES += __linux__

RESOURCES += \
    $$PWD/src/Icone.qrc

# Traduction pour la partie Communication et sauvegarde
TRANSLATIONS += $$PWD/src/ComAndSavePart_en.ts \
                $$PWD/src/ComAndSavePart_fr.ts \
                $$PWD/src/ComAndSavePart_nl.ts \
                $$PWD/src/ComAndSavePart_jp.ts
