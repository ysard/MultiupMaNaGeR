#include "ObjectMiseAJour.h"

MiseAJour::MiseAJour(QObject *parent): QObject(parent)
{
    // Nothing
    m_etape = 0;
}

MiseAJour::~MiseAJour()
{
    qDebug() << "MiseAJour :: Destruction MiseAJour en cours...";

    delete m_networkAccessManager;
}

void MiseAJour::demarrage()
{
    // !!!! Slot appelé lorsque on s'est assuré que le thread acceuillant cette classe dérivée de QObject est lancé !!!!
    qDebug() << "MiseAJour :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    // On crée notre requête
    QNetworkRequest requete;
    requete.setRawHeader("User-Agent", USER_AGENT);
    requete.setRawHeader("Accept-Language","fr,fr-fr;q=0.8,en-us;q=0.5,en;q=0.3");
    requete.setRawHeader("Accept-Charset","ISO-8859-1,utf-8;q=0.7,*;q=0.7");
    //pour Qt 4.7.x :
    requete.setRawHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
    //pour Qt 4.7.8 :
    //requete.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    if (m_etape == 0) {
        qDebug() << "MiseAJour :: Récupération infos version en cours...";
        #ifdef LINUX
            const QUrl url = QUrl(URL_MAJ_LINUX);
        #endif
        #ifdef WINDOWS
            const QUrl url = QUrl(URL_MAJ_WINDOWS);
        #endif
        requete.setUrl(url);

        // Initialisation du QNetworkAccessManager
        m_networkAccessManager = new QNetworkAccessManager;

        // Méthode get() pour poster le contenu de notre requête.
        QNetworkReply *r = m_networkAccessManager->get(requete);

        connect(r, SIGNAL(finished()), this, SLOT(finRecupVersion()));
    }
    else if (m_etape == 1) {
        qDebug() << "MiseAJour :: Récupération de l'application...";

        const QUrl url = QUrl(m_donneesMaj.at(1));
        requete.setUrl(url);

        // Pas d'initialisation du QNetworkAccessManager (déjà fait dans l'étape 0) => pas de vérification de sa présence ?
        QNetworkReply *r = m_networkAccessManager->get(requete);

        connect(r, SIGNAL(finished()), this, SLOT(finMiseAJour()));
        connect(r, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(progressionDownload(qint64,qint64)));
    }
}

void MiseAJour::finRecupVersion()
{
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender()); //On récupère la réponse du serveur

    qDebug() << "MiseAJour :: Récupération de la réponse du serveur...";

    // Traitement des erreurs éventuelles
    switch (r->error()) {
        // Doc de l'énumération : http://qt.developpez.com/doc/4.7/qnetworkreply/#networkerror-enum
        case QNetworkReply::NoError :   qDebug() << "MiseAJour :: Pas d'erreurs, poursuite";
                                        m_statutConnexion = Bad; // A la base, fichier non conforme
                                        break;

        default :                       qDebug() << "MiseAJour :: Erreur lors du chargement. Code de l'erreur : " << r->errorString();
                                        r->deleteLater();
                                        m_statutConnexion = Error;
                                        this->finProcedure();
                                        return;
                                        break;
    }

    // Récupération de la page
    // Le fichier doit être de la forme:
    //Numéro_de_version;lien_de_telechargement;checksum_md5
    QString temp = r->readAll();
    m_donneesMaj = temp.split(";");
    r->deleteLater();

    // On s'assure qu'on a bien un fichier de données contenant 3 champs,
    //et que le champ de version est différent de la version actuelle du programme
    if ((m_donneesMaj.count() == 3) && (QString::compare(QString(VERSION), m_donneesMaj.at(0)))) {
        qDebug() << "MiseAJour :: Données récupérées : version:" << m_donneesMaj.at(0) << "lien:" << m_donneesMaj.at(1) << "checksum md5:" << m_donneesMaj.at(2);
        m_statutConnexion = Ok;
    }

    this->finProcedure();
}

void MiseAJour::finMiseAJour()
{
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender()); //On récupère la réponse du serveur

    qDebug() << "MiseAJour :: Récupération de la réponse du serveur...";


    // Traitement des erreurs éventuelles
    switch (r->error()) {
        // Doc de l'énumération : http://qt.developpez.com/doc/4.7/qnetworkreply/#networkerror-enum
        case QNetworkReply::NoError :   qDebug() << "MiseAJour :: Pas d'erreurs, poursuite";
                                        m_statutConnexion = Bad; // A la base, pas d'ip trouvée
                                        break;

        default :                       qDebug() << "MiseAJour :: Erreur lors du chargement. Code de l'erreur : " << r->errorString();
                                        r->deleteLater();
                                        m_statutConnexion = Error;
                                        this->finProcedure();
                                        return;
                                        break;
    }

    // Récupération de la page
    QByteArray in = r->readAll();
    r->deleteLater();

    // Calcul checksum md5
    // Méthode décrite pour ls gros fichiers:
    //http://www.qtcentre.org/threads/47635-Calculate-MD5-sum-of-a-big-file
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(in);

    qDebug() << "MiseAJour :: Cheksum trouvé:" << hash.result().toHex() << "Checksum à trouver:" << m_donneesMaj.at(2);
    if (hash.result().toHex() == m_donneesMaj.at(2))
    {
        // Enregistrement du fichier
        #ifdef LINUX
                QFile file(QApplication::applicationDirPath() + "/multiup_new");
        #endif
        #ifdef WINDOWS
                QFile file(QApplication::applicationDirPath() + "/multiup_new.exe");
        #endif
        if(file.open(QIODevice::WriteOnly)) {
            file.write(in);
            file.close();

            m_statutConnexion = Ok;
        }
    }

    this->finProcedure();
}

void MiseAJour::progressionDownload(qint64 bytesReceived, qint64 bytesTotal)
{
    int valeur = ((bytesReceived*100) / bytesTotal);

    //qDebug() << "MiseAJour :: Progression en cours :" << bytesReceived << "/" << bytesTotal << "Total :" << valeur << "%";

    emit this->updateProgressBar(valeur);
}

void MiseAJour::receptionDemandeArret()
{
    qDebug() << "MiseAJour :: Reception demande arret";
    m_statutConnexion = Error;
    this->finProcedure();
}

void MiseAJour::finProcedure()
{
    switch (m_statutConnexion) {

        case Bad:   qDebug() << "MiseAJour :: Fichier non conforme";
                    emit this->emissionMiseAJourEtat(0);
                    emit this->finished();
                    break;

        case Ok:    qDebug() << "MiseAJour :: Fichier conforme";
                    if (m_etape == 0) {
                        emit this->emissionVersion(m_donneesMaj.at(0)); // remplace l'émission de emissionMiseAJourEtat(1)
                        m_etape = 1; // n'engage à rien. Si l'utilisateur refuse la maj dans le QDialog tout est détruit
                    }
                    else {
                        emit this->emissionMiseAJourEtat(1);
                        emit this->finished();
                    }
                    break;

        case Error: qDebug() << "MiseAJour :: Erreur";
                    emit this->emissionMiseAJourEtat(2);
                    emit this->finished();
                    break;

        default:    qDebug() << "MiseAJour :: Erreur inconnue";
                    emit this->emissionMiseAJourEtat(2);
                    emit this->finished();
                    break;
    }
}

