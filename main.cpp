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
#include "mainwindow.h"
#include <QtGui>
#include <QSslSocket>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QString locale = QLocale::system().name().section('_', 0, 0);

    QTranslator translator;
    bool translation_status = false;
    if (locale == "es")
        translation_status = translator.load(":/i18n/multiupManager_es");
    if (locale != "fr" && !translation_status)
        translation_status = translator.load(":/i18n/multiupManager_en");
    qDebug() << "Translations loading status:" << translation_status << locale;
    a.installTranslator(&translator);

    //qDebug()<<"SSL version use for build: "<< QSslSocket::sslLibraryBuildVersionString();
    //qDebug()<<"SSL version use for run-time: "<< QSslSocket::sslLibraryVersionNumber();


    MainWindow w;
    w.show();

    return a.exec();
}
