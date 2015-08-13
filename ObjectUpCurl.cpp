#include "ObjectUpCurl.h"

using namespace std;

// REVOIR ça ! structure globale dans tous les fichiers (extern) => très sale !
struct Bricolage        bricolo;

UpCurl::UpCurl(const string &FileName, const string &Url, QStringList hebergeursListe, const string &loginId, QObject *parent): QObject(parent)
{
    qDebug() << "Curl :: Mode connecté : Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();


    m_curlFileName      = FileName;
    m_curlUrl           = Url;
    m_hebergeursListe   = hebergeursListe;
    m_loginId           = loginId;
    m_connecte          = true;

    m_averageSpeed = 0.0;
    m_dureeTransfert = 0;

    bricolo.upTotal = 0;
    bricolo.upFait = 0;
    bricolo.upState = 0;
    bricolo.lien = "";
    bricolo.lienDelete = "";
}

UpCurl::UpCurl(const string &FileName, const string &Url, QStringList hebergeursListe, QObject *parent): QObject(parent)
{
    qDebug() << "Curl :: Mode anonyme : Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();


    m_curlFileName      = FileName;
    m_curlUrl           = Url;
    m_hebergeursListe   = hebergeursListe;
    m_connecte          = false;

    m_averageSpeed = 0.0;
    m_dureeTransfert = 0;

    bricolo.upTotal = 0;
    bricolo.upFait = 0;
    bricolo.upState = 0;
    bricolo.lien = "";
    bricolo.lienDelete = "";
}

UpCurl::~UpCurl()
{
    qDebug() << "Curl :: Destruction UpCurl en cours...";

    //delete m_timer;

    //curl_easy_cleanup(m_hCurl);

    // La destruction de ces attributs pose problème.
    //Mismatched free() / delete / delete []

    delete m_post;
    delete m_last;

    // Je détruis quand même ces attributs mais via les fonctions fournies par curl.
    //ça ne change rien valgrind raporte encore les lignes...
    // Mais la doc est formelle...

    //curl_formfree(m_post);
    //curl_formfree(m_last);
}

void UpCurl::demarrage()
{
    qDebug() << "Curl :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    //---------- Progression => placée dans MainWindow pour cause de pbs qui y sont mentionnés..
    // Appeler une structure statique contenant ces variables
    //les valeurs sont envoyes rgulirement par un Qtimer (1000 ms)
    // D'après la doc de curl, la fonction progress_func() est appelée toutes les secondes parfois moins.
    /*m_timer = new QTimer(this);
    m_timer->setInterval(1000);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(emissionProgressionSlot()));

    m_timer->start();*/

    //---------- Curl
    CURLcode hResult;

    // Réinitialisation des params de curl
    // Curl for form data
    m_post = NULL;
    m_last = NULL;

    // Initialize curl, just don't let easy_init to do it for you
    #ifdef WINDOWS
        curl_global_init(CURL_GLOBAL_WIN32);
    #endif
    #ifdef LINUX
        curl_global_init(CURL_GLOBAL_ALL);
    #endif


    // Handle to the curl
    m_hCurl = curl_easy_init();

    // Ajoute le nom d'utilisateur à la requête de curl (v2)
    if (m_connecte == true) {
        qDebug() << "Curl :: Login :" << QString::fromStdString(m_loginId);

        curl_formadd(&m_post,
                     &m_last,
                     CURLFORM_COPYNAME,
                     "user",
                     CURLFORM_COPYCONTENTS,
                     m_loginId.c_str(),
                     CURLFORM_END);
    }

    // Ajoute les hébergeurs à la requête d'upload de curl
    for (int i = 0; i < m_hebergeursListe.count(); i++)
    {
        qDebug() << "Curl :: Hébergeur :" << QString(m_hebergeursListe[i]);

        curl_formadd(&m_post,
                     &m_last,
                     CURLFORM_COPYNAME,
                     QString(m_hebergeursListe[i]).toStdString().c_str(),
                     CURLFORM_COPYCONTENTS,
                     "true",
                     CURLFORM_END); // Apparait dans valgrind..
    }

    // Ajoute le fichier  la requête d'upload de curl (file modifié en files[] pour la v2)
    curl_formadd(&m_post,
                 &m_last,
                 CURLFORM_COPYNAME,
                 "files[]",
                 CURLFORM_FILE,
                 m_curlFileName.c_str(),
                 CURLFORM_END); // Apparait dans valgrind..

    //Specify the API Endpoint
    hResult = curl_easy_setopt(m_hCurl, CURLOPT_URL, m_curlUrl.c_str());

    //Progression active et fonction
    hResult = curl_easy_setopt(m_hCurl, CURLOPT_NOPROGRESS, 0);
    hResult = curl_easy_setopt(m_hCurl, CURLOPT_PROGRESSFUNCTION, &progress_func);

    //Rcupration de la rponse du serveur
    hResult = curl_easy_setopt(m_hCurl, CURLOPT_WRITEFUNCTION, write_data);

    //UserAgent
    hResult = curl_easy_setopt(m_hCurl, CURLOPT_USERAGENT, "User-Agent: Mozilla/5.0 (Windows NT 5.1; rv:10.0.2) Gecko/20100101 Firefox/10.0.2");

    //Specify the HTTP Method
    hResult = curl_easy_setopt(m_hCurl, CURLOPT_HTTPPOST, m_post);

    //Excution de la requte (envoi du fichier et rception du lien)
    qDebug() << "Curl :: Dmarrage de la requete..";
    hResult = curl_easy_perform(m_hCurl);

    // paramètres renvoyés à la fin de l'up:
    // "Use this function AFTER a performed transfer if you want to get transfer- oriented data"
    // "The data pointed-to will be filled in [...] only if the function returns CURLE_OK"
    // => http://curl.haxx.se/libcurl/c/curl_easy_getinfo.html
    if (hResult == CURLE_OK) { 
        curl_easy_getinfo(m_hCurl, CURLINFO_SPEED_UPLOAD, &m_averageSpeed);
        qDebug() << "Curl :: Vitesse moyenne renvoyée :" << m_averageSpeed << "bytes/sec";

        curl_easy_getinfo(m_hCurl, CURLINFO_TOTAL_TIME, &m_dureeTransfert);
        qDebug() << "Curl :: Durée upload renvoyée :" << m_dureeTransfert;
    }

    //Arret d'envoi de la progression
    //m_timer->stop();

    curl_easy_cleanup(m_hCurl);
// Ces 2 lignes qui suivent, apparaissent dans Valgrind. pourtant c'est dans la doc de Curl :-( => déplacées dans le destructeur
    //curl_formfree(m_post);
    //curl_formfree(m_last);
    curl_global_cleanup();

    finProcedure(hResult);
}

