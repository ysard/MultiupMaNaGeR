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
 * Copyright 2012-2018 Lex
 * Copyright 2013 LuNiKoFf
 */
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
