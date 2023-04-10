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
 * Copyright 2013 LuNiKoFf
 */
#ifndef FENVOIRMISEAJOUR_H
#define FENVOIRMISEAJOUR_H

#include <QtGui>
#include <QDialog>
#include "ui_fenvoirmiseajour.h"
#include "Config.h"

namespace Ui {
    class FenVoirMiseAJour;
}

class FenVoirMiseAJour : public QDialog {
    Q_OBJECT
public:
    FenVoirMiseAJour(QWidget *parent = nullptr);
    ~FenVoirMiseAJour();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FenVoirMiseAJour *ui;

private slots:
    void on_pushButton_clicked();
};

#endif // FENVOIRMISEAJOUR_H