void UpCurl::pause()
{
    curl_easy_pause(m_hCurl, CURLPAUSE_SEND);
    //qDebug() << "Curl :: pause";
}

void UpCurl::unpause()
{
    curl_easy_pause(m_hCurl, CURLPAUSE_CONT);
    //qDebug() << "Curl :: unpause";
}

int UpCurl::progress_func(void *ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded)
{
    // It's here you will write the code for the progress message or bar

    //appeler une structure statique contenant ces variables
    //les valeurs sont envoyées à l'objet MainWindow régulièrement par un Qtimer
    UNUSED(ptr);
    UNUSED(TotalToDownload);
    UNUSED(NowDownloaded);

    bricolo.upTotal     = TotalToUpload;
    bricolo.upFait      = NowUploaded;

    return bricolo.upState;
}

size_t UpCurl::write_data(void *buffer, size_t size, size_t nmemb, void *userdata)
{
    //PS: fonction statique => on ne peut pas se servir des attributs/fonctions/signaux de la classe en cours..
    // Obligé de faire appel à une structure statique pour la remplir et s'en servir...
    UNUSED(userdata);

    stringstream strmResponse;
    size_t nReal = size * nmemb;
    strmResponse << string((char*)buffer, size*nmemb);
    string sLine("");

    QByteArray temp;

    while (getline(strmResponse, sLine)) {
        //utiliser la surcharge += ? => si jamais la réponse fait plusieurs lignes ...
        temp += sLine.c_str();
    }

    // Module de triage du lien (susceptible de changer de place ça..)
    // A l'avenir il est possible qu'on doive prendre là dedans, autre chose que le lien...
    //"[{"name":"test.txt","hash":"858ba0d4e80394e48bb9095de9ee93e8","size":7,"type":"text\/plain","sid":"","hashUpload":"","md5":"fcf1456d8927054e48c6024236c9960b","sha":"458afc80eb73e4aa1ccdd307aa51341b13091cb7","user":"","url":"http:\/\/www.multiup.org\/download\/858ba0d4e80394e48bb9095de9ee93e8\/test.txt","delete_url":"http:\/\/lenny.multiup.org\/upload\/?file=test.txt","delete_type":"DELETE"}]"

    // Suppression de [,] aux extrémités pour que ça soit du json exploitable...
    temp.remove(0,1);
    temp.remove(temp.size()-1,1);

    QJsonDocument loadDoc(
                QJsonDocument::fromJson(temp));

    if (!loadDoc.isNull()) { // Test de validité

        QJsonObject connexionObj = loadDoc.object();

        // Récupération du statut
        QString url = connexionObj["url"].toString();

        qDebug() << "Curl :: Lien :" << url;
        bricolo.lien = url;

        //url = connexionObj["delete_url"].toString();
        //qDebug() << "Curl :: LienDelete :" << url;
        bricolo.lienDelete = "";

    }

    return nReal;
}

void UpCurl::arretCurl()
{
    // Cette manip permet à la fonction progress_func() de renvoyer -1,
    //ce qui d'après la doc entraine un arret forcé de l'up.
    //=> "will cause libcurl to abort the transfer and return CURLE_ABORTED_BY_CALLBACK."
    qDebug() << "Curl :: Arret demandé";

    bricolo.upState = -1;
}

void UpCurl::emissionProgressionSlot()
{
    qDebug() << "Curl :: Emission progression";
// /!\ Fameux signal jamais envoyé..
//utiliser des mutex ? voir QMutexLocker et QMutex...

    emit this->emissionProgression(bricolo.upTotal, bricolo.upFait);
}

void UpCurl::finProcedure(CURLcode hResult)
{
    switch(hResult)
    {
        case CURLE_ABORTED_BY_CALLBACK: qDebug() << "Curl :: Arret";
                                        emit this->emissionEtatUpCurl(0);
                                        emit this->finished();
                                        break;

        case CURLE_OK:                  qDebug() << "Curl :: Terminé sans erreur";
                                        emit this->emissionLienEtStats(bricolo.lien, m_averageSpeed, m_dureeTransfert);
                                        emit this->emissionEtatUpCurl(1);
                                        emit this->finished();
                                        break;

        default:                        qDebug() << "Curl :: Erreur" << hResult;
                                        emit this->emissionEtatUpCurl(2);
                                        emit this->finished();
                                        break;
    }
}
