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
#include "InfoNewVersion.h"

InfoNewVersion::InfoNewVersion(bool hidden, QWidget *parent) : QDialog(parent), ui(new Ui::InfoNewVersion)
{
    ui->setupUi(this);

    // qDebug() << "InfoNewVersion :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    // Determines whether or not the interface should be displayed
    m_hidden = hidden;

    // Open links using QDesktopServices::openUrl()
    // instead of emitting the linkActivated() signal
    ui->label->setOpenExternalLinks(true);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    // Check GitHub to now if there is a new update
    check_update();
}

InfoNewVersion::~InfoNewVersion()
{
    delete ui;
}

void InfoNewVersion::check_update()
{
    // Build query
    QNetworkRequest query;
    query.setRawHeader("User-Agent", USER_AGENT);
    query.setRawHeader("Accept-Language", "fr,fr-fr;q=0.8,en-us;q=0.5,en;q=0.3");
    query.setRawHeader("Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7");
    query.setRawHeader("Content-Type", "application/json");
    query.setUrl(QString(URL_API_GITHUB_RELEASES));

    // Init QNetworkAccessManager
    m_networkAccessManager = new QNetworkAccessManager;
    m_networkAccessManager->get(query);
    connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(endOfQuery(QNetworkReply*)));
}

void InfoNewVersion::endOfQuery(QNetworkReply *reply)
{

    qDebug() << "InfoNewVersion :: EndOfQuery";

    switch (reply->error()) {
        // Enumeration doc : http://qt.developpez.com/doc/4.7/qnetworkreply/#networkerror-enum
        case QNetworkReply::NoError :   //qDebug() << "InfoNewVersion :: No error";
                                        break;

        default :                       qDebug() << "InfoNewVersion :: Error : " << reply->errorString();
                                        reply->deleteLater();
                                        return;
                                        break;
    }

    // Parse the webpage
    QByteArray temp = reply->readAll();
    reply->deleteLater();
    parseWebpage(temp);
}


void InfoNewVersion::parseWebpage(QByteArray data)
{
    // Extract release informations from the given JSON doc and update the current QDialog.
    // The last release is always the first in the list (index 0)
    // The version is in the 'tag_name' key. The changelog message is in the 'body' key.
    // Ex: tag_name: "0.4.6-dev"
    // Assets are listed in 'assets' key. Each of them has a 'browser_download_url' key
    // with the full url link.

    if (!m_hidden)
        this->setHidden(false);

    QJsonDocument json_data = QJsonDocument::fromJson(data);
    //qDebug() << json_data;

    if (json_data.isNull()) {
        ui->label->setText(tr("Une erreur a été rencontrée. Veuillez réessayer plus tard."));
        return;
    }

    QJsonObject lastRelease = json_data[0].toObject();
    QString     lastVersion = lastRelease["tag_name"].toString();
    QJsonArray  assets      = lastRelease["assets"].toArray();

    qDebug() << "Last version published:" << lastVersion;

    QVersionNumber version = QVersionNumber::fromString(lastVersion, nullptr);
    if (version <= QVersionNumber::fromString(VERSION, nullptr)) {
        ui->label->setText(tr("Multiup MaNaGeR est à jour."));
        if (m_hidden)
            this->close();
        return;
    }

    // Use a string pattern to detect if the release contains
    // an asset compatible with the current platform
#ifdef LINUX
    QString searchedPattern = "linux";
#elif WINDOWS
    QString searchedPattern = "win";
#endif

    QJsonArray::const_iterator assets_it = assets.constBegin();
    while (assets_it != assets.constEnd()) {
        QJsonObject asset    = assets_it->toObject();
        QString     assetUrl = asset["browser_download_url"].toString();
        qDebug() << assetUrl;

        ++assets_it;

        if (!assetUrl.contains(searchedPattern, Qt::CaseInsensitive)) {
            continue;
        }

        ui->label->setText(
            tr("Une mise à jour a été trouvée !<br>"
               "Cliquez <a href=\"%1\">ici</a> pour télécharger la nouvelle version.").arg(
                    lastRelease["html_url"].toString()
                )
            );

        // In hidden mode, we must show the dialog
        if (m_hidden)
            this->setHidden(false);

        return;
    }
    // No update available for this platform
    ui->label->setText(tr("Multiup MaNaGeR est à jour."));
    if (m_hidden)
        this->close();
}
