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
#include "ObjectSelectionServeur.h"

SelectionServeur::SelectionServeur(QObject *parent): QObject(parent)
{
    // Init default file size
    m_fileSize = 0;
}

SelectionServeur::SelectionServeur(int fileSize, QObject *parent): QObject(parent)
{
    // Init default file size
    m_fileSize = fileSize;
}

SelectionServeur::~SelectionServeur()
{
    qDebug() << "SelectionServeur :: Destruction SelectionServeur en cours...";

    delete m_networkAccessManager;
}

void SelectionServeur::demarrage()
{
    // !!!! Slot appelé lorsque on s'est assuré que le thread acceuillant cette classe dérivée de QObject est lancé !!!!
    qDebug() << "SelectionServeur :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    // On crée notre requête
    QNetworkRequest requete;
    requete.setRawHeader("User-Agent", USER_AGENT);
    requete.setRawHeader("Accept-Language","fr,fr-fr;q=0.8,en-us;q=0.5,en;q=0.3");
    requete.setRawHeader("Accept-Charset","ISO-8859-1,utf-8;q=0.7,*;q=0.7");
    //pour Qt 4.7.x :
    requete.setRawHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");

    qDebug() << "SelectionServeur :: Sélection serveur en cours...";

    QUrl url = QUrl(URL_SELECTION_SERVEUR);
    if (m_fileSize != 0) {
        // Choose the server according to the known size of the file
        QUrlQuery query;
        query.addQueryItem("size", QString::number(m_fileSize));
        url.setQuery(query.query());
    }
    requete.setUrl(url);

    // Initialisation du QNetworkAccessManager
    m_networkAccessManager = new QNetworkAccessManager;

    // Méthode get() pour poster le contenu de notre requête.
    QNetworkReply *r = m_networkAccessManager->get(requete);

    // Ligne permettant de mettre en file les QObject:connect,
    // Sinon pas d'appel du slot d'erreur...
    // http://doc.trolltech.com/4.2/qt.html#ConnectionType-enum
    // http://stackoverflow.com/questions/9646110/how-to-send-a-qt-signal-containing-a-cvmat
    // qRegisterMetaType< QNetworkReply::NetworkError >("QNetworkReply::NetworkError");

    connect(r, SIGNAL(finished()), this, SLOT(finSelectionServeur()));
}

void SelectionServeur::finSelectionServeur()
{
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender()); //On récupère la réponse du serveur

    qDebug() << "SelectionServeur :: Récupération de la réponse du serveur...";

    // Récupération de la page
    QByteArray temp = r->readAll();
    qDebug() << temp;

    // Traitement des erreurs éventuelles
    switch (r->error()) {
        // Doc de l'énumération : http://qt.developpez.com/doc/4.7/qnetworkreply/#networkerror-enum
        case QNetworkReply::NoError :   qDebug() << "SelectionServeur :: Pas d'erreurs, poursuite";
                                        r->deleteLater();
                                        m_statutConnexion = Bad; // A la base, pas d'ip trouvée
                                        break;

        default :                       qCritical() << "SelectionServeur :: Erreur lors du chargement. Code de l'erreur : " << r->errorString();
                                        r->deleteLater();
                                        m_statutConnexion = Error;
                                        this->finProcedure();
                                        return;
                                        break;
    }


    //V3
    //{"error":"success","server":"http:\/\/sarge.multiup.io\/upload\/index.php"}

    QJsonDocument loadDoc(QJsonDocument::fromJson(temp));

    if (!loadDoc.isNull()) { // Test de validité

        QJsonObject connexionObj = loadDoc.object();

        if (connexionObj["error"].toString() == "success") {
            // Succès certain

            m_adresseIp = connexionObj["server"].toString();
            // TODO: Retrait des caractères d'échappement inutiles... (méthode remove de QString)

            qDebug() << "SelectionServeur :: Serveur" << m_adresseIp  << "trouvé !";
            m_statutConnexion = Ok;
        }

        // Echec certain
        // m_statutConnexion reste à Bad
    }

    this->finProcedure();
}

void SelectionServeur::finProcedure()
{
    switch (m_statutConnexion) {

        case Bad:   qDebug() << "SelectionServeur :: Pas d'adresse IP trouvée";
                    emit this->emissionEtatSelectionServeur(0);
                    emit this->finished();
                    break;

        case Ok:    qDebug() << "SelectionServeur :: Adresse IP trouvée";
                    emit this->emissionAdresseIp(m_adresseIp);
                    emit this->emissionEtatSelectionServeur(1);
                    emit this->finished();
                    break;

        case Error: qDebug() << "SelectionServeur :: Erreur";
                    emit this->emissionEtatSelectionServeur(2);
                    emit this->finished();
                    break;

        default:    qDebug() << "SelectionServeur :: Erreur inconnue";
                    emit this->emissionEtatSelectionServeur(2);
                    emit this->finished();
                    break;
    }
}
