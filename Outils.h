#ifndef OUTILS_H
#define OUTILS_H

#include <QtGui>
#include "Config.h"

double conversion(qint64 valeur);
qint64 conversion(double valeur);
double conversion(int valeur);

bool enregistrement(const QString dossier, const QString code);

QString randomName();
qint64 sizeOfdir(QString path);
void searchPrezFile(QString prezFile, QString link);

#endif // OUTILS_H
