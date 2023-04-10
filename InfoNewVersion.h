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
#ifndef INFONEWVERSION_H
#define INFONEWVERSION_H

#include <QtGui>
#include <QPushButton>
#include <QDialog>
#include <QtNetwork>
#include <QJsonDocument>
#include "ui_InfoNewVersion.h"
#include "Config.h"

namespace Ui {
    class InfoNewVersion;
}

class InfoNewVersion : public QDialog
{
    Q_OBJECT

public:
    explicit InfoNewVersion(QWidget *parent = 0);
    ~InfoNewVersion();
    void check_update();

private slots:
    void endOfQuery(QNetworkReply *reply);
    void parseWebpage(QByteArray data);

private:
    Ui::InfoNewVersion      *ui;
    QNetworkAccessManager   *m_networkAccessManager;
};

#endif // INFONEWVERSION_H
