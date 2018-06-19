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
#include "ObjectRecupHebergeurs.h"

RecupHebergeurs::RecupHebergeurs(QObject *parent): QObject(parent)
{
    //qDebug() << "RecupHebergeurs :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    //qRegisterMetaType<QList<QByteArray> >("QList<QByteArray>");

    // PS: Rien d'autre ne peut être initialisé ici...
    // Surtout pas le QNetworkAccessManager car :
    // nous sommes encore dans un objet appartenant au thread parent
    // l'attribut sera lu dans le thread enfant
    // ex:
    // QObject: Cannot create children for a parent that is in a different thread.
    // (Parent is QNetworkAccessManager(0x89cc4e0), parent's thread is QThread(0x8930af8), current thread is QThread(0x89f3b38)
    //
    // PS: ce message notifiant 3 threads, est probablement du au fait que l'initialisation de cet objet se fait par défaut détaché de tout:
    // QObject *parent = NULL...
}

RecupHebergeurs::RecupHebergeurs(const QString &login, const QString &password, QObject *parent): QObject(parent)
{
    // Constructeur appelé en cas de récupération des hébergeurs avec le login

    m_login     = login;
    m_password  = password;
}

RecupHebergeurs::~RecupHebergeurs()
{
    qDebug() << "RecupHebergeurs :: Destruction en cours...";
}

void RecupHebergeurs::demarrage()
{
    // !!!! Slot appelé lorsque on s'est assuré que le thread acceuillant cette classe dérivée de QObject est lancé !!!!
    qDebug() << "RecupHebergeurs :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    // Initialisation du thread des icones
    //m_thread = new QThread(this);

    // Création et envoi de l'objet dans le thread
    //m_threadRecupIcones = new RecupHebergeursIcones(NULL);
    //m_threadRecupIcones->moveToThread(m_thread);

    //connect(m_thread, SIGNAL(started()), m_threadRecupIcones, SLOT(demarrage()));
    //connect(this, SIGNAL(emissionUrlIcone(QUrl, int)), m_threadRecupIcones, SLOT(downloadUrl(QUrl, int)));
    //connect(m_threadRecupIcones, SIGNAL(emissionRecupHebergeursIcones(QByteArray, int)), this, SIGNAL(emissionRecupHebergeursIcones(QByteArray, int)));
    //connect(m_threadRecupIcones, SIGNAL(finished()), m_thread, SLOT(quit()));
    //connect(m_threadRecupIcones, SIGNAL(finished()), m_threadRecupIcones, SLOT(deleteLater()));
    //connect(m_thread, SIGNAL(finished()), m_thread, SLOT(deleteLater()));
    //connect(m_thread, SIGNAL(destroyed()), this, SIGNAL(finished()));
    //m_thread->start();

    // On crée notre requête
    QNetworkRequest requete;
    requete.setRawHeader("User-Agent", USER_AGENT);
    requete.setRawHeader("Accept-Language","fr,fr-fr;q=0.8,en-us;q=0.5,en;q=0.3");
    requete.setRawHeader("Accept-Charset","ISO-8859-1,utf-8;q=0.7,*;q=0.7");
    requete.setRawHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
    requete.setUrl(QUrl(URL_RECUPERATION_DROITS));

    qDebug() << "RecupHebergeurs :: Recuperation des hebergeurs en cours...";

    // Initialisation du QNetworkAccessManager
    m_networkAccessManager = new QNetworkAccessManager(this);

    QNetworkReply *r = NULL;

    if (m_login.isEmpty() || m_password.isEmpty()) {
        // La requête GET suffit ici (pas de login spécifié)
        // en cas d'upload anonyme => pas de récupération des hébergeurs avec le login
        r = m_networkAccessManager->get(requete);
    } else {
        // en cas d'upload connecté => récupération des hébergeurs avec le login
        r = m_networkAccessManager->post(requete, QString("username=" +
                                                          m_login +
                                                          "&password=" +
                                                          m_password
                                                          ).toLatin1());
    }

    connect(r, SIGNAL(finished()), this, SLOT(finRecupHebergeurs()));
}

