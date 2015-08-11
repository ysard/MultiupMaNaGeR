#include "mainwindow.h"
#include "FenDemandeMiseAJour.h"
#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QString locale = QLocale::system().name().section('_', 0, 0);

    QTranslator translator;
    if( locale != "fr" )
        translator.load(QString("multiupManager_en"));
    a.installTranslator(&translator);

//    FenDemandeMiseAJour dialog;
//    dialog.hide();

    MainWindow w;
    w.show();
    
    return a.exec();
}
