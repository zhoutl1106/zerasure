TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -lemon -O3 -msse4
QMAKE_CFLAGS += -lemon -O3 -msse4
QMAKE_CC = gcc
QMAKE_CXX = g++

SOURCES += \
    Jerasure-1.2A/galois.c \
    Jerasure-1.2A/jerasure.c \
    Jerasure-1.2A/reed_sol.c \
    Jerasure-1.2A/cauchy.c \ 
    main.cpp \
    Jerasure-1.2A/cauchy_best_r6.c \
    Jerasure-1.2A/cauchy.c \
    Jerasure-1.2A/galois.c \
    Jerasure-1.2A/jerasure.c \
    Jerasure-1.2A/reed_sol.c

HEADERS += \
    Jerasure-1.2A/galois.h \
    Jerasure-1.2A/jerasure.h \
    Jerasure-1.2A/reed_sol.h \
    Jerasure-1.2A/cauchy.h \ 
    Jerasure-1.2A/cauchy.h \
    Jerasure-1.2A/galois.c.GiuGYS \
    Jerasure-1.2A/galois.h \
    Jerasure-1.2A/jerasure.h \
    Jerasure-1.2A/reed_sol.h

DISTFILES += \
    README.md
