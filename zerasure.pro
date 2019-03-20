TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -lemon -O3 -msse4 -mavx -I../lemon-1.3.1/ -I../lemon-1.3.1/build
QMAKE_CFLAGS += -lemon -O3 -msse4
QMAKE_CC = gcc
QMAKE_CXX = g++

SOURCES += \
    Jerasure-1.2A/galois.c \
    Jerasure-1.2A/jerasure.c \
    Jerasure-1.2A/reed_sol.c \
    Jerasure-1.2A/cauchy.c \ 
    main.cpp \
    utils.cpp \
    Example/zexample.cpp \
    Search/zelement.cpp \
    Search/zgenetic.cpp \
    Search/zrandomarray.cpp \
    Algorithm/zoxc.cpp \
    Search/zsimulatedannealing.cpp

HEADERS += \
    Jerasure-1.2A/galois.h \
    Jerasure-1.2A/jerasure.h \
    Jerasure-1.2A/reed_sol.h \
    Jerasure-1.2A/cauchy.h \
    utils.h \
    Example/zexample.h \
    Search/zelement.h \
    Search/zgenetic.h \
    Search/zrandomarray.h \
    Algorithm/zoxc.h \
    Search/zsimulatedannealing.h

DISTFILES += \
    README.md \
    mfile
