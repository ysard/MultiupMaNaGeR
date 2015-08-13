#-------------------------------------------------
#
# Project created by QtCreator 2012-10-15T23:44:59
#
#-------------------------------------------------

QT += core gui
QT += network

TARGET = multiupv2
TEMPLATE = app

# Supprimer tous les Debugs
DEFINES += QT_NO_DEBUG_OUTPUT


#CURL unix
#unix:INCLUDEPATH += /usr/include/curl
unix:INCLUDEPATH += ./dependances-developpement/curl-7.28.1-devel-unix-static/include
unix:LIBS += -L./dependances-developpement/curl-7.28.1-devel-unix-static/lib
unix:LIBS += -lcurl -lz -lidn -lssl -lcrypto
#-lidn -lssl -lcrypto -lrt -lssl -lz -lrtmp (selon les restrictions lors de la compilation de curl..)


#CURL windows
# Avec mingw32 :
win32:INCLUDEPATH += .\dependances-developpement\curl-7.28.1-devel-mingw32\include

# Avec msvc :
#win32:INCLUDEPATH += libcurl-7.19.3-win32-ssl-msvc
#win32:INCLUDEPATH += libcurl-7.19.3-win32-ssl-msvc\lib\Release

win32:LIBS += -L".\dependances-developpement\curl-7.28.1-devel-mingw32\lib"
#revoir la lib ici..-lws2_32  ptetre aussi... -lwldap32 ??
win32:LIBS += -lcurl -lwldap32 -lws2_32

# Pour les libraries CURL en static
DEFINES += CURL_STATICLIB
DEFINES += HTTP_ONLY




#QJSON
DEFINES += QJSON_STATIC
#DEFINES += QJSON_MAKEDLL

unix:INCLUDEPATH += ./dependances-developpement/qjson-unix-static/include
unix:LIBS += -L./dependances-developpement/qjson-unix-static/lib -lqjson

win32:INCLUDEPATH += .\dependances-developpement\qjson-win32-static\include
win32:LIBS += -L".\dependances-developpement\qjson-win32-static\lib" -lqjson




#QT
# A VOIR 
#win32:LIBS += -L"G:\QtSDK\mingw\lib" 
CONFIG += static
QMAKESPEC=win32-g++
QMAKE_LFLAGS += -static-libgcc




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
    ObjectRecupHebergeursIcones.h

FORMS    += mainwindow.ui \
    fenvoirmiseajour.ui \
    FenDemandeMiseAJour.ui

RESOURCES += Multiup.qrc

win32:RC_FILE = ressource_win32.rc

TRANSLATIONS = multiupManager_en.ts
