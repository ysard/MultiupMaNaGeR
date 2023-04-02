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
    void downloadUrl(const QUrl url, QString id);

private slots:
    void finRecupHebergeurIcone();

signals:
    void emissionRecupHebergeursIcones(QByteArray, QString);
    void finished();

private:
    QNetworkAccessManager       *m_networkAccessManager;
    QUrl                        m_url;
    QHash<QNetworkReply*, QString>  m_hash;
};

#endif // OBJECTRECUPHEBERGEURSICONES_H
