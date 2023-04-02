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
#include "ObjectConnexion.h"

Connexion::Connexion(const QString &login, const QString &password, QObject *parent): QObject(parent)
{
    qDebug() << "Connexion :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    m_login     = login;
    m_password  = password;

    // PS: Rien d'autre ne peut être initialisé ici...
    // Surtout pas le QNetworkAccessManager car :
    // nous sommes encore dans un objet appartenant au thread parent
    // l'attribut sera lu dans le thread enfant
    // ex:
    // QObject: Cannot create children for a parent that is in a different thread.
    // (Parent is QNetworkAccessManager(0x89cc4e0), parent's thread is QThread(0x8930af8), current thread is QThread(0x89f3b38)
    // PS: ce message notifiant 3 threads, est probablement du au fait que l'initialisation de cet objet se fait par défaut détaché de tout:
    // QObject *parent = NULL...
}

Connexion::~Connexion()
{
    qDebug() << "Connexion :: Destruction Connexion en cours...";
    delete m_networkAccessManager;
}

void Connexion::demarrage()
{
    // !!!! Slot appelé lorsque on s'est assuré que le thread acceuillant cette classe dérivée de QObject est lancé !!!!
    qDebug() << "Connexion :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    // On crée notre requête
    QNetworkRequest requete;
    requete.setRawHeader("User-Agent", USER_AGENT);
    requete.setRawHeader("Accept-Language","fr,fr-fr;q=0.8,en-us;q=0.5,en;q=0.3");
    requete.setRawHeader("Accept-Charset","ISO-8859-1,utf-8;q=0.7,*;q=0.7");
    requete.setRawHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
    requete.setUrl(QUrl(URL_CONNEXION_UTILISATEUR));

    qDebug() << "Connexion :: Connexion en cours...";

    //initialisation du QNetworkAccessManager
    m_networkAccessManager = new QNetworkAccessManager;

    // Méthode post() pour poster le contenu de notre requête.
    //v2
    //_csrf_token=69b27426be5296fef27a1817f0b022ea9c2cc6e3&_username=login&_password=pass&_submit=Login
    QNetworkReply *r = m_networkAccessManager->post(
                requete,
                QString("username=" + m_login +
                        "&password=" + m_password).toLatin1());

    connect(r, SIGNAL(finished()), this, SLOT(finLogin()));
    // Voir explications sur la désactivation dans la fonction :
    //connect(r, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(erreur(QNetworkReply::NetworkError)));
}

void Connexion::finLogin()
{
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender()); //On récupère la réponse du serveur

    qDebug() << "Connexion :: Récupération de la réponse du serveur...";

    // Récupération de la page
    QByteArray temp = r->readAll();

    // Traitement des erreurs éventuelles
    switch (r->error()) {
        // Doc de l'énumération : http://qt.developpez.com/doc/4.7/qnetworkreply/#networkerror-enum
        case QNetworkReply::NoError :   qDebug() << "Connexion :: Pas d'erreurs, poursuite";
                                        r->deleteLater();
                                        m_statutConnexion = Bad; // A la base il n'y a pas de connexion:
                                        break;

        default :                       qDebug() << "Connexion :: Erreur lors du chargement. Code de l'erreur : " << r->errorString();
                                        r->deleteLater();
                                        m_statutConnexion = Error;
                                        this->finProcedure();
                                        return;
                                        break;
    }

    // V3
    /*
    An array is a list of values, and an object is a collection of key/value pairs.
    All keys in an object are strings, and an object cannot contain any duplicate keys.
    The text representation of JSON encloses arrays in square brackets ([ ... ])
    and objects in curly brackets ({ ... }).

    Entries in arrays and objects are separated by commas.
    The separator between keys and values in an object is a colon (:).

    "{"error":"bad username OR bad password"}"
    "{"error":"success","login":"lex","user":1637}"
    */
    QJsonDocument loadDoc(QJsonDocument::fromJson(temp));

    if (!loadDoc.isNull()) { // Test de validité

        QJsonObject connexionObj = loadDoc.object();

        if (connexionObj["error"].toString() == "success") {
            // Succès certain

            // Récupération de l'id
            QString userId = QString::number(connexionObj["user"].toInt());

            if (!userId.isEmpty()) {
                qDebug() << "Connexion :: Id utilisateur :" << userId;

                // Envoi de l'id
                emit this->emissionLoginId(userId);

                // Permet l'émission du signal avec le paramètre 1 = réussite
                // Et le passage   l'étape suivante de récupération des droits Wwpw
                // Voir finProcedure()
                m_statutConnexion = Ok;
            }
        }

        // "bad username OR bad password"
        // Echec certain
        // m_statutConnexion reste à Bad
    }


    // Emission des signaux
    // Notons que m_statutConnexion peut être égal à BAD si l'id n'a pas trouvé;
    // ou à OK si l'id a été trouvé.
    this->finProcedure();
}

/*
void Connexion::erreur(QNetworkReply::NetworkError) // Fonction dépassée, sans utilité pour le programme, voir plus bas.
{
    // Parfois ça a le temps d'envoyer à la fois le signal error et finished....
    // Dans la doc :
    // This signal is emitted when the reply detects an error in processing.
    // The finished() signal will probably follow, indicating that the connection is over.
    // ...
    // Pour éviter un bordel aléatoire par la suite, je traite les erreurs de connexion dans le slot appelé par finished avec la fonction r->error()

    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender());

    qDebug() << "Connexion :: Erreur lors du chargement. Code de l'erreur : " << r->errorString();

    r->deleteLater();

    m_statutConnexion = ERROR;

    this->finProcedure();
}
*/
void Connexion::finProcedure()
{
    //BAD,OK ERROR ?
    switch (m_statutConnexion) {

    case Bad:   qDebug() << "Connexion :: Mauvais login";
        emit this->emissionLoginEtat(0);
        break;

    case Ok:    qDebug() << "Connexion :: Bon login";
        emit this->emissionLoginEtat(1);
        break;

    case Error: qDebug() << "Connexion :: Erreur login";
        emit this->emissionLoginEtat(2);
        break;

    default:    qDebug() << "Connexion :: Erreur inconnue login";
        emit this->emissionLoginEtat(2);
        break;
    }

    // Signaler la fin du thread
    emit this->finished();
}
