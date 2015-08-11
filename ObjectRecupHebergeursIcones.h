#ifndef OBJECTRECUPHEBERGEURSICONES_H
#define OBJECTRECUPHEBERGEURSICONES_H

#include <QtGui>
#include <QtNetwork>

class RecupHebergeursIcones : public QObject
{
    Q_OBJECT

public:
    RecupHebergeursIcones(QObject* parent = NULL);
    virtual ~RecupHebergeursIcones();

public slots:
    void demarrage();
    void downloadUrl(const QUrl url, const int id);

private slots:
    void finRecupHebergeurIcone();

signals:
    void emissionRecupHebergeursIcones(QByteArray, int);
    void finished();

private:
    QNetworkAccessManager       *m_networkAccessManager;
    QUrl                        m_url;
    QHash<QNetworkReply*, int>  m_hash;
};

#endif // OBJECTRECUPHEBERGEURSICONES_H
