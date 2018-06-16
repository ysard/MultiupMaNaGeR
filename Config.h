#ifndef CONFIG_H
#define CONFIG_H

#ifdef Q_OS_WIN
    #define WINDOWS
#else
    #define LINUX
#endif

#define VERSION "0.4.2"

// --- ObjectMiseAJour.cpp
#define URL_MAJ_LINUX "http://hotspot.fr/version_linux.txt"
#define URL_MAJ_WINDOWS "http://127.0.0.1/http/version_windows.txt"

// --- ObjectConnexion.cpp
#define URL_CONNEXION_UTILISATEUR "https://www.multiup.org/api/login"
#define URL_RECUPERATION_DROITS "https://www.multiup.org/api/get-list-hosts"
#define URL_SELECTION_SERVEUR "https://www.multiup.org/api/get-fastest-server"

// --- ObjectRecupIcones.cpp
#define URL_RECUPERATION_ICONES "http://www.multiup.org/bundles/multiupsite/img/hosts/"

// --- Custom directory for the application on Linux
#ifdef LINUX
    #define APP_DIR "Multiupv2"
#endif

#endif // CONFIG_H
