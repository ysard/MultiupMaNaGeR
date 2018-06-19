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

    // La destruction de ces attributs pose problème.
    // Mismatched free() / delete / delete []
    delete m_post;
    delete m_last;

    // Destruction de ces attributs via les fonctions fournies par curl
    // Valgrind rapporte encore ces lignes...
    // Mais la doc est formelle.
    //curl_formfree(m_post);
    //curl_formfree(m_last);
}

void UpCurl::demarrage()
{
    qDebug() << "Curl :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    //---------- Progression => placée dans MainWindow pour cause de pbs qui y sont mentionnés..
    // Appeler une structure statique contenant ces variables
    // les valeurs sont envoyes rgulirement par un Qtimer (1000 ms)
    // D'après la doc de curl, la fonction progress_func() est appelée toutes les secondes parfois moins.
    /*
    m_timer = new QTimer(this);
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

    // Ajoute le fichier à la requête d'upload de curl (file modifié en files[] pour la v2)
    curl_formadd(&m_post,
                 &m_last,
                 CURLFORM_COPYNAME,
                 "files[]",
                 CURLFORM_FILE,
                 m_curlFileName.c_str(),
                 CURLFORM_END); // Apparait dans valgrind..

    // Specify the API Endpoint
    hResult = curl_easy_setopt(m_hCurl, CURLOPT_URL, m_curlUrl.c_str());

    // Progression active et callback
    hResult = curl_easy_setopt(m_hCurl, CURLOPT_NOPROGRESS, 0);
    hResult = curl_easy_setopt(m_hCurl, CURLOPT_PROGRESSFUNCTION, &progress_func);

    // Récupération de la réponse du serveur
    hResult = curl_easy_setopt(m_hCurl, CURLOPT_WRITEFUNCTION, write_data);

    // User-Agent
    hResult = curl_easy_setopt(m_hCurl, CURLOPT_USERAGENT, USER_AGENT);

    // Specify the HTTP Method
    hResult = curl_easy_setopt(m_hCurl, CURLOPT_HTTPPOST, m_post);

    // Exécution de la requte (envoi du fichier et réception du lien)
    qDebug() << "Curl :: Dmarrage de la requete..";
    hResult = curl_easy_perform(m_hCurl);

    // Paramètres renvoyés à la fin de l'up:
    // "Use this function AFTER a performed transfer if you want to get transfer- oriented data"
    // "The data pointed-to will be filled in [...] only if the function returns CURLE_OK"
    // => http://curl.haxx.se/libcurl/c/curl_easy_getinfo.html
    if (hResult == CURLE_OK) { 
        curl_easy_getinfo(m_hCurl, CURLINFO_SPEED_UPLOAD, &m_averageSpeed);
        qDebug() << "Curl :: Vitesse moyenne renvoyée :" << m_averageSpeed << "bytes/sec";

        curl_easy_getinfo(m_hCurl, CURLINFO_TOTAL_TIME, &m_dureeTransfert);
        qDebug() << "Curl :: Durée upload renvoyée :" << m_dureeTransfert;
    }

    // Arret d'envoi de la progression
    // m_timer->stop();

    curl_easy_cleanup(m_hCurl);
    // Ces 2 lignes, apparaissent dans Valgrind.
    // Pourtant c'est dans la doc de Curl :-( => déplacées dans le destructeur
    // curl_formfree(m_post);
    // curl_formfree(m_last);
    curl_global_cleanup();

    finProcedure(hResult);
}

void UpCurl::pause()
{
    // See http://curl.haxx.se/libcurl/c/curl_easy_pause.html
    curl_easy_pause(m_hCurl, CURLPAUSE_SEND);
    //qDebug() << "Curl :: pause";
}

void UpCurl::unpause()
{
    // See http://curl.haxx.se/libcurl/c/curl_easy_pause.html
    curl_easy_pause(m_hCurl, CURLPAUSE_CONT);
    //qDebug() << "Curl :: unpause";
}

int UpCurl::progress_func(void *ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded)
{
    // It's here you will write the code for the progress message or bar
    // Appeler une structure statique contenant ces variables
    // les valeurs sont envoyées à l'objet MainWindow régulièrement par un Qtimer
    UNUSED(ptr);
    UNUSED(TotalToDownload);
    UNUSED(NowDownloaded);

    bricolo.upTotal     = TotalToUpload;
    bricolo.upFait      = NowUploaded;

    return bricolo.upState;
}

size_t UpCurl::write_data(void *buffer, size_t size, size_t nmemb, void *userdata)
{
    // NOTE: fonction statique => on ne peut pas se servir des attributs/fonctions/signaux de la classe en cours.
    // Obligé de faire appel à une structure statique pour la remplir et s'en servir.
    UNUSED(userdata);

    stringstream strmResponse;
    size_t nReal = size * nmemb;
    strmResponse << string((char*)buffer, size*nmemb);
    string sLine("");

    QByteArray temp;
    while (getline(strmResponse, sLine)) {
        // Surcharge += : si jamais la réponse fait plusieurs lignes
        temp += sLine.c_str();
    }

    // Triage du lien
    /* Exemple de JSON retourné:
    {
       "files":[
          {
             "name":"test.txt",
             "size":5,
             "type":"text/plain",
             "hash":"080e3fb87debea6e9818d55aa1c68b43",
             "user":"1637",
             "md5":".",
             "sha":".",
             "url":"https://multiup.org/download/080e3fb87debea6e9818d55aa1c68b43/test.txt",
             "deleteUrl":"http://walfrid.multiup.org/upload/index.php?file=test.txt",
             "deleteType":"DELETE"
          }
       ]
    }
    */

    QJsonDocument loadDoc(QJsonDocument::fromJson(temp));

    if (!loadDoc.isNull()) { // Test de validité

        QJsonObject connexionObj = loadDoc.object();

        // Récupération de l'objet contenant 1 liste de fichiers
        // (1 seul fichier car le logiciel n'upload que 1 fichier après l'autre)
        QJsonObject file = connexionObj["files"].toArray().at(0).toObject();

        // Récupération de l'url dans l'objet file récupéré
        QString url = file["url"].toString();

        qDebug() << "Curl :: Lien :" << url;
        bricolo.lien = url;
    }
    return nReal;
}

void UpCurl::arretCurl()
{
    // Cette manip permet à la fonction progress_func() de renvoyer -1,
    // ce qui d'après la doc entraine un arret forcé de l'up.
    // => "will cause libcurl to abort the transfer and return CURLE_ABORTED_BY_CALLBACK."
    qDebug() << "Curl :: Arret demandé";

    bricolo.upState = -1;
}

void UpCurl::emissionProgressionSlot()
{
    qDebug() << "Curl :: Emission progression";
    // WARNING: Signal jamais envoyé..
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
