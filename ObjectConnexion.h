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
    //void erreur(QNetworkReply::NetworkError);

signals:
    void emissionLoginEtat(int etatConnexion);
    void emissionLoginId(QString id);
    void finished();

private:
    void finProcedure();
    QNetworkAccessManager   *m_networkAccessManager;
    QString                 m_login;
    QString                 m_password;

    EtatConnexion           m_statutConnexion;

};

#endif // OBJECTCONNEXION_H
