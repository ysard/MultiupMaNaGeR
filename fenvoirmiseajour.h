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
    FenVoirMiseAJour(QWidget *parent = 0);
    ~FenVoirMiseAJour();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FenVoirMiseAJour *ui;

private slots:
    void on_pushButton_clicked();
};

#endif // FENVOIRMISEAJOUR_H
