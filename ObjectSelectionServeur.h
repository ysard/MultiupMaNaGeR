#ifndef OBJECTSELECTIONSERVEUR_H
#define OBJECTSELECTIONSERVEUR_H

#include <QtGui>
#include <QtNetwork>
#include <QJsonDocument>
#include "Config.h"

class SelectionServeur : public QObject
{
    Q_OBJECT

public:
    enum EtatConnexion
    {
        Bad, Ok, Error
    };

    SelectionServeur(QObject* parent = NULL);
    virtual ~SelectionServeur();

public slots:
    void demarrage();

private slots:
    void finSelectionServeur();

signals:
    void emissionAdresseIp(QString);
    void emissionEtatSelectionServeur(int); // que erreur ?
    void finished();

private:
    void finProcedure();

    QNetworkAccessManager   *m_networkAccessManager;
    QString                 m_adresseIp;
    EtatConnexion           m_statutConnexion;
    bool                    m_connecte;
};

#endif // OBJECTSELECTIONSERVEUR_H
