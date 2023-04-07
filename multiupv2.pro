# This file is part of Multiup MaNaGeR.
#
# Multiup MaNaGeR is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Multiup MaNaGeR is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Multiup MaNaGeR. If not, see <http://www.gnu.org/licenses/>.
#
# Copyright 2012-2023 Lex
# Copyright 2013 LuNiKoFf
#-------------------------------------------------
#
# Project created by QtCreator 2012-10-15T23:44:59
#
#-------------------------------------------------

QT += core gui widgets
QT += network
# Activate the processing of the FORMS content like mainwindow.ui to ui_mainwindow.h
QT += widgets
win32:QT += winextras

TARGET = multiup
TEMPLATE = app

CONFIG(release, debug|release) {
    # Remove logging on Release mode
    DEFINES += QT_NO_DEBUG_OUTPUT
    DEFINES += QT_NO_WARNING_OUTPUT
    message(Release mode is active!)
}
# Known platforms
# - win32 mingw
# - linux unix posix
message(Platform detected : $$QMAKE_PLATFORM)

# Detection & configuration of MSYS env
QTPREFIX=$$[QT_INSTALL_PREFIX]
# Ex: E:/msys64/mingw64/qt5-static
message(Qt kit used: $$QTPREFIX)
MSYS_DETECTION = $$find(QTPREFIX, "msys")
count(MSYS_DETECTION, 1) {
    message(MSYS env detected!)
    MSYS_PREFIX = "$$QTPREFIX/../.."
    message(MSYS_PREFIX: $$MSYS_PREFIX)
}

# Curl dependency for Windows
# Note: Depending on the options used when compilation of curl you may be
# expected to use the following flags:
# -lidn -lssl -lcrypto -lrt -lssl -lz -lrtmp -lwldap32 -lws2_32
count(MSYS_DETECTION, 1) {
    # Use manually compiled version of libcurl, without useless dependencies
    # This lib is NOT installed in msys env (to avoid conflicts).
    win32:INCLUDEPATH += E:\curl-7.88.1\include \
    win32:INCLUDEPATH += E:\curl-7.88.1\include\curl
    win32:LIBS += -L'E:\curl-7.88.1\lib\.libs'
    win32:LIBS += -lcurl -lbcrypt -ladvapi32 -lcrypt32 -lssl -lcrypto -lssl -lcrypto -lgdi32 -lzstd -lz -lws2_32
} else {
    # Legacy compilation toolchain
    # With mingw32 :
    win32:INCLUDEPATH += dependances-developpement\curl-7.28.1-devel-mingw32\include
    win32:INCLUDEPATH += dependances-developpement\curl-7.28.1-devel-mingw32\include\curl
    # Absolute path is mandatory for linking step
    win32:LIBS += -L"D:\Projets\multiupv2\dependances-developpement\curl-7.28.1-devel-mingw32\lib"
    win32:LIBS += -L"Z:\media\DATA\Projets\multiupv2\dependances-developpement\curl-7.28.1-devel-mingw32\lib"
    win32:LIBS += -lcurl -lwldap32 -lws2_32

    # With msvc (draft):
    #win32:INCLUDEPATH += libcurl-7.19.3-win32-ssl-msvc \
    #win32:INCLUDEPATH += libcurl-7.19.3-win32-ssl-msvc\include\curl \
    #win32:INCLUDEPATH += libcurl-7.19.3-win32-ssl-msvc\lib\Release
}

# Curl dependency for Unix systems
# Use system dev version
unix:LIBS += -lcurl

# Mandatory for static libcurl
# Works for Windows & GNU/Linux, but we make a static compilation only for Windows.
win32:DEFINES += CURL_STATICLIB

# Global paths for MSYS env
count(MSYS_DETECTION, 1) {
    win32:INCLUDEPATH += $$MSYS_PREFIX/usr/include
    win32:INCLUDEPATH += $$MSYS_PREFIX/usr/include/openssl
    win32:LIBS += -L'$$MSYS_PREFIX/usr/lib'
    win32:LIBS += -L'$$MSYS_PREFIX/mingw64/lib'
}

SOURCES += main.cpp\
    mainwindow.cpp \
    Outils.cpp \
    fenvoirmiseajour.cpp \
    FenLiens.cpp \
    ObjectConnexion.cpp \
    ObjectCompression.cpp \
    ObjectSelectionServeur.cpp \
    ObjectUpCurl.cpp \
    ObjectRecupHebergeurs.cpp \
    ObjectRecupHebergeursIcones.cpp \
    InfoNewVersion.cpp

HEADERS += mainwindow.h \
    Outils.h \
    fenvoirmiseajour.h \
    FenLiens.h \
    ObjectConnexion.h \
    ObjectCompression.h \
    ObjectSelectionServeur.h \
    ObjectUpCurl.h \
    Config.h \
    ObjectRecupHebergeurs.h \
    ObjectRecupHebergeursIcones.h \
    version.h \
    InfoNewVersion.h

FORMS += mainwindow.ui \
    fenvoirmiseajour.ui \
    InfoNewVersion.ui

RESOURCES += Multiup.qrc

win32:RC_FILE = ressource_win32.rc

# Make translations visible for lrelease & lupdate
# For lrelease only, use EXTRA_TRANSLATIONS instead.
TRANSLATIONS = multiupManager_en.ts multiupManager_es.ts
# Embed translations files
CONFIG += lrelease
CONFIG += embed_translations
# Default prefix in resources
#QM_FILES_RESOURCE_PREFIX=/i18n/
