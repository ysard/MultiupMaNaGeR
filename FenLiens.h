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
#ifndef FENLIENS_H
#define FENLIENS_H

#include <QtGui>
#include <QTextEdit>
#include <QPushButton>
#include <QDialog>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>

#include "Outils.h"

#include "Config.h"

class FenLiens : public QDialog
{
    Q_OBJECT

    public:
        FenLiens(const QString &code, QWidget *parent);
        virtual ~FenLiens();

    public slots:
        void chargerLog();
        void enregistrer();

    private:
        QTextEdit   *m_text;

        QPushButton *m_rec;
        QPushButton *m_close;
        QPushButton *m_boutonSwitchLog;

        QString     m_code;

        bool        m_switchLog;
};

#endif // FENLIENS_H
