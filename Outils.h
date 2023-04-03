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
#ifndef OUTILS_H
#define OUTILS_H

#include <QtGui>
#include <QApplication>
#include "Config.h"

double conversion(qint64 valeur);
qint64 conversion(double valeur);
double conversion(int valeur);

bool enregistrement(const QString dossier, const QString code);

QString randomName();
qint64 sizeOfdir(QString path);
QHash<QString, QByteArray> loadCachedIcons(QString path);
bool saveIconToCache(QString hostName, QByteArray iconData);
void searchPrezFile(QString prezFile, QString link);

#endif // OUTILS_H
