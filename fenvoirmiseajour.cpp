#include "fenvoirmiseajour.h"

FenVoirMiseAJour::FenVoirMiseAJour(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FenVoirMiseAJour)
{
    ui->setupUi(this);

    ui->label->setText(tr("Version ") + QString(VERSION) + tr(", compilée le : ") + QString(__DATE__));
}

FenVoirMiseAJour::~FenVoirMiseAJour()
{
    delete ui;
}

void FenVoirMiseAJour::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void FenVoirMiseAJour::on_pushButton_clicked()
{
    this->close();
}
