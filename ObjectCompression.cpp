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
#include "ObjectCompression.h"


Compression::Compression(const QString &cheminWinRar, const QStringList &arguments, QObject *parent): QObject(parent)
{
    qDebug() << "Compression :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    m_cheminWinRar  = cheminWinRar;
    m_arguments     = arguments;
    m_compression   = false;

    // PS: Rien d'autre ne peut être initialisé ici...
    // nous sommes encore dans un objet appartenant au thread parent
    // l'attribut sera lu dans le thread enfant
    // ex:
    // QObject: Cannot create children for a parent that is in a different thread.
    // (Parent is QNetworkAccessManager(0x89cc4e0), parent's thread is QThread(0x8930af8), current thread is QThread(0x89f3b38)
    // PS: ce message notifiant 3 threads, est probablement du au fait que l'initialisation de cet objet se fait par défaut détaché de tout:
    // QObject *parent = NULL...
}

Compression::~Compression()
{
    qDebug() << "Compression :: Destruction Compression en cours...";
    delete m_builder;
}

void Compression::demarrage()
{
    qDebug() << "Compression :: Début de la compression...";
    qDebug() << "Compression :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    m_builder = new QProcess;

    // rar a -hpMON_MOT_DE_PASSE -v760m -m0 -ep /chemin/$nom_sortie "$1"
    // Fusion de la sortie standard et de la sortie d'erreur standard vers la sortie standard du processus
    m_builder->setProcessChannelMode(QProcess::MergedChannels);

    // Mettre ça avant ou après le démarrage ?
    // This signal is emitted once every time new data is available for reading from the device's current read channel.
    connect(m_builder, SIGNAL(readyRead()), this, SLOT(lectureSortieStandard()));
    connect(m_builder, SIGNAL(finished(int)), this, SLOT(finProcedure(int)));

    // Démarrage
    m_builder->start(m_cheminWinRar, m_arguments);
}

void Compression::lectureSortieStandard()
{
    QByteArray ligne = m_builder->readAll();
    qDebug() << "Compression :: " << ligne;

    // Récupération de la valeur de l'avancement
    QRegExp rx(".*([0-9]{1,3}).*");
    if (rx.indexIn(ligne) != -1) {
        //qDebug() << "Compression :: " << rx.cap(1);

        emit this->emissionCompressionAvancement(rx.cap(1).toInt());
    }
    else {
        // Récupération de la réussite
        rx.setPattern(".*(Done|Termin|OK).*");
        if (rx.indexIn(ligne) != -1) {
            qDebug() << "Compression :: Fin trouvée : " << rx.cap(1);

            // On spécifie que la compression a réussi
            // Ce booléen servira à envoyer le signal emissionCompressionEtat(false) à la fin du processus
            m_compression = true;
        }
        // TODO: Marche mais pas tout le temps... le flux dans stdout est probablement trop rapide...
        // Récupération de l'étape
        rx.setPattern((".*(Creating archive).*"));
        if (rx.indexIn(ligne) != -1) {
            qDebug() << "Compression :: Etape trouvée : " << rx.cap(1);

            emit this->emissionCompressionEtape(rx.cap(1));
        }
        //else if erreur etc.... renvoit de 0
    }
}

void Compression::finProcedure(int codeRetour)
{
    // Ici on ne se sert pas du code de retour du processus.
    // EDIT: codeRetour=0 dans tous les cas, sauf si plantage...
    // Ce signal n'est envoyé que si le processus est terminé donc on peut amorcer la destruction de la classe après

    // TODO:
    // Le pb sous windows consistant à une absence de récupération de l'état de la compression est peut etre du à une fermeture prématurée du thread..
    // Dans ce cas, FinProcedure serait appelé alors que lectureSortieStandard() n'aurait pas terminé son travail... à voir...
    // si on est SUR que les Done ou Terminé peuvent etre récupérés, on peut mettre un booléen qui permettrait ici de faire par ex :
    //
    // while(!m_ready) msleep(50); // Attente tant que le résultat n'a pas été trouvé

    qDebug() << "Compression :: codeRetour:" << codeRetour << "; Reste:" << m_builder->readAll();

    // Envoi d'un signal contenant le statut du processus (true/false)
    emit this->emissionCompressionEtat(m_compression);
}
