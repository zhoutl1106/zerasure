TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -lemon -O3 -msse4 -mavx
QMAKE_CFLAGS += -lemon -O3 -msse4
QMAKE_CC = gcc
QMAKE_CXX = g++

SOURCES += \
    Jerasure-1.2A/galois.c \
    Jerasure-1.2A/jerasure.c \
    Jerasure-1.2A/reed_sol.c \
    Jerasure-1.2A/cauchy.c \ 
    main.cpp \
    Search/zabstractelement.cpp \
    Search/zxorelement.cpp \
    Search/zabstractgenetic.cpp \
    Search/zxorgenetic.cpp \
    utils.cpp

HEADERS += \
    Jerasure-1.2A/galois.h \
    Jerasure-1.2A/jerasure.h \
    Jerasure-1.2A/reed_sol.h \
    Jerasure-1.2A/cauchy.h \
    Search/zabstractelement.h \
    Search/zxorelement.h \
    Search/zabstractgenetic.h \
    Search/zxorgenetic.h \
    utils.h

DISTFILES += \
    README.md