void RecupHebergeurs::finRecupHebergeurs()
{
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender());

    qDebug() << "RecupHebergeurs :: Analyse de la liste des hebergeurs en cours...";

    // Récupération de la page
    QByteArray temp = r->readAll();

    // Traitement des erreurs éventuelles
    switch (r->error()) {
        // Doc de l'énumération : http://qt.developpez.com/doc/4.7/qnetworkreply/#networkerror-enum
        case QNetworkReply::NoError :   //qDebug() << "RecupHebergeurs :: Pas d'erreurs, poursuite";
                                        r->deleteLater();
                                        m_statutConnexion = Bad; // A la base on est pas sûr d'analyser correctement la chaine...
                                        break;

        default :                       qDebug() << "RecupHebergeurs :: Erreur lors du chargement. Code de l'erreur : " << r->errorString();
                                        r->deleteLater();
                                        m_statutConnexion = Error;
                                        this->finProcedure();
                                        return;
                                        break;
    }

    /* V4
    {
        "error":"success",
        "hosts":
                {
                    "1fichier.com": {
                                        "selected":"true",
                                        "size":5120
                                    },
                    "easybytez.com":{
                                        "selected":"false",
                                        "size":5120
                                    }
                },
        "default":
                [
                    "dl.free.fr",
                    "uptobox.com",
                    "uploaded.net",
                    "ezfile.ch",
                    "1fichier.com",
                    "turbobit.net",
                    "hugefiles.net",
                    "rapidgator.net",
                    "mega.co.nz",
                    "zippyshare.com",
                    "2shared.com",
                    "filesupload.org",
                    "uplea.com",
                    "clicknupload.com",
                    "uploadable.ch",
                    "oboom.com",
                    "filerio.in",
                    "userscloud.com"
                ],
        "maxHosts":18
    }
    */
    QJsonDocument loadDoc(QJsonDocument::fromJson(temp));

    if (!loadDoc.isNull()) { // Test de validité

        QJsonObject connexionObj = loadDoc.object();

        if (connexionObj["error"].toString() == "success") {
            // Succès certain

            // Récupération de l'objet liste de hosts
            QJsonObject hosts(connexionObj["hosts"].toObject());

            int numero_host = 0;
            // Here's how to iterate over a QMap<QString, int> using an iterator
            // http://qt-project.org/doc/qt-4.8/qmap.html
            // http://doc.qt.io/qt-5/containers.html#stl-style-iterators
            QJsonObject::const_iterator hosts_it = hosts.constBegin();
            while (hosts_it != hosts.constEnd()) {

                // Objet host
                QJsonObject host(hosts[hosts_it.key()].toObject());
                //qDebug() << hosts_it.key() << ":" << host;

                // Stockage du texte affiché sur la checkbox => nom du host
                QString nom = hosts_it.key();
                nom[0] = nom.at(0).toUpper();

                // Etat de sélection
                bool etat_selection = (host["selected"] == "true") ? true : false;

                // Création checkbox
                emit this->emissionRecupHebergeursHebergeurs(
                            hosts_it.key(),
                            nom + " (" + QString::number(host["size"].toInt(0)) + " Mo)",
                            etat_selection,
                            numero_host);

                // Récupération de l'icone via son URL
                QUrl url(URL_RECUPERATION_ICONES + hosts_it.key() + ".png");
                emit this->emissionUrlIcone(url, numero_host);

                ++hosts_it;
                numero_host++;
            }
            // Permet l'émission du signal avec le paramètre 1 = réussite
            // Voir finProcedure()
            m_statutConnexion = Ok;

            // Nombre maximal d'hébergeurs à sélectionner
            int maxHosts = connexionObj["maxHosts"].toInt();
            qDebug() << "RecupHebergeurs :: Nombre max d'hébergeurs autorisés: " << maxHosts;
            emit this->emissionMaxSelectionHebergeurs(maxHosts);
        }

        // Echec certain
        // m_statutConnexion reste à Bad
    }

    this->finProcedure();
}

void RecupHebergeurs::finProcedure()
{
    //BAD,OK ERROR ?
    switch (m_statutConnexion) {

        case Bad:   qDebug() << "RecupHebergeurs :: Echec d'analyse de la chaine";
                    emit this->emissionRecupHebergeursEtat(0);
                    emit this->finished();
                    break;

        case Ok:    //qDebug() << "RecupHebergeurs :: Ok";
                    emit this->emissionRecupHebergeursEtat(1);
                    // Since no icon is retrieved we finish this thread here
                    emit this->finished();
                    break;

        default:    qDebug() << "RecupHebergeurs :: Erreur inconnue";
                    emit this->emissionRecupHebergeursEtat(2);
                    emit this->finished();
                    break;
    }
}
