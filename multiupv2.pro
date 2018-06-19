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

# Supprimer tous les messages de Debug
#DEFINES += QT_NO_DEBUG_OUTPUT
#DEFINES += QT_NO_WARNING_OUTPUT

#CURL unix
#unix:INCLUDEPATH += /usr/include/curl
#unix:INCLUDEPATH += /media/DATA/Projets/multiupv2/dependances-developpement/curl-7.29.0/include
#unix:INCLUDEPATH += /media/DATA/Projets/multiupv2/dependances-developpement/curl-7.29.0/include/curl
#unix:LIBS += -L/media/DATA/Projets/multiupv2/dependances-developpement/curl-7.29.0/lib
unix:LIBS += -lcurl
#-lidn -lssl -lcrypto -lrt -lssl -lz -lrtmp (selon les restrictions lors de la compilation de curl..)


#CURL windows
# Avec mingw32 :
win32:INCLUDEPATH += D:\Projets\multiupv2\dependances-developpement\curl-7.28.1-devel-mingw32\include \
win32:INCLUDEPATH += D:\Projets\multiupv2\dependances-developpement\curl-7.28.1-devel-mingw32\include\curl

# Avec msvc :
#win32:INCLUDEPATH += libcurl-7.19.3-win32-ssl-msvc \
#win32:INCLUDEPATH += libcurl-7.19.3-win32-ssl-msvc\include\curl \
#win32:INCLUDEPATH += libcurl-7.19.3-win32-ssl-msvc\lib\Release

win32:LIBS += -L"D:\Projets\multiupv2\dependances-developpement\curl-7.28.1-devel-mingw32\lib"
#revoir la lib ici..-lws2_32  ptetre aussi... -lwldap32 ??
win32:LIBS += -lcurl -lwldap32 -lws2_32

# Pour les libraries CURL en static Windows/Linux
win32:DEFINES += CURL_STATICLIB
win32:DEFINES += HTTP_ONLY




#QT
# A VOIR
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
    ObjectRecupHebergeurs.cpp \
    ObjectRecupHebergeursIcones.cpp \
    InfoNewVersion.cpp

HEADERS  += mainwindow.h \
    Outils.h \
    fenvoirmiseajour.h \
    FenLiens.h \
    ObjectConnexion.h \
    ObjectCompression.h \
    ObjectSelectionServeur.h \
    ObjectUpCurl.h \
    ObjectMiseAJour.h \
    Config.h \
    ObjectRecupHebergeurs.h \
    ObjectRecupHebergeursIcones.h \
    version.h \
    InfoNewVersion.h

FORMS    += mainwindow.ui \
    fenvoirmiseajour.ui \
    InfoNewVersion.ui

RESOURCES += Multiup.qrc

win32:RC_FILE = ressource_win32.rc

TRANSLATIONS = multiupManager_en.ts
