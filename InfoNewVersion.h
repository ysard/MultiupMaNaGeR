#ifndef INFONEWVERSION_H
#define INFONEWVERSION_H

#include <QtGui>
#include <QPushButton>
#include <QDialog>
#include <QtNetwork>
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
    void parseWebpage(QString data);

private:
    Ui::InfoNewVersion      *ui;
    QNetworkAccessManager   *m_networkAccessManager;
};

#endif // INFONEWVERSION_H
