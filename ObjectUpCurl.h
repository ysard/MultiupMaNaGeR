#ifndef OBJECTUPCURL_H
#define OBJECTUPCURL_H

#include <QtGui>
#include <sstream>
#include "curl/curl.h"
#include <QJsonDocument>
#include "Config.h"

// Fix Unused variable warning, usage: UNUSED(x);
#define UNUSED(x) (void)(x)

struct Bricolage
{
    double  upTotal;
    double  upFait;
    int     upState;
    QString lien;
    QString lienDelete;
};


class UpCurl : public QObject
{
    Q_OBJECT

    public:
        UpCurl(const std::string &FileName, const std::string &Url, QStringList hebergeursListe, const std::string &loginId, QObject *parent = NULL);
        UpCurl(const std::string &FileName, const std::string &Url, QStringList hebergeursListe, QObject *parent = NULL);
        virtual ~UpCurl();
        static int progress_func(void *ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded);
        static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userdata);

    public slots:
        void demarrage();
        void arretCurl();
        void emissionProgressionSlot();
        void pause();
        void unpause();

    signals:
        void emissionLienEtStats(QString, double averageSpeed, double dureeTransfert);
        void emissionProgression(double TotalToUpload, double NowUploaded);
        void emissionEtatUpCurl(int etatUpCurl);
        void finished();

    private:
        void finProcedure(CURLcode hResult);

        CURL                    *m_hCurl;
        struct curl_httppost    *m_post;
        struct curl_httppost    *m_last;

        QStringList             m_hebergeursListe;
        std::string             m_curlFileName;
        std::string             m_curlUrl;
        std::string             m_loginId;
        bool                    m_connecte;
        //QTimer                  *m_timer;
        double                  m_averageSpeed;
        double                  m_dureeTransfert;
};

#endif // OBJECTUPCURL_H
