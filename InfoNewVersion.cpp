#include "InfoNewVersion.h"

InfoNewVersion::InfoNewVersion(QWidget *parent) : QDialog(parent), ui(new Ui::InfoNewVersion)
{
    ui->setupUi(this);

    // qDebug() << "InfoNewVersion :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

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
    query.setRawHeader("Accept-Language","fr,fr-fr;q=0.8,en-us;q=0.5,en;q=0.3");
    query.setRawHeader("Accept-Charset","ISO-8859-1,utf-8;q=0.7,*;q=0.7");
    query.setRawHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
    query.setUrl(QString(URL_GITHUB_RELEASES));

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
    QString temp = reply->readAll();
    reply->deleteLater();
    parseWebpage(temp);
}

void InfoNewVersion::parseWebpage(QString data)
{
    // Extract versions from the webpage, and update the current QDialog.

    //qDebug() << data;
    QRegularExpression re(REGEX_VERSION_GITHUB);
    // find all the occurrences of a given regular expression
    QRegularExpressionMatchIterator i = re.globalMatch(data);
    QRegularExpressionMatch match;
    while (i.hasNext()) {
        match = i.next();
        // Full link, version:
        // qDebug() << match.captured(1) << match.captured(2);

        // Check if the new version is greater than current version
        // (in lexicographic order)
        if (match.captured(2) > VERSION) {
            ui->label->setText(
                        tr("Une mise à jour a été trouvée !<br>"
                           "Cliquez <a href=\"%1\">ici</a> pour télécharger la nouvelle version.").arg(URL_GITHUB_RELEASES));
            return;
        }
    }

    ui->label->setText(tr("Multiup MaNaGeR est à jour."));
}
