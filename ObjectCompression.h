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
 * Copyright 2012-2023 Lex
 */
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
