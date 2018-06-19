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
#include "ObjectRecupHebergeursIcones.h"

RecupHebergeursIcones::RecupHebergeursIcones(QObject *parent): QObject(parent)
{
    qDebug() << "RecupHebergeursIcones :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();
}

RecupHebergeursIcones::~RecupHebergeursIcones()
{
    qDebug() << "RecupHebergeursIcones :: Destruction RecupHebergeursIcones en cours...";
}

void RecupHebergeursIcones::demarrage()
{
    qDebug() << "RecupHebergeursIcones :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    // Simple initialisation
    m_networkAccessManager = new QNetworkAccessManager(this);
}

void RecupHebergeursIcones::downloadUrl(const QUrl url, int id)
{
    // Récupération sur le site des icones des hebergeurs...

    //qDebug() << "RecupHebergeursIcones :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();
    //qDebug() << "RecupHebergeursIcones :: Icone : Recuperation de l'icone" << url << "en cours...";

    // On crée notre requête => suppression des en-têtes => soyons rapides ici !
    QNetworkRequest request(url);

    // Envoi de la requête
    QNetworkReply *reply = m_networkAccessManager->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(finRecupHebergeurIcone()));

    // Miracle des tables de hashage ! On associe à chaque requête l'id de l'hébergeur associé pour le retrouver lors de la réception de la requête.
    // http://www.siteduzero.com/forum/sujet/qt-qnetworkrequest-27057

    // Stockage de la requête (autant d'icone que d'hébergeur)

    static int i = 0;
    m_hash.insert(reply, id); // méthode plus mieux..
    //m_hash[reply] = id;
    //si on utilise l'opérateur [] pour autre chose que l'insertion ça insert silencieusement des paires non désirée.. habitude à prendre !

    i++;
}

void RecupHebergeursIcones::finRecupHebergeurIcone()
{
    //qDebug() << "RecupHebergeursIcones :: Icone : recue...";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender()); //On récupère la réponse du serveur

    // Récupération de la page
    QByteArray temp = reply->readAll();

    // Traitement des erreurs éventuelles
    // Doc de l'énumération : http://qt.developpez.com/doc/4.7/qnetworkreply/#networkerror-enum
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "RecupHebergeurs :: Icone : Erreur lors du chargement. Code de l'erreur : " << reply->errorString();

        // On reset le QByteArray pr éviter un "trou" dans l'interface et donner 1 chance d'affichage aux autres icones
        temp.clear();
    }

    int id = m_hash.value(reply);
    //qDebug() << "Hash :" << id;
    emit this->emissionRecupHebergeursIcones(temp, id);

    // Nettoyage de la liste des requêtes en attente
    m_hash.remove(reply);

    reply->deleteLater();

    // Si tout est terminé => fin - Ce signal met aussi fin au thread RecupHebergeur
    if (m_hash.isEmpty())
        emit this->finished();
}
