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

#include "qglobal.h"
#ifdef Q_OS_WIN
    #define WINDOWS
#else
    #define LINUX
#endif

#define VERSION "0.4.6"
#define URL_MULTIUP "https://multiup.io/"

// --- GitHub urls
#define URL_GITHUB_LICENSE "https://github.com/ysard/MultiupMaNaGeR/blob/master/LICENSE"
#define URL_API_GITHUB_RELEASES "https://api.github.com/repos/ysard/MultiupMaNaGeR/releases"

// --- ObjectConnexion.cpp
#define URL_CONNEXION_UTILISATEUR "https://multiup.io/api/login"
#define URL_RECUPERATION_DROITS "https://multiup.io/api/get-list-hosts"
#define URL_SELECTION_SERVEUR "https://multiup.io/api/get-fastest-server"

// --- ObjectRecupIcones.cpp
#define URL_RECUPERATION_ICONES "https://multiup.io/assets/hosts/"

// --- User-Agent
#ifdef LINUX
    #define USER_AGENT "Mozilla/5.0 (Linux) MultiupMaNaGeR/" VERSION
#endif
#ifdef WINDOWS
    #define USER_AGENT "Mozilla/5.0 (Windows) MultiupMaNaGeR/" VERSION
#endif

// --- Custom directory in ~/ for the application on GNU/Linux
#ifdef LINUX
    #define APP_DIR "Multiupv2"
#endif

#define SETTINGS_FILE "config.ini"

#endif // CONFIG_H
