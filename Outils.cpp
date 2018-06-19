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
 */
#include "Outils.h"

double conversion(qint64 valeur)
{
    return static_cast<double>(valeur);
}

qint64 conversion(double valeur)
{
    return static_cast<qint64>(valeur);
}

double conversion(int valeur)
{
    return static_cast<double>(valeur);
}


bool enregistrement(const QString dossier, const QString code)
{
    #ifdef WINDOWS
        qDebug() << "Chemin fichier liens :" << dossier << "\\liens_multiup.txt";
        QFile file(dossier + "\\liens_multiup.txt");
    #endif
    #ifdef LINUX
        qDebug() << "Chemin fichier liens :" << dossier << "/liens_multiup.txt";
        QFile file(dossier + "/liens_multiup.txt");
    #endif

    if (!file.open(QIODevice::Append | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << QTime::currentTime().toString("hh:mm:ss") + '\n';
    out << code + '\n';
    //ou:
    //file.write(code.toStdString().c_str());
    file.close();

    return true;
}

QString randomName()
{
    // Renvoit un QString comportant une chaine de 12 caractères aléatoires, choisis parmi 62

    //qDebug() << qrand();
    //Chaine de 62 caractères prédéfinis
    QString chaine = "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    QString resultat;

    // Initialisation de qsrand() avec une graine,
    //qrand() récupère a priori cette initialisation pour générer des nombres..
    // De toute façon sans ça, les nombres générés ne sont pas aléatoire d'un lancement de l'app à l'autre...
    // => http://www.siteduzero.com/forum-83-415615-p1-qt-generer-un-nombre-au-hasard.html
    qsrand(QDateTime::currentDateTime().toTime_t());

    for (int i = 0; i< 12; i++)
    {
        //qDebug() << qrand() % ((61 + 1) - 0) + 0;
        resultat.append(chaine.at(qrand() % ((61 + 1) - 0) + 0));

    }
    //qDebug() << resultat;

    return resultat;
}

// Fonction chourrée là: http://www.qtfr.org/viewtopic.php?id=2775
qint64 sizeOfdir(QString path)
{
    // Attention, le flag QDir::NoDotAndDotDot permet de ne pas lister les fichiers ou dossiers commençant par . ou ..
    // C'est nécessaire pour éviter du bordel MAIS ces fichiers prennent de la place et faussent la taille obtenue...
    // pas vraiment d'autre méthode...
    qint64 size = 0;
    QDir folder;
    folder.setPath(path);
    QFileInfoList fileInfoList = folder.entryInfoList(QDir::AllEntries|QDir::NoDotAndDotDot);
    for(int i = 0; i < fileInfoList.size(); ++i)
    {
        if(fileInfoList.at(i).isDir())
        {
            qDebug() << "Répertoire : " << fileInfoList.at(i).path() + "/" + fileInfoList.at(i).fileName() << "\n";
            size+=sizeOfdir(fileInfoList.at(i).path() + "/" + fileInfoList.at(i).fileName());
        }
        else
        {
            qDebug() << "Fichier : " << fileInfoList.at(i).fileName() << "\n";
              size+=fileInfoList.at(i).size();
        }
    }
    return size;
}

void searchPrezFile(QString prezFile, QString link)
{
    // Recherche un fichier de présentation du même nom que le fichier uploadé
    // Recherche dans ce fichier l'occurence <multiup_manager> et la remplace par le lien

    qDebug() << "Tentative d'ouverture du fichier" << prezFile << "et d'ajout du lien";

    QFile file(prezFile);

    if (!file.exists())
        return;

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "Fichier non ouvrable !";
        return;
    }

    QString data = file.readAll();
    // On retourne au début du fichier.
    file.seek(0);

    // Recherche de la chaine
    QRegExp rx("(multiup_manager)");
    if (data.contains(rx))
    {
        qDebug() << "Chaine <multiup_manager> trouvée !";

        // Remplacement de la chaine
        data.replace(rx, link);

        QTextStream out(&file);
        out << data;
        out << "\nUp by Multiup MaNaGeR on http://multiup.org\n";

    }

    file.close();
}
