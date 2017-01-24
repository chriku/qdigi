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
#CONFIG += console
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
    kvdiagram.cpp \
    zip/zip_source_file.c \
    zip/zip_dirent.c \
    zip/zip_open.c \
    zip/zip_set_name.c \
    zip/zip_set_default_password.c \
    zip/zip_extra_field.c \
    zip/zip_source_filep.c \
    zip/zip_fdopen.c \
    zip/zip_hash.c \
    zip/zip_buffer.c \
    zip/zip_err_str.c \
    zip/zip_utf-8.c \
    zip/zip_unchange.c \
    zip/zip_unchange_data.c \
    zip/zip_unchange_archive.c \
    zip/zip_unchange_all.c \
    zip/zip_string.c \
    zip/zip_strerror.c \
    zip/zip_stat.c \
    zip/zip_stat_init.c \
    zip/zip_stat_index.c \
    zip/zip_source_zip_new.c \
    zip/zip_source_zip.c \
    zip/zip_source_window.c \
    zip/zip_source_win32w.c \
    zip/zip_source_win32utf8.c \
    zip/zip_source_win32handle.c \
    zip/zip_source_win32a.c \
    zip/zip_source_supports.c \
    zip/zip_source_stat.c \
    zip/zip_source_read.c \
    zip/zip_source_pkware.c \
    zip/zip_source_open.c \
    zip/zip_source_layered.c \
    zip/zip_source_function.c \
    zip/zip_source_free.c \
    zip/zip_source_error.c \
    zip/zip_source_deflate.c \
    zip/zip_source_crc.c \
    zip/zip_source_close.c \
    zip/zip_source_call.c \
    zip/zip_source_buffer.c \
    zip/zip_set_file_compression.c \
    zip/zip_set_file_comment.c \
    zip/zip_set_archive_flag.c \
    zip/zip_set_archive_comment.c \
    zip/zip_replace.c \
    zip/zip_rename.c \
    zip/zip_new.c \
    zip/zip_name_locate.c \
    zip/zip_memdup.c \
    zip/zip_io_util.c \
    zip/zip_get_num_files.c \
    zip/zip_get_num_entries.c \
    zip/zip_get_name.c \
    zip/zip_get_file_comment.c \
    zip/zip_get_encryption_implementation.c \
    zip/zip_get_compression_implementation.c \
    zip/zip_get_archive_flag.c \
    zip/zip_get_archive_comment.c \
    zip/zip_fread.c \
    zip/zip_fopen.c \
    zip/zip_fopen_index.c \
    zip/zip_fopen_index_encrypted.c \
    zip/zip_fopen_encrypted.c \
    zip/zip_filerange_crc.c \
    zip/zip_file_strerror.c \
    zip/zip_file_set_external_attributes.c \
    zip/zip_file_set_comment.c \
    zip/zip_file_replace.c \
    zip/zip_file_rename.c \
    zip/zip_file_get_offset.c \
    zip/zip_file_get_external_attributes.c \
    zip/zip_file_get_comment.c \
    zip/zip_file_error_get.c \
    zip/zip_file_error_clear.c \
    zip/zip_file_add.c \
    zip/zip_fclose.c \
    zip/zip_extra_field_api.c \
    zip/zip_error.c \
    zip/zip_error_to_str.c \
    zip/zip_error_strerror.c \
    zip/zip_error_get.c \
    zip/zip_error_get_sys_type.c \
    zip/zip_error_clear.c \
    zip/zip_entry.c \
    zip/zip_discard.c \
    zip/zip_dir_add.c \
    zip/zip_delete.c \
    zip/zip_close.c \
    zip/zip_add.c \
    zip/zip_add_entry.c \
    zip/zip_add_dir.c \
    zip/zip_source_tell.c \
    zip/zip_source_tell_write.c \
    zip/zip_source_seek.c \
    zip/zip_source_seek_write.c \
    zip/zip_source_write.c \
    zip/zip_source_rollback_write.c \
    zip/zip_source_commit_write.c \
    zip/zip_source_begin_write.c \
    zip/zip_file_set_mtime.c \
    zip/zip_source_remove.c \
    zip/zip_source_is_deleted.c \
    zip/mkstemp.c \
    zlib/zutil.c \
    zlib/inftrees.c \
    zlib/inffast.c \
    zlib/gzwrite.c \
    zlib/gzlib.c \
    zlib/deflate.c \
    zlib/trees.c \
    zlib/inflate.c \
    zlib/infback.c \
    zlib/gzread.c \
    zlib/crc32.c \
    zlib/compress.c \
    zlib/adler32.c \
    zlib/uncompr.c \
    zlib/gzclose.c



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
    passworddialog.h \
    kvdiagram.h \
    zip/zipint.h \
    zip/compat.h \
    zip/zipwin32.h \
    zip/zip.h \
    zlib/zlib.h \
    zlib/zutil.h \
    zlib/zconf.h \
    zlib/inflate.h \
    zlib/gzguts.h \
    zlib/deflate.h \
    zlib/crc32.h \
    zlib/inffixed.h \
    zlib/inffast.h \
    zlib/inftrees.h \
    zlib/trees.h
android:SOURCES = crypto/arm_arch.h \
crypto/armcap.c
FORMS    += mainwindow.ui \
    settingsdialog.ui \
    passworddialog.ui \
    kvdiagram.ui
INCLUDEPATH += ./crypto/ ./crypto/include/
RESOURCES += images.qrc
DEFINES += LUASOCKET_API="'__attribute__((visibility(\"default\")))'"
win32:DEFINES += LUASOCKET_INET_PTON
win32:DEFINES += WINVER=0x0501
# -DLUASOCKET_API='__declspec(dllexport)' \
#	-DMIME_API='__declspec(dllexport)'
win32:DEFINES  += _WIN32
#win32:CONFIG += console

DISTFILES += \
    qdigi.reg
