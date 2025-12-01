QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# Para que la consola permanezca abierta en Windows
win32:CONFIG += console

SOURCES += \
    main.cpp \
    particle.cpp \
    obstacle.cpp \
    box.cpp \
    simulator.cpp

HEADERS += \
    particle.h \
    obstacle.h \
    box.h \
    simulator.h

# Directorio de salida
DESTDIR = $$PWD

# Para debugging
DEFINES += QT_MESSAGELOGCONTEXT
