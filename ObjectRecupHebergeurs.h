#ifndef OBJECTRECUPHEBERGEURS_H
#define OBJECTRECUPHEBERGEURS_H

#include <QtGui>
#include <QtNetwork>
#include <QJsonDocument>

//#include "parser.h" //qjson parser
#include "ObjectRecupHebergeursIcones.h"

#include "Config.h"

class RecupHebergeurs : public QObject
{
    Q_OBJECT

public:
    // Statut de la connexion BAD,OK,ERROR
    enum EtatConnexion
    {
        Bad, Ok, Error
    };

    RecupHebergeurs(QObject* parent = NULL);
    RecupHebergeurs(const QString &login, const QString &password, QObject* parent = NULL);
    virtual ~RecupHebergeurs();

public slots:
    void demarrage();

private slots:
    void finRecupHebergeurs();

signals:
    void emissionUrlIcone(QUrl url);
    void emissionRecupHebergeursEtat(int etatConnexion);
    void emissionRecupHebergeursHebergeurs(QString, QString, bool);
    void emissionRecupHebergeursIcones(QByteArray, int);
    void emissionMaxSelectionHebergeurs(int);
    void finished();

private:
    void finProcedure();
    QNetworkAccessManager   *m_networkAccessManager;
    EtatConnexion           m_statutConnexion;

    RecupHebergeursIcones   *m_threadRecupIcones;
    QThread                 *m_thread;

    QString                 m_login;
    QString                 m_password;
};

#endif // OBJECTRECUPHEBERGEURS_H
