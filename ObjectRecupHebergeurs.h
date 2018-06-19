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
 * Copyright 2012-2018 Lex
 */
#ifndef OBJECTRECUPHEBERGEURS_H
#define OBJECTRECUPHEBERGEURS_H

#include <QtGui>
#include <QtNetwork>
#include <QJsonDocument>
#include "ObjectRecupHebergeursIcones.h"
#include "Config.h"

class RecupHebergeurs : public QObject
{
    Q_OBJECT

public:
    // Statut de la connexion BAD,OK,ERROR
    enum EtatConnexion
    {
        Bad, Ok, Error
    };

    RecupHebergeurs(QObject* parent = NULL);
    RecupHebergeurs(const QString &login, const QString &password, QObject* parent = NULL);
    virtual ~RecupHebergeurs();

public slots:
    void demarrage();

private slots:
    void finRecupHebergeurs();

signals:
    void emissionUrlIcone(QUrl, int);
    void emissionRecupHebergeursEtat(int etatConnexion);
    void emissionRecupHebergeursHebergeurs(QString, QString, bool, int);
    void emissionRecupHebergeursIcones(QByteArray, int);
    void emissionMaxSelectionHebergeurs(int);
    void finished();

private:
    void finProcedure();
    QNetworkAccessManager   *m_networkAccessManager;
    EtatConnexion           m_statutConnexion;

    RecupHebergeursIcones   *m_threadRecupIcones;
    QThread                 *m_thread;

    QString                 m_login;
    QString                 m_password;
};

#endif // OBJECTRECUPHEBERGEURS_H
