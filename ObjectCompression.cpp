#include "ObjectCompression.h"


Compression::Compression(const QString &cheminWinRar, const QStringList &arguments, QObject *parent): QObject(parent)
{
    qDebug() << "Compression :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    m_cheminWinRar  = cheminWinRar;
    m_arguments     = arguments;
    m_compression   = false;

    //PS: Rien d'autre ne peut être initialisé ici...
    //Surtout pas le QNetworkAccessManager car :
    //nous sommes encore dans un objet appartenant au thread parent
    //l'attribut sera lu dans le thread enfant
    //ex:
    //QObject: Cannot create children for a parent that is in a different thread.
    //(Parent is QNetworkAccessManager(0x89cc4e0), parent's thread is QThread(0x8930af8), current thread is QThread(0x89f3b38)

//PS: ce message notifiant 3 threads, est probablement du au fait que l'initialisation de cet objet se fait par défaut détaché de tout: QObject *parent = NULL...
}

Compression::~Compression()
{
    qDebug() << "Compression :: Destruction Compression en cours...";
    delete m_builder;
}

void Compression::demarrage()
{
    qDebug() << "Compression :: Début de la compression...";
    qDebug() << "Compression :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    m_builder = new QProcess;

    //rar a -hpwww.wawapowa.com -v760m -m0 -ep /root/Desktop/$nom_sortie "$1"
    // Fusion de la sortie standard et de la sortie d'erreur standard vers la sortie standard du processus
    m_builder->setProcessChannelMode(QProcess::MergedChannels);

    // Mettre ça avant ou après le démarrage ?
    connect(m_builder, SIGNAL(readyReadStandardOutput()), this, SLOT(lectureSortieStandard()));
    connect(m_builder, SIGNAL(finished(int)), this, SLOT(finProcedure(int)));

    // Démarrage
    m_builder->start(m_cheminWinRar, m_arguments);
}

void Compression::lectureSortieStandard()
{
    QByteArray ligne = m_builder->readLine();
    qDebug() << "Compression :: " << ligne;

    // Récupération de la valeur de l'avancement
    QRegExp rx(".*([0-9]{1,3}).*");
    if (rx.indexIn(ligne) != -1) {
        //qDebug() << "Compression :: " << rx.cap(1);

        emit this->emissionCompressionAvancement(rx.cap(1).toInt());
    }
    else {
        // Récupération de la réussite
        rx.setPattern(".*(Done|Termin|OK).*");
        if (rx.indexIn(ligne) != -1) {
            qDebug() << "Compression :: Fin trouvée : " << rx.cap(1);

            // On spécifie que la compression a réussi
            //Ce booléen servira   envoyer le signal emissionCompressionEtat(false) à la fin du processus
            m_compression = true;
        }
// Ne marche pas, je ne comprends pas :(
// EDIT: marche mais pas tout le temps... le flux dans stdout est probablement trop rapide...
        // Récupération de l'étape
        rx.setPattern((".*(Creating archive).*"));
        if (rx.indexIn(ligne) != -1) {
            qDebug() << "Compression :: Etape trouvée : " << rx.cap(1);

            emit this->emissionCompressionEtape(rx.cap(1));
        }
        //else if erreur etc.... renvoit de 0
    }
}

void Compression::finProcedure(int codeRetour)
{
    // Ici on ne se sert pas du code de retour du processus..
    //Peut être dans le futur. Le truc c'est que je doute que le code soit le même sous linux et sous windows..
    //EDIT: 0 dans tous les cas, sauf plantage...
    // Ce signal n'est envoyé que si le processus est terminé donc on peut amorcer la destruction de la classe après


// Le pb sous windows consistant à une absence de récupération de l'état de la compression est peut etre du à une fermeture prématurée du thread..
// Dans ce cas, FinProcedure serait appelé alors que lectureSortieStandard() n'aurait pas terminé son travail... à voir...
// si on est SUR que les Done ou Terminé peuvent etre récupérés, on peut mettre un booléen qui permettrait ici de faire par ex :
//
// while(!m_ready) msleep(50); // Attente tant que le résultat n'a pas été trouvé

    qDebug() << "Compression :: codeRetour:" << codeRetour;

    // Envoi d'un signal disant que le processus a échoué ou pas
    emit this->emissionCompressionEtat(m_compression);
}
