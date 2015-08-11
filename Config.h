#ifndef CONFIG_H
#define CONFIG_H

#if defined (Q_WS_WIN)
    #define WINDOWS
#else
    #define LINUX
#endif
//#define LINUX_ANDROID

#define VERSION "0.4"

// --- ObjectMiseAJour.cpp
#define URL_MAJ_LINUX "http://hotspot.fr/version_linux.txt"
#define URL_MAJ_WINDOWS "http://127.0.0.1/http/version_windows.txt"

// --- ObjectConnexion.cpp
#define URL_CONNEXION_UTILISATEUR "http://www.multiup.org/api/login"
#define URL_RECUPERATION_DROITS "http://www.multiup.org/api/get-list-hosts"
#define URL_SELECTION_SERVEUR "http://www.multiup.org/api/get-fastest-server"

// --- ObjectRecupIcones.cpp
#define URL_RECUPERATION_ICONES "http://www.multiup.org/bundles/multiupsite/img/hosts/"

#endif // CONFIG_H
