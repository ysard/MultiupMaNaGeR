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
