QT -= gui
QT += core serialport sql mqtt widgets

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        athenticationclass.cpp \
        cypress_serial.cpp \
        database.cpp \
        defines.cpp \
        encryption/Base64.cpp \
        i2c.cpp \
        i2cmasterclass.cpp \
        jobhandler.cpp \
        main.cpp \
        mainclass.cpp \
        mqttclass.cpp \
        query.cpp \
        serial.cpp \
        tablehandler.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    athenticationclass.h \
    cypress_serial.h \
    database.h \
    defines.h \
    encryption/Base64.h \
    encryption/encrypt.h \
    encryption/vigenere.h \
    i2c.h \
    i2cmasterclass.h \
    jobhandler.h \
    mainclass.h \
    mqttclass.h \
    query.h \
    serial.h \
    tablehandler.h



INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./release/ -lwiringPi
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./debug/ -lwiringPi
else:unix: LIBS += -L$$PWD/./ -lwiringPi
