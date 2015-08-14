#-------------------------------------------------
#
# Project created by QtCreator 2012-10-15T23:44:59
#
#-------------------------------------------------

QT += core gui widgets
QT += network
win32:QT += winextras

TARGET = multiupv2
TEMPLATE = app

#CURL
# Avec mingw32 :
win32:INCLUDEPATH += D:\Projets\multiupv2\dependances-developpement\curl-7.28.1-devel-mingw32\include \
win32:INCLUDEPATH += D:\Projets\multiupv2\dependances-developpement\curl-7.28.1-devel-mingw32\include\curl

# Avec msvc :
#win32:INCLUDEPATH += libcurl-7.19.3-win32-ssl-msvc \
#win32:INCLUDEPATH += libcurl-7.19.3-win32-ssl-msvc\include\curl \
#win32:INCLUDEPATH += libcurl-7.19.3-win32-ssl-msvc\lib\Release

# Pour les libraries CURL en static
win32:LIBS += -L"D:\Projets\multiupv2\dependances-developpement\curl-7.28.1-devel-mingw32\lib"
DEFINES += CURL_STATICLIB
DEFINES += HTTP_ONLY
#revoir la lib ici..-lws2_32  ptetre aussi... -lwldap32 ??
win32:LIBS += -lcurl -lwldap32 -lws2_32


#QJSON
#DEFINES += QJSON_STATIC
##DEFINES += QJSON_MAKEDLL
#win32:INCLUDEPATH += .\dependances-developpement\qjson-win32-static\include
#win32:LIBS += -L".\dependances-developpement\qjson-win32-static\lib" -lqjson

#QT
##win32:LIBS += -L"G:\QtSDK\mingw\lib"
#CONFIG += static
#QMAKESPEC=win32-g++
#QMAKE_LFLAGS += -static-libgcc

SOURCES += main.cpp\
        mainwindow.cpp \
    Outils.cpp \
    fenvoirmiseajour.cpp \
    FenLiens.cpp \
    ObjectConnexion.cpp \
    ObjectCompression.cpp \
    ObjectSelectionServeur.cpp \
    ObjectUpCurl.cpp \
    ObjectMiseAJour.cpp \
    FenDemandeMiseAJour.cpp \
    ObjectRecupHebergeurs.cpp \
    ObjectRecupHebergeursIcones.cpp

HEADERS  += mainwindow.h \
    Outils.h \
    fenvoirmiseajour.h \
    FenLiens.h \
    ObjectConnexion.h \
    ObjectCompression.h \
    ObjectSelectionServeur.h \
    ObjectUpCurl.h \
    ObjectMiseAJour.h \
    FenDemandeMiseAJour.h \
    Config.h \
    ObjectRecupHebergeurs.h \
    ObjectRecupHebergeursIcones.h \
    version.h

FORMS    += mainwindow.ui \
    fenvoirmiseajour.ui \
    FenDemandeMiseAJour.ui

RESOURCES += Multiup.qrc

win32:RC_FILE = ressource_win32.rc

TRANSLATIONS = multiupManager_en.ts
