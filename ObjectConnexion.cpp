#include "ObjectConnexion.h"

Connexion::Connexion(const QString &login, const QString &password, QObject *parent): QObject(parent)
{
    qDebug() << "Connexion :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    m_login     = login;
    m_password  = password;

    m_etape     = 0;

    //typedef enum EtatConnexion EtatConnexion; // Statut de la connexion BAD,OK,ERROR
    //m_statutConnexion = BAD; // Par défaut la connexion est un échec.

    //PS: Rien d'autre ne peut être initialisé ici...
    //Surtout pas le QNetworkAccessManager car :
    //nous sommes encore dans un objet appartenant au thread parent
    //l'attribut sera lu dans le thread enfant
    //ex:
    //QObject: Cannot create children for a parent that is in a different thread.
    //(Parent is QNetworkAccessManager(0x89cc4e0), parent's thread is QThread(0x8930af8), current thread is QThread(0x89f3b38)

//PS: ce message notifiant 3 threads, est probablement du au fait que l'initialisation de cet objet se fait par défaut détaché de tout: QObject *parent = NULL...
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
    requete.setRawHeader("User-Agent","Mozilla/5.0 (Windows NT 5.1; rv:10.0.2) Gecko/20100101 Firefox/10.0.2");
    requete.setRawHeader("Accept-Language","fr,fr-fr;q=0.8,en-us;q=0.5,en;q=0.3");
    requete.setRawHeader("Accept-Charset","ISO-8859-1,utf-8;q=0.7,*;q=0.7");
    //pour Qt 4.7.x :
    requete.setRawHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
    //pour Qt 4.7.8 :
    //requete.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    if (m_etape == 0) {
        qDebug() << "Connexion :: Connexion en cours...";

        const QUrl url = QUrl(URL_CONNEXION_UTILISATEUR);
        requete.setUrl(url);

        //initialisation du QNetworkAccessManager
        m_networkAccessManager = new QNetworkAccessManager;

        // Méthode post() pour poster le contenu de notre requête.
        //v1
        //_method=POST&data%5BUtilisateur%5D%5Butilisateur%5D=NOM&data%5BUtilisateur%5D%5BmotDePasse%5D=MDP
        //v2
        //_csrf_token=69b27426be5296fef27a1817f0b022ea9c2cc6e3&_username=login&_password=pass&_submit=Login
        QNetworkReply *r = m_networkAccessManager->post(requete, QString("username=" +
                                                    m_login +
                                                    "&password=" +
                                                    m_password
                                                    ).toLatin1());

        connect(r, SIGNAL(finished()), this, SLOT(finLogin()));
        // Voir explications sur la désactivation dans la fonction :
        //connect(r, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(erreur(QNetworkReply::NetworkError)));
    }
    else if (m_etape == 1) {
        qDebug() << "Connexion :: Récupération des droits en cours...";

        const QUrl url = QUrl(URL_RECUPERATION_DROITS);
        requete.setUrl(url);

        // Pas d'initialisation du QNetworkAccessManager (déjà fait dans l'étape 0) => pas de vérification de sa présence ?
        //V1
        //QNetworkReply *r = m_networkAccessManager->get(requete);
        //V2
        QNetworkReply *r = m_networkAccessManager->post(requete, QString("username=" +
                                                                         m_login +
                                                                         "&password=" +
                                                                         m_password
                                                                         ).toLatin1());

        connect(r, SIGNAL(finished()), this, SLOT(finServeurWwpw()));
        // Voir explications sur la désactivation dans la fonction :
        //connect(r, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(erreur(QNetworkReply::NetworkError)));
    }
}

void Connexion::finLogin()
{
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender()); //On récupère la réponse du serveur

    qDebug() << "Connexion :: Récupération de la réponse du serveur...";

    // Récupération de la page
    //V3
    QByteArray temp = r->readAll();
    //qDebug() << temp;

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
    */

    //"{"error":"bad username OR bad password"}"
    //"{"error":"success","login":"lex","user":1637}"
    QJsonDocument loadDoc(
                QJsonDocument::fromJson(temp));

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

        //"bad username OR bad password"
        // Echec certain
        // m_statutConnexion reste à Bad
    }


    // Emission des signaux
    //Notons que m_statutConnexion peut être égal à BAD si pas trouvé l'id;
    //ou à OK si l'id a été trouvé.
    this->finProcedure();
}

