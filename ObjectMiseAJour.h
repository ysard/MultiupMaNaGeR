#ifndef OBJECTMISEAJOUR_H
#define OBJECTMISEAJOUR_H

#include <QtGui>
#include <QtNetwork>
#include <QApplication>

#include "Outils.h"
#include "Config.h"

class MiseAJour : public QObject
{
    Q_OBJECT

public:
    enum EtatConnexion
    {
        Bad, Ok, Error
    };

    MiseAJour(QObject *parent = NULL);
    virtual ~MiseAJour();

public slots:
    void demarrage();
    void receptionDemandeArret();

private slots:
    void finRecupVersion();
    void finMiseAJour();
    void progressionDownload(qint64 bytesReceived, qint64 bytesTotal);

signals:
    void finished();
    void updateProgressBar(int valeur);
    void emissionVersion(QString version);
    void emissionMiseAJourEtat(int etatConnexion);

private:
    void finProcedure();

    QNetworkAccessManager   *m_networkAccessManager;
    EtatConnexion           m_statutConnexion;
    int                     m_etape;
    QStringList             m_donneesMaj;
};

#endif // OBJECTMISEAJOUR_H
