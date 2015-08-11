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
    enum EtatConnexion
    {
        Bad, Ok, Error
    };

    RecupHebergeurs(QObject* parent = NULL);
    virtual ~RecupHebergeurs();

public slots:
    void demarrage();

private slots:
    void finRecupHebergeurs();

signals:
    void emissionUrlIcone(QUrl url, int id);
    void emissionRecupHebergeursEtat(int etatConnexion);
    void emissionRecupHebergeursHebergeurs(QString, QString);
    void emissionRecupHebergeursIcones(QByteArray, int);
    void finished();

private:
    void finProcedure();
    QNetworkAccessManager   *m_networkAccessManager;
    EtatConnexion           m_statutConnexion;

    RecupHebergeursIcones   *m_test;
    QThread                 *m_thread;
};

#endif // OBJECTRECUPHEBERGEURS_H
