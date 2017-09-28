#-------------------------------------------------
#
# Project created by QtCreator 2016-12-16T14:11:29
#
#-------------------------------------------------

QT       += core gui network
#LIBS += -lssl -lcrypto
RC_FILE = app.rc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qdigi
TEMPLATE = app
win32:LIBS += -lws2_32

#CONFIG += console
SOURCES += main.cpp\
        mainwindow.cpp \
    digiview.cpp \
    items/block.cpp \
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
    filechoose.cpp \
    remotedrive.cpp \
    remotedrivelist.cpp \
    lua/auxiliar.c \
    #lua/buffer.c \
    lua/compat.c \
    lua/except.c \
    #lua/inet.c \
    #lua/io.c \
    lua/lapi.c \
    lua/lauxlib.c \
    lua/lbaselib.c \
    lua/lbitlib.c \
    lua/lcode.c \
    lua/lcorolib.c \
    lua/lctype.c \
    lua/ldblib.c \
    lua/ldebug.c \
    lua/ldo.c \
    lua/ldump.c \
    lua/lfunc.c \
    lua/lgc.c \
    lua/linit.c \
    lua/liolib.c \
    lua/llex.c \
    lua/lmathlib.c \
    lua/lmem.c \
    lua/loadlib.c \
    lua/lobject.c \
    lua/lopcodes.c \
    lua/loslib.c \
    lua/lparser.c \
    lua/lstate.c \
    lua/lstring.c \
    lua/lstrlib.c \
    lua/ltable.c \
    lua/ltablib.c \
    lua/ltm.c \
    lua/lua.c \
    lua/luac.c \
    #lua/luasocket.c \
    lua/lundump.c \
    lua/lutf8lib.c \
    lua/lvm.c \
    lua/lzio.c \
    #lua/mime.c \
    #lua/options.c \
    #lua/select.c \
    #lua/serial.c \
    #lua/tcp.c \
    #lua/timeout.c \
    #lua/udp.c \
    #lua/unix.c \
    #lua/unixdgram.c \
    #lua/unixstream.c \
    #lua/usocket.c \
    #lua/wsocket.c \
    items/item.cpp \
    items/line.cpp \
    items/text.cpp \
    items/via.cpp \
    schematic.cpp \
    items/impulselabel.cpp \
    markdownwidget.cpp \
    helpdialog.cpp \
    items/jumplabel.cpp \
    items/subitemblock.cpp \
    items/luablock.cpp \
    extension/extension.cpp \
    extension/extensionloader.cpp \
    extension/dirloader.cpp \
    extension/ziploader.cpp \
    extension/module.cpp \
    extension/blockmodule.cpp \
    items/extensionblock.cpp

QMAKE_INCDIR += ./lua


HEADERS  += mainwindow.h \
    digiview.h \
    items/block.h \
    dragview.h \
    blocklist.h \
    settings.h \
    settingsdialog.h \
    updater.h \
    painter.h \
    passworddialog.h \
    filechoose.h \
    remotedrive.h \
    remotedrivelist.h \
    lua/auxiliar.h \
    #lua/buffer.h \
    lua/compat.h \
    lua/except.h \
    #lua/inet.h \
    lua/io.h \
    lua/lapi.h \
    lua/lauxlib.h \
    lua/lcode.h \
    lua/lctype.h \
    lua/ldebug.h \
    lua/ldo.h \
    lua/lfunc.h \
    lua/lgc.h \
    lua/llex.h \
    lua/llimits.h \
    lua/lmem.h \
    lua/lobject.h \
    lua/lopcodes.h \
    lua/lparser.h \
    lua/lprefix.h \
    lua/lstate.h \
    lua/lstring.h \
    lua/ltable.h \
    lua/ltm.h \
    lua/lua.h \
    lua/luaconf.h \
    lua/lualib.h \
    #lua/luasocket.h \
    lua/lundump.h \
    lua/lvm.h \
    lua/lzio.h \
    lua/mime.h \
    #lua/socket.h \
    #lua/tcp.h \
    #lua/timeout.h \
    #lua/udp.h \
    #lua/unix.h \
    #lua/unixdgram.h \
    #lua/unixstream.h \
    #lua/usocket.h \
    #lua/wsocket.h \
    items/item.h \
    items/line.h \
    items/text.h \
    items/via.h \
    schematic.h \
    items/impulselabel.h \
    markdownwidget.h \
    helpdialog.h \
    items/jumplabel.h \
    items/subitemblock.h \
    items/luablock.h \
    extension/extension.h \
    extension/extensionloader.h \
    extension/dirloader.h \
    extension/ziploader.h \
    extension/module.h \
    extension/blockmodule.h \
    items/extensionblock.h
android:SOURCES = crypto/arm_arch.h \
crypto/armcap.c
FORMS    += mainwindow.ui \
    settingsdialog.ui \
    passworddialog.ui \
    filechoose.ui \
    helpdialog.ui
INCLUDEPATH += ./crypto/ ./crypto/include/
RESOURCES += images.qrc \
    misc.qrc \
    res.qrc
DEFINES += LUASOCKET_API="'__attribute__((visibility(\"default\")))'"
win32:DEFINES += LUASOCKET_INET_PTON
win32:DEFINES += WINVER=0x0501
# -DLUASOCKET_API='__declspec(dllexport)' \
#	-DMIME_API='__declspec(dllexport)'
win32:DEFINES  += _WIN32
#win32:CONFIG += console

DISTFILES += \
    qdigi.reg

#INCLUDEPATH += ./zip
CONFIG += link_pkgconfig
#PKGCONFIG += libzip
