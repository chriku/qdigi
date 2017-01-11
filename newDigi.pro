#-------------------------------------------------
#
# Project created by QtCreator 2016-12-16T14:11:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = btd
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    digiview.cpp \
    lapi.c \
    lauxlib.c \
    lbaselib.c \
    lbitlib.c \
    lcode.c \
    lcorolib.c \
    lctype.c \
    ldblib.c \
    ldebug.c \
    ldo.c \
    ldump.c \
    lfunc.c \
    lgc.c \
    linit.c \
    liolib.c \
    llex.c \
    lmathlib.c \
    lmem.c \
    loadlib.c \
    lobject.c \
    lopcodes.c \
    loslib.c \
    lparser.c \
    lstate.c \
    lstring.c \
    lstrlib.c \
    ltable.c \
    ltablib.c \
    ltm.c \
    lua.c \
    luac.c \
    lundump.c \
    lutf8lib.c \
    lvm.c \
    lzio.c \
    block.cpp \
    dragview.cpp \
    blocklist.cpp \
    settings.cpp \
    settingsdialog.cpp

HEADERS  += mainwindow.h \
    digiview.h \
    lapi.h \
    lauxlib.h \
    lcode.h \
    lctype.h \
    ldebug.h \
    ldo.h \
    lfunc.h \
    lgc.h \
    llex.h \
    llimits.h \
    lmem.h \
    lobject.h \
    lopcodes.h \
    lparser.h \
    lprefix.h \
    lstate.h \
    lstring.h \
    ltable.h \
    ltm.h \
    lua.h \
    luaconf.h \
    lualib.h \
    lundump.h \
    lvm.h \
    lzio.h \
    block.h \
    dragview.h \
    blocklist.h \
    settings.h \
    settingsdialog.h

FORMS    += mainwindow.ui \
    settingsdialog.ui
