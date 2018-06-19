#ifndef CONFIG_H
#define CONFIG_H

#ifdef Q_OS_WIN
    #define WINDOWS
#else
    #define LINUX
#endif

#define VERSION "0.4.2"
#define URL_MULTIUP "https://www.multiup.org/"

// --- GitHub urls
#define URL_GITHUB_LICENSE ""
#define URL_GITHUB_RELEASES "http://127.0.0.1/github_simulation.html"
#ifdef LINUX
    #define REGEX_VERSION_GITHUB "<a href=\"([\\/\\w \\.-]*linux-v(.*)\\.zip)\""
#endif
#ifdef WINDOWS
    #define REGEX_VERSION_GITHUB "<a href=\"([\\/\\w \\.-]*windows-v(.*)\\.zip)\""
#endif

// --- ObjectMiseAJour.cpp
#define URL_MAJ_LINUX "http://hotspot.fr/version_linux.txt"
#define URL_MAJ_WINDOWS "http://127.0.0.1/http/version_windows.txt"

// --- ObjectConnexion.cpp
#define URL_CONNEXION_UTILISATEUR "https://www.multiup.org/api/login"
#define URL_RECUPERATION_DROITS "https://www.multiup.org/api/get-list-hosts"
#define URL_SELECTION_SERVEUR "https://www.multiup.org/api/get-fastest-server"

// --- ObjectRecupIcones.cpp
#define URL_RECUPERATION_ICONES "http://www.multiup.org/bundles/multiupsite/img/hosts/"

// --- User-Agent
#define USER_AGENT "Mozilla/5.0 (Windows NT 5.1; rv:10.0.2) Gecko/20100101 Firefox/10.0.2"

// --- Custom directory for the application on Linux
#ifdef LINUX
    #define APP_DIR "Multiupv2"
#endif

#endif // CONFIG_H
