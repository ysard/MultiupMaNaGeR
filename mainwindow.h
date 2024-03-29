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
 * Copyright 2013 LuNiKoFf
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QtGui>
#include <QtNetwork>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QProgressBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenu>
#include <QAction>

#include <string>
#include "curl/curl.h"

#include "Outils.h"
#include "fenvoirmiseajour.h"
#include "FenLiens.h"
#include "InfoNewVersion.h"
#include "Config.h"

#ifdef WINDOWS
    #include <QWinTaskbarProgress>
    #include <QWinTaskbarButton>
#endif

#include "ObjectConnexion.h"
#include "ObjectCompression.h"
#include "ObjectSelectionServeur.h"
#include "ObjectUpCurl.h"
#include "ObjectRecupHebergeurs.h"


#include "ui_mainwindow.h"


// Fix Unused variable warning, usage: UNUSED(x);
#define UNUSED(x) (void)(x)


class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    // Statuts possibles des fichiers
    enum EtatFichier
    {
        STOPED, REMOVED, FAILED_e, FINISHED, PREPROCESSING, READY, UPLOADING
    };

    // Statuts possibles pour la manipulation des données du tableau
    enum EtatTableau
    {
        AJOUT, RETRAIT, RAZ
    };

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void creationTableau(EtatTableau etat, const QStringList liste); // Il aurait fallu mettre un paramètre facultatif sur le QStringList..
    void remplissageTableau(int row, int column, const QString text);
    bool testFichier(QFile &file) const;
    void suppressionTableau(const EtatFichier state);
    void readSettings();

    void readCommandLineArguments();

public slots:
    void trayClicked(QSystemTrayIcon::ActivationReason reason);

    void on_tableWidget_customContextMenuRequested(QPoint point);
    void actionAnnuler_triggered();
    void actionArreter_triggered();
    void actionEnAttente_triggered();
    void actionCopier_triggered();

    void actionSysTrayRetablir_triggered();
    void actionSysTrayArreter_triggered();

    void on_checkBoxExtinction_clicked();
    void on_checkBoxReprise_clicked();
    void on_checkBoxEffacement_clicked();
    void on_checkBoxPrez_clicked();
    void on_checkBoxRegVitesse_clicked();
    void on_horizontalSliderRegVitesse_valueChanged(int value);

    void on_boutonSuppressionTout_clicked();
    void on_boutonSuppressionAnnules_clicked();
    void on_boutonSuppressionTermines_clicked();

    void on_boutonOuvrir_clicked();

    void on_boutonLiens_clicked();
    void on_boutonSavParametres_clicked();

    void on_boutonConnexion_clicked();
    void receptionLoginEtat(int etatConnexion);
    void receptionLoginId(QString id);

    void receptionSelectionServeurEtat(int etatSelectionServeur);

    void on_descriptionText_linkActivated(const QString &link);
    void checkUpdateWindow(bool hidden=false);

    void on_boutonCompressSrc_clicked();
    void on_boutonCompressDest_clicked();
    void on_boutonCompress_clicked();
    void ajoutFichierCompress(QString fichier);
    void receptionCompressionAvancement(int avancementCompression);
    void receptionCompressionEtat(bool compressionSucess);
    void receptionCompressionEtape(QString etapeCompression);

    void rechercheHebergeurs();
    void checkboxHebergeursClicked(QString hebergeur);
    bool regulationMaxSelectedHosts();

    void on_boutonUpload_clicked();

    void editionLogin();


    void receptionAdresseIpServeur(QString adresseIp);

    void receptionUpCurlLienEtStats(QString lien, double averageSpeed, double dureeTransfert);
    void receptionUpCurlEtat(int etatUpCurl);
    void receptionRecupHebergeursEtat(int etatRecupHebergeurs);
    void receptionRecupHebergeursHebergeurs(QString hebergeur, QString hebergeurTexte, bool etat_selection, QString id);
    void receptionRecupHebergeursIcones(QByteArray icone, QString id);
    void receptionMaxSelectionHebergeurs(int);
    void receptionUpCurlProgression(double TotalToUpload, double NowUploaded);
    void upCurlUnpause();

    void finThreadConnexion();
    void finThreadCompression();
    void finThreadSelectionServeur();
    void finThreadUpCurl();
    void finThreadRecupHebergeurs();


    void emissionProgressionSlot();


signals:
    void emissionDemandeArretCurl(); // Marche pas

private:
    void calculTailleTotale();
    void interfaceUpdate();
    void rechercheRetraitListe(const QString &hebergeur);

    void dataForge();
    void preparationUpload();
    void uploadCurl();
    void selectionServeur();


    QLineEdit               *m_login;
    QLineEdit               *m_password;
    QString                 m_loginId;
    QCheckBox               *m_checkBox_Wwpw;
    QSignalMapper           *m_signalMapper;            // mappage des signaux des checkbox
    QHash<QByteArray, QString>  m_listIconesHebergeurs;     // Fait correspondre les icones avec les checkbox reçues
    QHash<QCheckBox*, QString>  m_listCheckBoxHebergeurs;   // Fait correspondre les icones avec les checkbox reçues
    QList<QCheckBox*>       m_sortedListCheckBox;       // permet l'affichage des checkbox dans un ordre harmonieux (déjà cochées en premier)
    int                     m_maxHosts;

    QLabel                  *m_labelExtinction;
    QLabel                  *m_labelReprise;
    QLabel                  *m_labelEffacement;
    QLabel                  *m_labelPrez;
    QLabel                  *m_labelAuteurs;

    #ifdef WINDOWS
        QWinTaskbarButton   *m_taskbarButton;
        QWinTaskbarProgress *m_taskbarProgress;
    #endif

    QSystemTrayIcon         *m_sticon;
    QMenu                   *m_stmenu;
    QAction                 *actionAnnuler;
    QAction                 *actionArreter;
    QAction                 *actionEnAttente;
    QAction                 *actionCopier;
    QAction                 *actionSysTrayArreter;
    QAction                 *actionSysTrayRetablir;
    QAction                 *actionSysTrayQuitter;

    bool                    m_extinction;
    bool                    m_reprise;
    bool                    m_effacement;
    bool                    m_prez;
    bool                    m_upEnCours;
    bool                    m_regVitesse;

    bool                    m_configIdentifiants;
    bool                    m_configParametres;

    int                     m_row;
    qint64                  m_sizeTot;
    qint64                  m_sizeFaite;
    QElapsedTimer           m_chronometre;
    QTimer                  *m_timer;
    QProgressBar            *m_item;

    QString                 m_adresseIpServeur;
    bool                    m_connecte;
    QStringList             m_hebergeursListe;

    QStringList             m_cheminsFichiers;
    QList<EtatFichier>      m_etatsFichiers;
    QList<qint64>           m_tailleFichiers;
    QStringList             m_liensFichiers;
    QString                 m_liens;

    QString                 m_cheminWinRar;
    QString                 m_cheminSettingsFile;

    Connexion               *m_connexion;
    Compression             *m_compression;
    SelectionServeur        *m_selectionServeur;
    UpCurl                  *m_upCurl;
    RecupHebergeurs         *m_recupHebergeurs;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent* event);
    void showEvent(QShowEvent *event);
};

#endif // MAINWINDOW_H