void Connexion::finServeurWwpw()
{
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender());

    qDebug() << "Connexion :: Récupération de la page de droits...";

    // Récupération de la page
    QString temp = r->readAll();
    //qDebug() << temp;

    // Traitement des erreurs éventuelles
    switch (r->error()) {
        // Doc de l'énumération : http://qt.developpez.com/doc/4.7/qnetworkreply/#networkerror-enum
        case QNetworkReply::NoError :   qDebug() << "Connexion :: Pas d'erreurs, poursuite";
                                        r->deleteLater();
                                        m_statutConnexion = Bad; // A la base il n'y a pas de droits (doublon avec le else plus bas)
                                        break;

        default :                       qDebug() << "Connexion :: Erreur lors du chargement. Code de l'erreur : " << r->errorString();
                                        r->deleteLater();
                                        m_statutConnexion = Error;
                                        this->finProcedure();
                                        return;
                                        break;
    }

    // V2
    //???

    // V1
    // Détection de la présence de ce mot clé sur la page
    QRegExp rx("(debridpowa.com)");

    if (rx.indexIn(temp) != -1) {
        qDebug() << "Connexion :: Nb d'occurences trouvées :" << rx.captureCount();
        // Emission du signal permettant l'affichage de la checkbox cachée
        m_statutConnexion = Ok;
    }
    else {
        qDebug() << "Connexion :: Pas de droits";
        // A la base il n'y a pas de droits:
        // Emission du signal permettant de masquer la checkbox si elle était visible
        m_statutConnexion = Bad;
    }

    this->finProcedure();
}
/*
void Connexion::erreur(QNetworkReply::NetworkError) // Fonction dépassée, sans utilité pour le programme, voir plus bas.
{
    // Parfois ça a le temps d'envoyer   la fois le signal error et finished....
    // Dans la doc :
    // This signal is emitted when the reply detects an error in processing. The finished() signal will probably follow, indicating that the connection is over.
    // Super...
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
    if (m_etape == 0) {
        //BAD,OK ERROR ?
        switch (m_statutConnexion) {

            case Bad:   qDebug() << "Connexion :: Mauvais login";
                        emit this->emissionLoginEtat(0);
                        emit this->finished();
                        break;

            case Ok:    qDebug() << "Connexion :: Bon login";
                        //emit this->emissionLoginCookies(m_cookieListe);
                        emit this->emissionLoginEtat(1);
                        m_etape = 1;
                        this->demarrage();
                        // pas de signal finished car on poursuit la procédure par l'obention des droits Wwpw
                        break;

            case Error: qDebug() << "Connexion :: Erreur login";
                        emit this->emissionLoginEtat(2);
                        emit this->finished();
                        break;

            default:    qDebug() << "Connexion :: Erreur inconnue login";
                        emit this->emissionLoginEtat(2);
                        emit this->finished();
                        break;
        }
    }
    else { // m_etape = 1 => 2ieme procédure
        //BAD,OK ERROR ?
        switch (m_statutConnexion) {

            case Bad:   qDebug() << "Connexion :: Droits refusés";
                        emit this->emissionDroitsServeurWwpw(false);
                        emit this->finished();
                        break;

            case Ok:    qDebug() << "Connexion :: Droits acceptés";
                        emit this->emissionDroitsServeurWwpw(true);
                        emit this->finished();
                        break;

            case Error: qDebug() << "Connexion :: Droits erreur";
                        emit this->emissionLoginEtat(2);
                        emit this->finished();
                        break;

            default:    qDebug() << "Connexion :: Droits erreur inconnue";
                        emit this->emissionLoginEtat(2);
                        emit this->finished();
                        break;
        }
    }
}
