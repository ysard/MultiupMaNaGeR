#-------------------------------------------------
#
# Project created by QtCreator 2012-10-15T23:44:59
#
#-------------------------------------------------

QT += core gui widgets
QT += network

TARGET = multiupv2
TEMPLATE = app

#CURL
#unix:INCLUDEPATH += /usr/include/curl
unix:INCLUDEPATH += /media/DATA/Projets/multiupv2/dependances-developpement/curl-7.29.0/include
unix:INCLUDEPATH += /media/DATA/Projets/multiupv2/dependances-developpement/curl-7.29.0/include/curl
unix:LIBS += -L/media/DATA/Projets/multiupv2/dependances-developpement/curl-7.29.0/lib
unix:LIBS += -lcurl

# Pour les libraries CURL en static
#DEFINES += CURL_STATICLIB
#DEFINES += HTTP_ONLY


#QJSON
#DEFINES += QJSON_STATIC
#unix:INCLUDEPATH += ./dependances-developpement/qjson-unix-static/include
#unix:LIBS += -L./dependances-developpement/qjson-unix-static/lib -lqjson

#QT
# A VOIR 




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
