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
#ifndef OBJECTCONNEXION_H
#define OBJECTCONNEXION_H

#include <QtGui>
#include <QtNetwork>
#include <QJsonDocument>
#include "Config.h"

class Connexion : public QObject
{
    Q_OBJECT

public:
    // Statut de la connexion BAD,OK,ERROR
    enum EtatConnexion
    {
        Bad, Ok, Error
    };

    Connexion(const QString &login, const QString &password, QObject* parent = NULL);
    virtual ~Connexion();


public slots:
    void demarrage();

private slots:
    void finLogin();
    //void erreur(QNetworkReply::NetworkError);

signals:
    void emissionLoginEtat(int etatConnexion);
    void emissionLoginId(QString id);
    void finished();

private:
    void finProcedure();
    QNetworkAccessManager   *m_networkAccessManager;
    QString                 m_login;
    QString                 m_password;

    EtatConnexion           m_statutConnexion;

};

#endif // OBJECTCONNEXION_H
