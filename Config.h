/* This file is part of Multiup MaNaGeR.
 *
 * Multiup MaNaGeR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Multiup MaNaGeR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Multiup MaNaGeR. If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2012-2023 Lex
 */
#ifndef CONFIG_H
#define CONFIG_H

#ifdef Q_OS_WIN
    #define WINDOWS
#else
    #define LINUX
#endif

#define VERSION "0.4.4"
#define URL_MULTIUP "https://www.multiup.org/"

// --- GitHub urls
#define URL_GITHUB_LICENSE "https://github.com/ysard/MultiupMaNaGeR/blob/master/LICENSE"
#define URL_GITHUB_RELEASES "https://github.com/ysard/MultiupMaNaGeR/releases"
#ifdef LINUX
    #define REGEX_VERSION_GITHUB "<a href=\"([\\/\\w \\.-]*linux-v(.*)\\.zip)\""
#endif
#ifdef WINDOWS
    #define REGEX_VERSION_GITHUB "<a href=\"([\\/\\w \\.-]*windows-v(.*)\\.zip)\""
#endif

// --- ObjectConnexion.cpp
#define URL_CONNEXION_UTILISATEUR "https://www.multiup.org/api/login"
#define URL_RECUPERATION_DROITS "https://www.multiup.org/api/get-list-hosts"
#define URL_SELECTION_SERVEUR "https://www.multiup.org/api/get-fastest-server"

// --- ObjectRecupIcones.cpp
#define URL_RECUPERATION_ICONES "https://multiup.org/assets/hosts/"

// --- User-Agent
#define USER_AGENT "Mozilla/5.0 (Windows NT 5.1; rv:10.0.2) Gecko/20100101 Firefox/10.0.2"

// --- Custom directory for the application on Linux
#ifdef LINUX
    #define APP_DIR "Multiupv2"
#endif

#endif // CONFIG_H
