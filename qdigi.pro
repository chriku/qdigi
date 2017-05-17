#-------------------------------------------------
#
# Project created by QtCreator 2016-12-16T14:11:29
#
#-------------------------------------------------

QT       += core gui network webkitwidgets

RC_FILE = app.rc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qdigi
TEMPLATE = app
win32:LIBS += -lws2_32
#CONFIG += console
SOURCES += main.cpp\
        mainwindow.cpp \
    digiview.cpp \
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
    kvdiagram.cpp \
    gdrive.cpp \
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
    lualibcollection.cpp \
    luahttp.cpp \
    luasettings.cpp \
    luadesktop.cpp \
    luahttpserver.cpp \
    impulsedialog.cpp \
    impulsewidget.cpp \
    item.cpp \
    line.cpp \
    text.cpp \
    via.cpp \
    schematic.cpp \
    impulselabel.cpp \
    helpdialog.cpp \
    markdownwidget.cpp \
    helpmanager.cpp \
    helpreply.cpp

QMAKE_INCDIR += ./lua


HEADERS  += mainwindow.h \
    digiview.h \
    block.h \
    dragview.h \
    blocklist.h \
    settings.h \
    settingsdialog.h \
    updater.h \
    painter.h \
    passworddialog.h \
    kvdiagram.h \
    gdrive.h \
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
    lualibcollection.h \
    luahttp.h \
    luasettings.h \
    luadesktop.h \
    luahttpserver.h \
    impulsedialog.h \
    impulsewidget.h \
    item.h \
    line.h \
    text.h \
    via.h \
    schematic.h \
    impulselabel.h \
    helpdialog.h \
    markdownwidget.h \
    helpmanager.h \
    helpreply.h
android:SOURCES = crypto/arm_arch.h \
crypto/armcap.c
FORMS    += mainwindow.ui \
    settingsdialog.ui \
    passworddialog.ui \
    kvdiagram.ui \
    filechoose.ui \
    impulsedialog.ui
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

INCLUDEPATH += ./zip
CONFIG += link_pkgconfig
PKGCONFIG += libzip
