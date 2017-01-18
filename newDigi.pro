#-------------------------------------------------
#
# Project created by QtCreator 2016-12-16T14:11:29
#
#-------------------------------------------------

QT       += core gui network

RC_FILE = app.rc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = newDigi
TEMPLATE = app
win32:LIBS += -lws2_32

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
    settingsdialog.cpp \
    updater.cpp \
    painter.cpp \
    passworddialog.cpp \
#    wsocket.c \
##    usocket.c \
##    unixstream.c \
##    unixdgram.c \
##    unix.c \
#    udp.c \
#    timeout.c \
#    tcp.c \
##    serial.c \
#    select.c \
#    options.c \
#    mime.c \
#    luasocket.c \
#    io.c \
#    inet.c \
#    except.c \
#    compat.c \
#    buffer.c \
#    auxiliar.c


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
    settingsdialog.h \
    updater.h \
    painter.h \
    wsocket.h \
    usocket.h \
    unixstream.h \
    unixdgram.h \
    unix.h \
    udp.h \
    timeout.h \
    tcp.h \
    socket.h \
    select.h \
    pierror.h \
    options.h \
    mime.h \
    luasocket.h \
    io.h \
    inet.h \
    except.h \
    compat.h \
    buffer.h \
    auxiliar.h \
    passworddialog.h
android:SOURCES = crypto/arm_arch.h \
crypto/armcap.c
FORMS    += mainwindow.ui \
    settingsdialog.ui \
    passworddialog.ui
INCLUDEPATH += ./crypto/ ./crypto/include/
RESOURCES += images.qrc
DEFINES += LUASOCKET_API="'__attribute__((visibility(\"default\")))'"
win32:DEFINES += LUASOCKET_INET_PTON
win32:DEFINES += WINVER=0x0501
# -DLUASOCKET_API='__declspec(dllexport)' \
#	-DMIME_API='__declspec(dllexport)'
win32:DEFINES  += _WIN32
