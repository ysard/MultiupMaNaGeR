#ifndef OBJECTCOMPRESSION_H
#define OBJECTCOMPRESSION_H

#include <QtGui>
#include <QtNetwork>

class Compression : public QObject
{
    Q_OBJECT

public:
    Compression(const QString &cheminWinRar, const QStringList &arguments, QObject* parent = NULL);
    virtual ~Compression();

public slots:
    void demarrage();

protected:

private slots:
    void lectureSortieStandard();
    void finProcedure(int codeRetour);

signals:
    void emissionCompressionAvancement(int avancementCompression);
    void emissionCompressionEtat(bool etatCompression); // Ce signal n'est envoyé que si le processus est terminé donc on peut amorcer la destruction de la classe après
    void emissionCompressionEtape(QString etapeCompression);

private:
    QString                 m_cheminWinRar;
    QStringList             m_arguments;
    QProcess                *m_builder;
    bool                    m_compression;
};

#endif // OBJECTCOMPRESSION_H
