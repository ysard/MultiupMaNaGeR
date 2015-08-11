#ifndef FENDEMANDEMISEAJOUR_H
#define FENDEMANDEMISEAJOUR_H

#include <QtGui>
#include <QPushButton>
#include <QDialog>

#include "ObjectMiseAJour.h"
#include "ui_FenDemandeMiseAJour.h"
#include "Config.h"

namespace Ui {
class FenDemandeMiseAJour;
}

class FenDemandeMiseAJour : public QDialog
{
    Q_OBJECT
    
public:
    explicit FenDemandeMiseAJour(QWidget *parent = 0);
    ~FenDemandeMiseAJour();
    void processusRemplacement();

public slots:
    void updateProgressBar(int valeur);
    void receptionMiseAJourVersion(QString version);
    void finThreadMiseAJour();
    void receptionMiseAJourEtat(int etatConnexion);
    
private slots:
    void boutonOk_clicked();
    void boutonAnnuler_clicked();

signals:
    void emissionDemandeMAJ();
    void emissionDemandeArret();

private:
    Ui::FenDemandeMiseAJour *ui;
    QPushButton             *m_boutonOk;
    QPushButton             *m_boutonAnnuler;
    bool                    m_etape;
    bool                    m_reussite;
    MiseAJour               *m_miseAJour;

protected:
    void closeEvent(QCloseEvent* event);
    void reject();
};

#endif // FENDEMANDEMISEAJOUR_H
