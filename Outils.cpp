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

    //Chaine de 62 caractères prédéfinis
    QString chaine = "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    QString resultat;

    for (int i = 0; i < 20; i++) {
        resultat.append(chaine.at(QRandomGenerator::global()->bounded(62)));
    }
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

QHash<QString, QByteArray> loadCachedIcons(QString path)
{
    // Get files from the given path
    // Return a QHash with filenames as keys and QByteArrays as values

    QHash<QString, QByteArray> table;
    QDir folder;
    folder.setPath(path);
    if (!folder.mkpath(path)) {
        qWarning() << "Cache dir not created!";
        return table;
    }
    QFileInfoList fileInfoList = folder.entryInfoList(QDir::AllEntries|QDir::NoDotAndDotDot);
    QList<QFileInfo>::const_iterator i = fileInfoList.begin();
    while (i != fileInfoList.constEnd()) {

        // all characters in the file up to (but not including) the last '.' character.
        qDebug() << i->completeBaseName();

        QFile file(path + i->fileName());
        if (!file.open(QIODevice::ReadOnly))
            continue;

        QByteArray fileContent;
        while (!file.atEnd()) {
            fileContent += file.readLine();
        }
        table[i->completeBaseName()] = fileContent;

        ++i;
    }
    return table;
}

bool saveIconToCache(QString hostName, QByteArray iconData)
{
    // Save the given data in the cache
    #ifdef WINDOWS
        QFile file(QApplication::applicationDirPath() + "\\cache\\" + hostName + ".png");
    #endif
    #ifdef LINUX
        QFile file(QDir::homePath() + "/" APP_DIR + "/cache/" + hostName + ".png");
    #endif
    if (!file.open(QIODevice::WriteOnly))
        return false;
    file.write(iconData);
    file.close();
    return true;
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
        out << "\nUp by Multiup MaNaGeR on http://multiup.io\n";

    }

    file.close();
}
