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
#include "FenLiens.h"

FenLiens::FenLiens(const QString &code, QWidget *parent = 0) : QDialog(parent)
{
    m_code      = code;
    m_switchLog = false;


    this->setWindowTitle(tr("Visualisation des liens"));
    this->resize(750, 450);

        QVBoxLayout *m_vText = new QVBoxLayout;

            m_text = new QTextEdit();
            m_text->setText(m_code);
            m_text->setFont(QFont("Courier", 9));
            m_text->setReadOnly(true);

        m_vText->addWidget(m_text);

            QHBoxLayout *m_hText = new QHBoxLayout;

                m_boutonSwitchLog = new QPushButton(tr("Afficher le log"));
                m_boutonSwitchLog->setIcon(QIcon(":/interface/img/interface/retry.png"));

                m_rec = new QPushButton(tr("Sauvegarder"));
                m_rec->setIcon(QIcon(":/interface/img/interface/status_editing.png"));

                m_close = new QPushButton(tr("Fermer"));
                m_close->setIcon(QIcon(":/interface/img/interface/remove.png"));

            m_hText->addWidget(m_boutonSwitchLog);
            m_hText->addWidget(m_rec);
            m_hText->addWidget(m_close);

        m_vText->addLayout(m_hText);

    this->setLayout(m_vText);

    QObject::connect(m_boutonSwitchLog, SIGNAL(clicked()), this, SLOT(chargerLog()));
    QObject::connect(m_rec, SIGNAL(clicked()), this, SLOT(enregistrer()));
    QObject::connect(m_close, SIGNAL(clicked()), this, SLOT(accept()));
}

FenLiens::~FenLiens()
{
    qDebug() << "FenLiens :: Destruction";
}

void FenLiens::enregistrer()
{
    //sélectionner un dossier
    //appel fonction enregistrement
    QString dossier = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier..."), "", QFileDialog::ShowDirsOnly);

    if (dossier != "") { //doit etre testé avant, car la fonction d'enregistrement enregistre par défaut dans le dossier de l'application (meme si dossier="")
        if (enregistrement(dossier, m_code)) {
            //Si tout s'est bien passé on emet le signal accept, et on quitte la condition.
            this->accept();
            return;
        }
        QMessageBox::information(NULL, tr("Attention !"), tr("Fichier non enregistré."));
    }
}

void FenLiens::chargerLog()
{
    // Affiche les liens provenant du logiciel si m_switchLog = true
    // si m_switchLog = false on charge le contenu du log.
    if (m_switchLog == true)
    {
        m_text->setText(m_code);

        m_boutonSwitchLog->setText(tr("Afficher le log"));
        m_switchLog = false;
        return;
    }

    // Chargement du fichier de log

    #ifdef WINDOWS
        qDebug() << "Chemin fichier liens :" << QApplication::applicationDirPath() << "\\liens_multiup.txt";
        QFile file(QApplication::applicationDirPath() + "\\liens_multiup.txt");
    #endif
    #ifdef LINUX
        qDebug() << "Chemin fichier liens :" << QApplication::applicationDirPath() << "/liens_multiup.txt";
        QFile file(QDir::homePath() + "/" + APP_DIR + "/liens_multiup.txt");
    #endif

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    m_text->setText(file.readAll());
    file.close();

    m_boutonSwitchLog->setText(tr("Afficher les liens"));
    m_switchLog = true;
}

