#ifndef OBJECTCONNEXION_H
#define OBJECTCONNEXION_H

#include <QtGui>
#include <QtNetwork>
#include <QJsonDocument>

//#include "parser.h" //qjson parser

#include "Config.h"

class Connexion : public QObject
{
    Q_OBJECT

public:
    // Statut de la connexion BAD,OK,ERROR
    enum EtatConnexion
    {
        Bad, Ok, Error
    };

    Connexion(const QString &login, const QString &password, QObject* parent = NULL);
    virtual ~Connexion();


public slots:
    void demarrage();

private slots:
    void finLogin();
    void finServeurWwpw();
    //void erreur(QNetworkReply::NetworkError);

signals:
    void emissionLoginEtat(int etatConnexion);
    void emissionLoginId(QString id);
    //void emissionLoginCookies(QList<QNetworkCookie> cookieListe);
    void emissionDroitsServeurWwpw(bool droitsServeurWwpw);
    void finished();

private:
    void finProcedure();
    QNetworkAccessManager   *m_networkAccessManager;
    QString                 m_login;
    QString                 m_password;
    //QList<QNetworkCookie>   m_cookieListe;

    EtatConnexion           m_statutConnexion;

    int                     m_etape; // Disons 0 = connexion, 1 = droits wwpw

};

#endif // OBJECTCONNEXION_H
