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
#include "mainwindow.h"

#define STOPED_TEXT() QObject::tr("Arrêté")
#define REMOVED_TEXT() QObject::tr("Annulé")
#define FAILED_TEXT() QObject::tr("Erreur")
#define FINISHED_TEXT() QObject::tr("Terminé")
#define PREPROCESSING_TEXT() QObject::tr("Choix serveur")
#define READY_TEXT() QObject::tr("En attente")
#define UPLOADING_TEXT() QObject::tr("En cours d'upload")

#define STOPED_IMG ":/interface/img/interface/stop.png"
#define REMOVED_IMG ":/interface/img/interface/remove.png"
#define FAILED_IMG ":/interface/img/interface/status_failed.png"
#define FINISHED_IMG ":/interface/img/interface/status_finished.png"
#define PREPROCESSING_IMG ":/interface/img/interface/status_preprocessing.png"
#define READY_IMG ":/interface/img/interface/status_ready.png"
#define UPLOADING_IMG ":/interface/img/interface/status_uploading.png"
#define RETRY_IMG ":/interface/img/interface/retry.png"
#define COPY_IMG ":/interface/img/interface/menu_paste.png"
#define VIEW_IMG ":/interface/img/interface/view.png"

// Récupération de l'instance de la structure globale initialisée dans ObjectUpCurl.cpp et déclarée dans ObjectUpCurl.h
extern struct Bricolage bricolo;

/*
Les états des fichiers dans le tableau sont définis par une énumération: etatFichier

typedef enum etatFichier etatFichier;
enum etatFichier
{
    STOPED, REMOVED, FAILED_e, FINISHED, PREPROCESSING, READY, UPLOADING
};
Peut être faudrait t'il définir le fichier en cours d'utilisation voir tous les fichiers donc toutes les lignes du tablewidget par 1 structure voire un tableau de structure ????
*/


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // On crée la fenêtre
    setupUi(this);

    this->setWindowIcon(QIcon(":/img/favicon.png"));
    this->setWindowTitle(QString("MultiUp MaNaGeR v" + QString(VERSION) + tr(" Fr")));

    // Rajout du champ de login
    m_login = new QLineEdit;
    m_password = new QLineEdit;
    m_password->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    formLayoutIdentifiants->addRow(tr("Identifiant :"), m_login);
    formLayoutIdentifiants->addRow(tr("Password :"), m_password);


    // Ajout checkbox multiples
    // Initialisation du QSignalMapper
    m_signalMapper = new QSignalMapper(this);
    connect(m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(checkboxHebergeursClicked(QString)));

//    rechercheHebergeurs();


    // Ajout des labels normaux (pas permanents) à  la statusBar
    m_labelExtinction   = new QLabel;
    m_labelReprise      = new QLabel;
    m_labelEffacement   = new QLabel;
    m_labelPrez         = new QLabel;
    m_labelAuteurs      = new QLabel;

    m_labelExtinction->setText(QString(tr("Extinction : Désactivée")));
    m_labelReprise->setText(QString(tr("Reprise : Désactivée")));
    m_labelEffacement->setText(QString(tr("Effacement : Désactivé")));
    m_labelPrez->setText(QString(tr("Prez : Désactivée")));
    m_labelAuteurs->setText(QString("by Lex & LuNiKoFf"));

    barreEtat->addWidget(m_labelExtinction);
    barreEtat->addWidget(m_labelReprise);
    barreEtat->addWidget(m_labelEffacement);
    barreEtat->addWidget(m_labelPrez);
    barreEtat->addPermanentWidget(m_labelAuteurs);

    // Ajout d'un validator au lineEditCompressNb, pour n'entrer que des nombres entiers de 1 à  100 (pose problème ?)
    QValidator *validatorEntier = new QIntValidator(1, 100, lineEditCompressNb);
    lineEditCompressNb->setValidator(validatorEntier);

    // Réglages du tableau
    QStringList arguments;
    arguments << tr("Nom") << tr("Etat") << tr("Avancement") << tr("Taille (Mio)") << tr("Vitesse") << tr("Lien");
    tableWidget->setColumnCount(6);
    tableWidget->setHorizontalHeaderLabels(arguments);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // Adapte la taille du tableau à la taille de la fenÃÅ¡tre
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // Le tableau n'est pas éditable
    tableWidget->setContextMenuPolicy(Qt::CustomContextMenu); // Permet l'appel d'un menu au clic droit

    // Réglages drag & drop
    tableWidget->setAcceptDrops(true); //Active le dragdrop sur le tablewidget
    groupBoxCompression->setAcceptDrops(true);
    this->setAcceptDrops(true); //Active le dragdrop sur la fenetre principale
    //PS: si on active pour les 2 => ça génère 1'évènement à  chaque fois : 1 pr la fenetre + 1 pr le widget
    //si on active que pour le widget et pas la fenetre => ne marche pas

    // Réglages des boutons
    boutonOuvrir->setShortcut(QKeySequence("Ctrl+O"));
    boutonUpload->setShortcut(QKeySequence("Ctrl+U"));
    boutonLiens->setShortcut(QKeySequence("Ctrl+L"));

    // Création des QAction des menus contextuels
    actionAnnuler = new QAction(tr("Annuler"), this);
    actionAnnuler->setIcon(QIcon(REMOVED_IMG));

    actionArreter = new QAction(tr("Arreter"), this);
    actionArreter->setIcon(QIcon(STOPED_IMG));

    actionEnAttente = new QAction(tr("En attente"), this);
    actionEnAttente->setIcon(QIcon(RETRY_IMG));

    actionCopier = new QAction(tr("Copier"), this);
    actionCopier->setIcon(QIcon(COPY_IMG));

    actionSysTrayArreter = new QAction(tr("Arreter l'Upload !"), this);
    actionSysTrayArreter->setDisabled(true);
    actionSysTrayArreter->setIcon(QIcon(STOPED_IMG));

    actionSysTrayRetablir = new QAction(tr("Cacher Multiup MaNaGeR"), this);
    actionSysTrayRetablir->setIcon(QIcon(VIEW_IMG));

    actionSysTrayQuitter = new QAction(tr("Quitter"), this);
    actionSysTrayQuitter->setIcon(QIcon(REMOVED_IMG));

    // SysTrayIcon & SysTrayMenu
    m_sticon = new QSystemTrayIcon(this);
    m_stmenu = new QMenu();

    m_stmenu->addAction(actionSysTrayArreter);
    m_stmenu->addSeparator();
    m_stmenu->addAction(actionSysTrayRetablir);
    m_stmenu->addSeparator();
    m_stmenu->addAction(actionSysTrayQuitter);
    m_stmenu->addSeparator();

    m_sticon->setIcon(QIcon(":/img/favicon.png"));
    m_sticon->setContextMenu(m_stmenu);
    m_sticon->setToolTip(tr("MultiUp MaNaGeR : au repos"));
    m_sticon->show();

    connect(m_sticon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
    connect(actionSysTrayRetablir, SIGNAL(triggered()), this, SLOT(actionSysTrayRetablir_triggered()));
    connect(actionSysTrayArreter, SIGNAL(triggered()), this, SLOT(actionSysTrayArreter_triggered()));
    connect(actionSysTrayQuitter, SIGNAL(triggered()), this, SLOT(close()));


    // Qt Win Extras
    #ifdef WINDOWS
        /* Ce comportement est donné en exemple dans http://doc.qt.io/qt-5/qwintaskbarbutton.html
         * La simple instanciation de QTaskbarProgress sans le QWinTaskbarButton est impossible.
         * On a beau afficher avec .show(), la barre ne s'affiche pas.
         *
         * Mais même en ajoutant le QWinTaskbarButton, cela ne marche pas:
         *
         * In fact, it seems like calling
         * button->setWindow(widget->windowHandle());
         * in QMainWindow constructor doesn't work and QWinTaskbarProgress
         * won't show at all even after calling setVisible(true) or show().
         *
         * If created in the QMainWindow constructor it has to be called once the Window is shown like in:
         * => réimplémentation de showEvent()
         *
         * En fait l'affichage doit se faire quand la fenêtre principale est déjà affichée.
         * => pas dans le constructeur donc.
         */

        m_taskbarButton = new QWinTaskbarButton(this);
        //m_taskbarButton->setWindow(windowHandle());
        m_taskbarProgress = m_taskbarButton->progress();
    #endif



    // Connect du menu contextuel sur le tableWidget
    //PS: crées ici et non dans l'event on_tableWidget_customContextMenuRequested() car sinon le truc tourne en boucle et "superpose" à  chaque fois les actions sur le menu...
    //Ce qui était à  l'origine du foirage au premier clic et d'une multitude d'appels pour les clics suivants
    connect(actionArreter, SIGNAL(triggered()), this, SLOT(actionArreter_triggered()));
    connect(actionAnnuler, SIGNAL(triggered()), this, SLOT(actionAnnuler_triggered()));
    connect(actionEnAttente, SIGNAL(triggered()), this, SLOT(actionEnAttente_triggered()));
    connect(actionCopier, SIGNAL(triggered()), this, SLOT(actionCopier_triggered()));

    // Initialisation de variables
    m_upEnCours     = false;    //Etat du programme
    m_row           = 0;        //Le programme travaille sur cette ligne à un moment t
    m_sizeTot       = 0;        //Taille totale des fichiers à up => sert pour la barre de progression
    m_sizeFaite     = 0;        //Taille totale des fichiers déjà envoyés => sert pour la barre de progression

    // Initialisation des pointeurs persos!
    //évite de mauvaises surprises... La même chose est faite dans tous les slots publics finThreadXXX()
    m_connexion     = NULL;
    m_compression   = NULL;
    m_selectionServeur = NULL;
    m_upCurl        = NULL;

    m_connecte      = false;        // Par def le statut est déconnecté (anonyme)
    m_extinction    = false;        // Par def l'extinction programmée de l'ordi est désactivée
    m_reprise       = false;        // Par def la reprise sur erreur est désactivée
    m_effacement    = false;        // Par def l'effacement des fichiers est désactivé
    m_prez          = false;        // Par def l'aide à la présentation des ups est désactivée
    m_regVitesse    = false;        // Par def le bridage de la vitesse est désactivé

    // Gestion du fichier de config
    // Par def on considère que le fichier de config n'a jamais été crée.
    // Les boutons sont à "Sauvegarder".
    m_configParametres = false;

    //---------- Progression => j'ai pas mal de pbs avec l'émission des signaux dans la nouvelle implémentation des threads..
    //surtout avec celui qui fait vivre l'up par curl :s
    // Appeler une structure statique contenant ces variables
    // les valeurs sont envoyées régulièrement par un Qtimer (1000 ms)
    // D'après la doc de curl, la fonction progress_func() est appelée toutes les secondes parfois moins.
    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(emissionProgressionSlot()));

    //voir si le signal textEdited ne serait pas mieux.. mais il se délenche à chaque lettre entrée..
    //editingFinished ne se délenche que si le widget perd le focus
    connect(m_login, SIGNAL(editingFinished()), this, SLOT(editionLogin()));
    connect(m_password, SIGNAL(editingFinished()), this, SLOT(editionLogin()));

    // Affichage loader/spinbox (gif animé)
    QMovie *movie = new QMovie(":/interface/img/interface/spinload.gif");
    label_8->setMovie(movie);
    movie->start();

    // Rappel des identifiants
    #ifdef WINDOWS
        m_cheminSettingsFile = QApplication::applicationDirPath() + "\\" + SETTINGS_FILE;
    #endif
    #ifdef LINUX
        m_cheminSettingsFile = QDir::homePath() + "/" + APP_DIR + "/" + SETTINGS_FILE;
    #endif
    readSettings();

    if (checkBoxCheckUpdate->isChecked())
        checkUpdateWindow(true);

    // Actualisation des champs et auto-connexion
    // Pas d'alerte si l'utilisateur n'a pas configuré config.ini
    if (m_login->text().isEmpty())
        rechercheHebergeurs();
    else
        this->on_boutonConnexion_clicked();

    //récupération des paramètres passés à  l'application
    readCommandLineArguments();
}

MainWindow::~MainWindow()
{
    qDebug() << "Destruction MainWindow";

    // Note:
    // voir ici => www.siteduzero.com/tutoriel-3-11256-heriter-un-widget.html
    // A partir du moment où, on a crée un pointeur avec en widget parent this;
    // Lorsque le widget parent est détruit, Qt détruit aussi ses enfants.
    // Notons que je n'ai que rarement spécifié this dans le constructeur de chacun de ces objets créés dynamiquement...
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    //http://qt.developpez.com/faq/?page=qt4Gui#qt4gui-dragdrop
    //http://www.informit.com/articles/article.aspx?p=1405546

    //Le problème ici, c'est que le survol de chaque widget (fenêtre principale, chekbox, groupbox, tablewidget) génère ce signal.

    qDebug() << "Appel de la fonction DragEnterEvent";
    // qDebug() << tableWidget->geometry();
    // qDebug() << tableWidget->pos();
    // qDebug() << event->pos();

    // Vérification du drag dans un widget autorisé (le faire ici et pas après permet d'économiser le nombre de tests sur les fichiers déposés)
    // if (!groupBoxCompression->frameGeometry().contains(event->pos()) && !tableWidget->frameGeometry().contains(event->pos())) {
    //     event->ignore();
    //     return;
    // }

    // N'accepte que le mime format "text/uri-list" (pas d'images ou d'autres données)
    if (!event->mimeData()->hasFormat("text/uri-list"))
        return;

    // Plus de vérification de l'ouverture du fichier.
    // Ça ne sert à rien de vérifier ici vu que lors de l'ajout dans le tableau, cette vérification est faite.
    // De plus l'event Drag est appelé souvent à chaque passage de la souris au dessus d'un widget donc... c'est lourd !
    event->acceptProposedAction();
    /*
    bool validation = false;

    // On vérifie que c'est bien un fichier qui est utilisé pour le glisser-déposer
    if(event->mimeData()->hasUrls()) {
        //Récupération du chemin du fichier
        for (int i = 0; i < event->mimeData()->urls().count(); i++)
        {
            qDebug() << "Donnée :" << event->mimeData()->urls().at(i);

            QFile file(event->mimeData()->urls().at(i).toLocalFile());
            //Test de validité
            if (testFichier(file) == true) {
                qDebug() << "ok drag : " + file.fileName();
                validation = true;
            }
            else {
                QMessageBox::information(this, tr("Info"), tr("Le glisser-déposer contient un fichier non valide !"));
                validation = false;
            }
        }
    }

    if (validation == true) {
        event->acceptProposedAction();
        return;
    }

    event->ignore();
    */
}

void MainWindow::dropEvent(QDropEvent *event)
{
    // Vérification de l'onglet du drop. On pourrait aussi vérifier la position du drag mais déjà fait dans dragEnterEvent..
    // Pour faire la différence entre un fichier et un dossier: mettre l'url dans un QFileInfo et tester: isDir() ou isFile()

    if (tabWidget->currentIndex() == 1) {
        qDebug() << "Drop Action :: GroupBoxCompression";

        // Ajout du premier fichier proposé dans le lineEdit contenant le fichier source à  compresser (on les fait un par un svp)
        qDebug() << "Objet drag & drop :" << event->mimeData()->urls().at(0);

        ajoutFichierCompress(event->mimeData()->urls().at(0).toLocalFile());

    } else if (tabWidget->currentIndex() == 0) {
        qDebug() << "Drop Action :: TableWidget";

        // On récupère tous les fichiers sélectionnés par l'utilisateur pour le glisser-déposer
        // et on s'assure qu'il s'agisse bien d'un fichier et non d'autre chose..
        QFileInfo fileInfo;
        QStringList fileList;

        //On parcourt l'event et on ajoute dans la liste
        foreach(const QUrl & url, event->mimeData()->urls()) {
            // Test fichier ou autre chose ?
            fileInfo.setFile(url.toLocalFile());
            if (fileInfo.isFile()) {
                qDebug() << "Fichier drag & drop :" << url.toLocalFile();
                fileList.append(url.toLocalFile());
            }
        }
        creationTableau(AJOUT, fileList);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Surcharge la méthode closeEvent
    // Si un up est en cours, une fenetre apparait pour demander confirmation
    if (m_upEnCours == true) {
        // Vérification que l'application n'est pas iconifiée. Sinon l'affichage du QMessageBox la fait planter
        if(!isVisible()) {
            this->setWindowFlags(Qt::Window);
            this->show();
            this->activateWindow();
        }

        int ret = QMessageBox::question(this,tr("Quitter ?"),tr("Voulez-vous vraiment quitter ?\nLes tâches en cours seront perdues !"), QMessageBox::Yes | QMessageBox::No);

        if (ret == QMessageBox::Yes)
            event->accept();
        else
            event->ignore();
    } else {
        // Si pas d'up en cours => fermeture
        event->accept();
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    // Réimplémentation de showEvent pour l'affichage de QWinTaskbarProgress
    #ifdef WINDOWS
        m_taskbarButton->setWindow(windowHandle());
    #endif

    event->accept();
}

void MainWindow::on_tableWidget_customContextMenuRequested(QPoint point)
{
    //Gère le menu activé par le clic droit souris sur le tableau

    // Si aucune case n'est présente sous le clic droit souris
    if (!tableWidget->itemAt(point))
        return;

    //qDebug() << "item valide";

    // Création du menu
    QMenu menu(this);

    if (tableWidget->itemAt(point)->column() == 5) { //Si le clic droit est fait sur la colonne 5 => copier
        menu.addAction(actionCopier);
    }
    else {
        // Récupération de la ligne du tableau concernée
        int row = tableWidget->itemAt(point)->row();
        //qDebug() << "ligne" << row;

        // Spécifie la case courante pour que les slots sachent où agir
        // je reporte cela dans les SLOTS pour éviter que la case active ne change avant le choix => coté pratique pr l'utilisateur
        // tableWidget->setCurrentItem(tableWidget->item(tableWidget->currentItem()->row(), 1));

        switch(m_etatsFichiers.at(row))
        {
            case UPLOADING :    // "En cours d'upload" => "Arreter"
                                actionArreter->setEnabled(true);
                                actionAnnuler->setDisabled(true);
                                actionEnAttente->setDisabled(true);
                                break;

            case READY :        // "En attente" => "Annuler"
                                actionArreter->setDisabled(true);
                                actionAnnuler->setEnabled(true);
                                actionEnAttente->setDisabled(true);
                                break;

            case STOPED :       // "Arreté" ou "Annulé" => "En attente"
                                actionArreter->setDisabled(true);
                                actionAnnuler->setDisabled(true);
                                actionEnAttente->setEnabled(true);
                                break;

            case REMOVED :      // "Arreté" ou "Annulé" => "En attente"
                                actionArreter->setDisabled(true);
                                actionAnnuler->setDisabled(true);
                                actionEnAttente->setEnabled(true);
                                break;

            default :           // "Terminé" ou "Erreur" ou "selection serveur" => Ne rien faire
                                actionArreter->setDisabled(true);
                                actionAnnuler->setDisabled(true);
                                actionEnAttente->setDisabled(true);
                                break;
                                // /!\ PS: si le statut est à  PREPROCESSING c'est le merdier.
                                //L'up ne se fera pas l'objet est sauté et cette fonction, ne permet pas de modifier le statut pour remettre le fichier.
                                //Mais si il est bloqué en PREPROCESSING, l'objet et le thread SelectionServeur ne sont jamais détruits.. => fuite mémoire
        }
        // Positionner les actions
        menu.addAction(actionArreter);
        menu.addAction(actionAnnuler);
        menu.addAction(actionEnAttente);
    }

    // Afficher le menu
    menu.exec(QCursor::pos());
}

void MainWindow::actionArreter_triggered()
{
    // Arreter l'upload ici
    boutonUpload->setEnabled(false);
    m_upCurl->arretCurl();

    // L'arret de l'up se charge de rétablir l'interface et de mettre à  jour l'état du fichier
    m_etatsFichiers[m_row] = STOPED; // Juste pour que les tailles soient mises à jour dans la foulée et non au prochain ajout ou manip de l'état des fichiers
    calculTailleTotale();
}

void MainWindow::actionAnnuler_triggered()
{
    int row = tableWidget->currentItem()->row();

    // Spécifie la case courrante ("état") pour que les slots sachent où agir
    m_etatsFichiers[row] = REMOVED;
    remplissageTableau(row, 1, REMOVED_TEXT());
    tableWidget->item(row, 1)->setIcon(QIcon(REMOVED_IMG));

    calculTailleTotale();
}

void MainWindow::actionEnAttente_triggered()
{
    int row = tableWidget->currentItem()->row();

    // Spécifie la case courrante ("état") pour que les slots sachent où agir
    m_etatsFichiers[row] = READY;
    remplissageTableau(row, 1, READY_TEXT());
    tableWidget->item(row, 1)->setIcon(QIcon(READY_IMG));

    calculTailleTotale();
}

void MainWindow::actionCopier_triggered()
{
    qDebug() << "Copie du lien demandée";
    int row = tableWidget->currentItem()->row();

    //Copie dans le presse papier
    QApplication::clipboard()->setText(m_liensFichiers.at(row));
}

void MainWindow::trayClicked(QSystemTrayIcon::ActivationReason reason)
{
    // Meme comportement que VLC (sous linux en tout cas).
    // L'icone est tout le temps présente dans le systray. 1 clic dessus actve ou réduit la fenetre.
    // La minimisation reste non touchée et n'engendre pas l'iconification.
    // Sous linux: cela évite les bugs quand on change de bureau et qu'une fenetre ou un QMessageBox est actif. Le programme s'iconifiait mais pas son enfant.
    //qDebug() << reason;

    // PS : pour détecter si une fenetre est en arrière plan, il faudrait détecter le focus ou les signaux Activated/Deactivated...
    /*if (isActiveWindow())
        qDebug() << "fenetre active";
    if (isMinimized())
        qDebug() << "fenetre minimisée";
    if (this->isVisible())
        qDebug() << "Fenetre visible";
        */

    if (reason == QSystemTrayIcon::Trigger) { //Clic gauche

        if (isVisible()) {
            if (isMinimized()) { // Restaure la fenêtre
                this->showNormal();

                this->activateWindow();
                return;
            }

            // La fenêtre est active mais en avant ou arrière plan => on réduit en systray
            setWindowFlags(Qt::FramelessWindowHint);
            this->hide();
            this->setWindowState(windowState() & ~Qt::WindowMinimized);
            actionSysTrayRetablir->setText(tr("Rétablir Multiup MaNaGeR"));
        }
        else {  // L'app est dans le systray
            this->setWindowFlags(Qt::Window);
            this->show();
            this->activateWindow();
            actionSysTrayRetablir->setText(tr("Cacher Multiup MaNaGeR"));
        }
    }
}

void MainWindow::actionSysTrayRetablir_triggered()
{
    // Action "Rétablir" du menu du systray
    if (isVisible()) {
        setWindowFlags(Qt::FramelessWindowHint);
        this->hide();
        this->setWindowState(windowState() & ~Qt::WindowMinimized);
        actionSysTrayRetablir->setText(tr("Rétablir Multiup MaNaGeR"));
    }
    else {
        this->setWindowFlags(Qt::Window);
        this->show();
        this->activateWindow();
        actionSysTrayRetablir->setText(tr("Cacher Multiup MaNaGeR"));
    }
}

void MainWindow::actionSysTrayArreter_triggered()
{
    this->on_boutonUpload_clicked();
}

void MainWindow::on_checkBoxExtinction_clicked()
{
    // Fonction gérant l'état de la checkBox "éteindre l'ordinateur"
    // Le booléen et l'état de la statusbar
    if (checkBoxExtinction->isChecked()) {
        m_extinction = true;
        m_labelExtinction->setText(QString(tr("Extinction : Activée")));
    }
    else {
        m_extinction = false;
        m_labelExtinction->setText(QString(tr("Extinction : Désactivée")));
    }
}

void MainWindow::on_checkBoxReprise_clicked()
{
    // Fonction gérant l'état de la checkBox "reprise sur erreur"
    // Mise à jour du booléen et de la statusbar
    if (checkBoxReprise->isChecked()) {
        m_reprise = true;
        m_labelReprise->setText(QString(tr("Reprise : Activée")));
    }
    else {
        m_reprise = false;
        m_labelReprise->setText(QString(tr("Reprise : Désactivée")));
    }
}

void MainWindow::on_checkBoxEffacement_clicked()
{
    // Fonction gérant l'état de la checkBox "effacement programmé"
    // Mise à jour du booléen et de la statusbar
    if (checkBoxEffacement->isChecked()) {
        m_effacement = true;
        m_labelEffacement->setText(QString(tr("Effacement : Activé")));
    }
    else {
        m_effacement = false;
        m_labelEffacement->setText(QString(tr("Effacement : Désactivé")));
    }
}

void MainWindow::on_checkBoxPrez_clicked()
{
    // Fonction gérant l'état de la checkBox "aide à la présentation"
    // Mise à jour du booléen et de la statusbar
    if (checkBoxPrez->isChecked()) {
        m_prez = true;
        m_labelPrez->setText(QString(tr("Prez : Activée")));
    }
    else {
        m_prez = false;
        m_labelPrez->setText(QString(tr("Prez : Désactivée")));
    }
}

void MainWindow::on_checkBoxRegVitesse_clicked()
{
    // Fonction gérant l'état de la checkBox "Bridage de connexion"
    // Mise à jour du booléen et de l'état de l'upload,
    // ainsi que de la couleur de la taskbar sous Windows
    if (checkBoxRegVitesse->isChecked()) {
        m_regVitesse = true;

        // Changement de couleur de la taskbar
        #ifdef WINDOWS
            m_taskbarProgress->setPaused(true);
        #endif
    }
    else {
        m_regVitesse = false;

        // Changement de couleur de la taskbar
        #ifdef WINDOWS
            m_taskbarProgress->setPaused(false);
        #endif

        if ((m_upEnCours == true) && (m_upCurl != NULL)) { // ssi un up est en cours et l'objet dans le thread d'up est valide
            // On s'assure que unpause sera appelé
            m_upCurl->unpause();
        }
    }
}

void MainWindow::on_horizontalSliderRegVitesse_valueChanged(int value)
{
    // Bridage de la vitesse d'upload selon le curseur de réglage
    QString val;

    switch(value)
    {
        case 1: val = "1 %";
                break;

        case 2: val = "25 %";
                break;

        case 3: val = "50 %";
                break;

        case 4: val = "75 %";
                break;

        default: break;
    }

    checkBoxRegVitesse->setText(tr("Bridage à : ") + val);
}

void MainWindow::on_boutonSuppressionTout_clicked()
{
    // Gestion des uploads: Suppression de tout le tableau
    tableWidget->clearContents();
    tableWidget->setRowCount(0);
    m_cheminsFichiers.clear();
    m_etatsFichiers.clear();
    m_tailleFichiers.clear();
    m_liensFichiers.clear();
}

void MainWindow::on_boutonSuppressionAnnules_clicked()
{
    // Gestion des uploads: Suppresion des éléments annulés (et avec erreur)
    suppressionTableau(REMOVED);
}

void MainWindow::on_boutonSuppressionTermines_clicked()
{
    // Gestion des uploads: Suppresion des éléments terminés (et avec erreur)
    suppressionTableau(FINISHED);
}

void MainWindow::suppressionTableau(const EtatFichier state)
{
    // Parcourir le tableau (colonne 1), si "Annulé" / "Terminé" ou "Erreur"
    // effacer l'item concerné
    // effacer le tableau
    // réinitialise m_cheminsFichiers
    // remplir le tableau avec m_cheminsFichiers

    if (m_cheminsFichiers.count() == 0)
        return;

    int numeroLigne = 0;

    while (numeroLigne < m_cheminsFichiers.count())
    {
        if ((m_etatsFichiers.at(numeroLigne) == state) || (m_etatsFichiers.at(numeroLigne) == FAILED_e)) {
            qDebug() << m_cheminsFichiers.at(numeroLigne);
            m_cheminsFichiers.removeAt(numeroLigne);
            m_etatsFichiers.removeAt(numeroLigne);
            m_tailleFichiers.removeAt(numeroLigne);
            m_liensFichiers.removeAt(numeroLigne);
            numeroLigne--;
        }
        numeroLigne++;
    }

    tableWidget->clearContents();
    tableWidget->setRowCount(0);

    // TODO: Useless
    creationTableau(RAZ, m_cheminsFichiers);
}

void MainWindow::on_boutonOuvrir_clicked()
{
    // TODO: Il est possible que l'ouverture de cette boite de dialogue (et de la boite affichant les liens),
    // pendant qu'une autre tâche s'exécute dans la fenêtre principale fasse planter
    // (jamais arrivé) ou donne des bugs (ex: affichage des petites icones qui ne se fait pas).
    #ifdef WINDOWS
        QStringList liste = QFileDialog::getOpenFileNames(
                    this, tr("Ouvrir un fichier"),
                    "C:\\", tr("Archives (*.rar *.zip *.7z *.iso *.tar *.gz *.bz *.bz2);;Vidéos (*.avi *.mpg *.mpeg *.mkv *.flv);;Images (*jpg *.jpeg *.png *.bmp);;Tous les fichiers (*.*)"));
    #endif
    #ifdef LINUX
        QStringList liste = QFileDialog::getOpenFileNames(
                    this, tr("Ouvrir un fichier"),
                    "/", tr("Archives (*.rar *.zip *.7z *.iso *.tar *.gz *.bz *.bz2);;Vidéos (*.avi *.mpg *.mpeg *.mkv *.flv);;Images (*jpg *.jpeg *.png *.bmp);;Tous les fichiers (*.*)"));
    #endif

    if (liste.isEmpty())
        return;

    //qDebug() << "Fichiers à  ajouter :" << liste;
    creationTableau(AJOUT, liste);
}

void MainWindow::creationTableau(EtatTableau etat, const QStringList liste)
{
    // Parcourt la liste
    // Teste l'ouverture du fichier
    // Parcourt le tableau, respecte les lignes déjà  présentes => cette fonction n'efface RIEN
    // Parcourt tous les fichiers (m_cheminsFichiers) pour les repasser à  la fonction d'upload qui se chargera de savoir si ils sont uploadables ou pas.
    // Additionne la taille de tous les fichiers pour connaitre la taille totale à  up (utilisation pour la barre de progression générale)

    // Modification de la liste en cours d'upload:
    // si cela se fait au moment du début d'un up ou du choix serveur cela va planter, car ces fonctions utilisent m_cheminsFichiers)
    // Solution : les fonctions supprimant les items sont dispo ssi aucun upload n'est en cours

    // !!! on parcourt la liste !!!
    // si on doit ajouter c'est à  la derniere ligne qu'on vient de créer
    // en temps normal etat = FALSE => on construit avec des valeurs neuves en ajoutant 1 ligne
    // en cas de reconstruction apres item supprimé etat = TRUE => on détruit le tableau et on le construit avec des valeurs anciennes des QStringList en mémoire.


    if (etat == AJOUT) {
        // Remplissage par ajout de ligne à  la fin des lignes existantes

        // Si count = 0 : 1iere ligne du tableau donc numéro 0 etc..
        int numeroLigne = m_cheminsFichiers.count();


        int i = 0;
        while (i < liste.count())
        {
            //On garde les lignes déjà  existantes, donc dans tous les cas on en rajoute une.
            tableWidget->setRowCount(tableWidget->rowCount()+1);

            m_cheminsFichiers.append(liste[i]);
            remplissageTableau(numeroLigne, 0, QFileInfo(liste[i]).fileName());

            QFile file(liste[i]);
            EtatFichier statutFichier;
            if (testFichier(file) == true) { // Fichier valide
                statutFichier = READY;
                remplissageTableau(numeroLigne, 1, READY_TEXT());
                tableWidget->item(numeroLigne, 1)->setIcon(QIcon(READY_IMG));
            }
            else { // Fichier illisible ou non ouvrable
                statutFichier = FAILED_e;
                remplissageTableau(numeroLigne, 1, FAILED_TEXT());
                tableWidget->item(numeroLigne, 1)->setIcon(QIcon(FAILED_IMG));
            }

            m_etatsFichiers.append(statutFichier);

            // Attention, la grille contient des qint64 avec des bytes alors que ce qui est affiché dans le tableau est converti en Mio
            qint64 taille = QFileInfo(liste[i]).size();
            m_tailleFichiers.append(taille);
            remplissageTableau(numeroLigne, 3, QString::number(taille / (1024 * 1024)));

            remplissageTableau(numeroLigne, 4, tr("NC."));

            m_liensFichiers.append(tr("NC."));
            remplissageTableau(numeroLigne, 5, tr("NC."));

            i++;
            numeroLigne++;
        }
    }
    else {
        // etat = RETRAIT ou RAZ:
        // la construction se fait à  la suite d'un retrait d'un item par les toolsbuttons
        int numeroLigne = 0;

        while (numeroLigne < m_cheminsFichiers.count())
        {
            // On garde les lignes déjà  existantes, donc dans tous les cas on en rajoute une.
            tableWidget->setRowCount(tableWidget->rowCount()+1);

            remplissageTableau(numeroLigne, 0, QFileInfo(m_cheminsFichiers.at(numeroLigne)).fileName());

            switch (m_etatsFichiers.at(numeroLigne))
            {
                case STOPED:    remplissageTableau(numeroLigne, 1, STOPED_TEXT());
                                tableWidget->item(numeroLigne, 1)->setIcon(QIcon(STOPED_IMG));
                                break;
                case REMOVED:   remplissageTableau(numeroLigne, 1, REMOVED_TEXT());
                                tableWidget->item(numeroLigne, 1)->setIcon(QIcon(REMOVED_IMG));
                                break;
                case FAILED_e:  remplissageTableau(numeroLigne, 1, FAILED_TEXT());
                                tableWidget->item(numeroLigne, 1)->setIcon(QIcon(FAILED_IMG));
                                break;
                case FINISHED:  remplissageTableau(numeroLigne, 1, FINISHED_TEXT());
                                tableWidget->item(numeroLigne, 1)->setIcon(QIcon(FINISHED_IMG));
                                break;
                case PREPROCESSING:      remplissageTableau(numeroLigne, 1, PREPROCESSING_TEXT());
                                tableWidget->item(numeroLigne, 1)->setIcon(QIcon(PREPROCESSING_IMG));
                                break;
                case READY:     remplissageTableau(numeroLigne, 1, READY_TEXT());
                                tableWidget->item(numeroLigne, 1)->setIcon(QIcon(READY_IMG));
                                break;
                case UPLOADING: remplissageTableau(numeroLigne, 1, UPLOADING_TEXT());
                                tableWidget->item(numeroLigne, 1)->setIcon(QIcon(UPLOADING_IMG));
                                break;
                default:
                                remplissageTableau(numeroLigne, 1, FAILED_TEXT());
                                tableWidget->item(numeroLigne, 1)->setIcon(QIcon(FAILED_IMG));
                                break;
            }

            // Attention, la grille contient des qint64 avec des bytes alors que ce qui est affiché dans le tableau est converti en Mio
            remplissageTableau(numeroLigne, 3, QString::number((m_tailleFichiers.at(numeroLigne) / (1024 * 1024))));
            remplissageTableau(numeroLigne, 4, tr("NC."));
            remplissageTableau(numeroLigne, 5, m_liensFichiers.at(numeroLigne));

            numeroLigne++;
        }
    }
    calculTailleTotale();
}

void MainWindow::remplissageTableau(int row, int column, const QString text)
{
    // Remplit une case du tableau ayant les coordonnées données avec le texte donné
    if (!tableWidget->item(row, column)) {
        // L'item n'existe pas encore, on le crée
        tableWidget->setItem(row, column, new QTableWidgetItem(text));
    }
    else {
        tableWidget->item(row, column)->setText(text);
    }
}

bool MainWindow::testFichier(QFile &file) const
{
    // Teste la lecture du fichier traité (lors du remplisage du tableau)
    // Retourne true si le fichier existe et que les droits en lecture sont présents
    if (!file.exists())
        return false;

    if (!file.open(QIODevice::ReadOnly))
        return false;

    file.close();
    return true;
}

void MainWindow::calculTailleTotale()
{
    // Evaluation des tailles de l'upload
    int    numeroLigne   = 0;
    qint64 sizeTotTemp   = 0;
    qint64 sizeFaiteTemp = 0;

    // On garde la taille des fichiers PREPROCESSING, READY, UPLOADING
    foreach(const qint64 & taille, m_tailleFichiers)
    {
        switch(m_etatsFichiers.at(numeroLigne))
        {
            case STOPED:    break;

            case REMOVED:   break;

            case FAILED_e:  break;

            case FINISHED:  break;

            case UPLOADING: // Cas particulier d'un up en cours et de l'ajout/retrait d'un fichier:
            // Réinitialiser m_sizeTot est bien mais il faut aussi s'occuper de m_sizeFaite.
            // On ajoute donc à m_sizeFaite la taille du fichier qui est UPLOADING.
            // Ainsi, à la moindre manipulation de l'état des fichiers, la barre de progression est mise à jour enfonction de ce qui reste à faire.
                            if (m_upEnCours)
                                sizeFaiteTemp += taille;

                            sizeTotTemp += taille;
                            break;

            default:        sizeTotTemp += taille;
                            break;

        }
        numeroLigne++;
    }

    // /!\ Voir au dessus. Sais pas si très utile ici vu que il n'est pas censé avoir de statut UPLOADING quand m_upEnCours = false...
    if (m_upEnCours == true) {
        m_sizeFaite = sizeFaiteTemp;
    }

    m_sizeTot = sizeTotTemp;
    qDebug() << "Taille totale des uploads dans le tableau : " << m_sizeTot;
}

void MainWindow::on_boutonLiens_clicked()
{
    // Affiche la liste des liens obtenus
    FenLiens *fenetreLiens = new FenLiens(m_liens, this);
    fenetreLiens->exec();
    fenetreLiens->deleteLater();
}

void MainWindow::on_descriptionText_linkActivated(const QString &link)
{
    qDebug() << "Click on link: " << link;

    if(link == "#about_Qt")
        qApp->aboutQt();

    if(link == "#license")
        QDesktopServices::openUrl(QUrl(URL_GITHUB_LICENSE));

    if(link == "#website")
        // Ouvre l'url du site
        QDesktopServices::openUrl(QUrl(URL_MULTIUP));

    if(link == "#changelog") {
        FenVoirMiseAJour *window = new FenVoirMiseAJour(this);
        connect(window, SIGNAL(finished(int)), window, SLOT(deleteLater()));
        window->show();
    }

    if(link == "#support")
        // Ouvre l'url du site
        QDesktopServices::openUrl(QUrl(tr("https://www.multiup.org/fr/premium")));

    if(link == "#check_update")
        checkUpdateWindow();
}

void MainWindow::checkUpdateWindow(bool hidden)
{
    // Display the QDialog that checks if there is an available update
    InfoNewVersion *window = new InfoNewVersion(hidden, this);
    connect(window, SIGNAL(finished(int)), window, SLOT(deleteLater()));
    window->hide();
}

void MainWindow::readCommandLineArguments()
{
    //Récupération de tous les arguments passés en ligne de commande.
    QStringList args;
    args << QApplication::arguments();
    //Si il y a au moins 1 argument passé en manuel (le premier est toujours le nom de l'application)
    if (args.count() > 1) {
        //Recherche de l'aide
        QRegExp rx("--help");
        rx.setCaseSensitivity(Qt::CaseInsensitive);
        int pos = args.indexOf(rx);
        if (pos != -1) {
            QMessageBox::information(this, tr("Aide"), tr("Syntaxe à utiliser : \nMultiUp.exe --login [nom] --read [fichiers]"));
        }

        //Recherche du login
        rx.setPattern("--login");
        pos = args.indexOf(rx);
        //On parcourt la commande à  partir de la position trouvée.
        //En éliminant les options --read et --password suivantes présentes en cas d'omission du login.
        if (pos != -1 && ((pos + 1) < args.count()) && (args[pos + 1] != "--read") && (args[pos + 1] != "--Read") && (args[pos + 1] != "--password") && (args[pos + 1] != "--Password")) {
            m_login->setText(args[pos + 1]);
        }

        //Recherche du password
        rx.setPattern("--password");
        pos = args.indexOf(rx);
        //On parcourt la commande à  partir de la position trouvée.
        //En éliminant les options --read et --login suivantes présentes en cas d'omission du login.
        if (pos != -1 && ((pos + 1) < args.count()) && (args[pos + 1] != "--read") && (args[pos + 1] != "--Read") && (args[pos + 1] != "--login") && (args[pos + 1] != "--Login")) {
            m_password->setText(args[pos + 1]);
        }

        //Recherche des chemins des fichiers.
        rx.setPattern("--read");
        pos = args.indexOf(rx);
        //L'option a été trouvée.
        if (pos != -1) {
            QStringList liste;
            QFile file;
            //On parcourt la commande jusqu'à  la fin en s'arretant à  l'option --login et --password.
            for (int i = pos + 1; i < args.count() && (args[i] != "--login") && (args[i] != "--Login") && (args[i] != "--password") && (args[i] != "--Password"); i++)
            {
                //Test de la validité du fichier (fait doublon avec le remplissage du tableau !!!..)
                file.setFileName(args[i]);
                if (testFichier(file) == true) {
                    liste << args[i];
                    //QMessageBox::information(this, "Test", args[i]);
                }
            }
            //On remplit le tableau
            if (!liste.isEmpty()) {
                creationTableau(AJOUT, liste);
            }
        }
    }
}

void MainWindow::on_boutonSavParametres_clicked()
{
    // Lit ou écrit les paramètres dans le fichier de config.ini suite au clic sur le bouton de sauvegarde

    if (m_configParametres) {
        // Le fichier a déjà été sauvegardé, on recharge les paramètres
        readSettings();

        boutonSavParametres->setText(tr("Sauvegarder"));
        m_configParametres = false;
        return;
    }

    // Sauvegarde des paramètres
    QSettings settings(m_cheminSettingsFile, QSettings::IniFormat);

    // Sauvegarde des identifiants
    settings.beginGroup("login");
    settings.setValue("username", m_login->text());
    settings.setValue("password", m_password->text());
    settings.endGroup();

    if (m_cheminWinRar.isEmpty()) {
        #ifdef WINDOWS
            settings.setValue("compression/rar_binary_path", "C:\\Program Files\\WinRAR\\Rar.exe");
        #endif
        #ifdef LINUX
            settings.setValue("compression/rar_binary_path", "rar");
        #endif
    }

    settings.setValue("compression/output_path", lineEditCompressDest->text());
    settings.setValue("compression/password", lineEditCompressMdp->text());

    settings.setValue("misc/check_update", checkBoxCheckUpdate->isChecked());

    boutonSavParametres->setText(tr("Recharger"));
    m_configParametres = true;
}

void MainWindow::readSettings()
{
    // Récupère le contenu des clés du fichier config.ini situé dans le répertoire de l'application et remplit les champs et valeurs
    qDebug() << "Current config file : " << m_cheminSettingsFile;

    if (!QFile(m_cheminSettingsFile).exists())
        return;

    QSettings settings(m_cheminSettingsFile, QSettings::IniFormat);

    m_login->setText(settings.value("login/username").toString());
    m_password->setText(settings.value("login/password").toString());

    m_cheminWinRar = settings.value("compression/rar_binary_path").toString();
    lineEditCompressDest->setText(settings.value("compression/output_path").toString());
    lineEditCompressMdp->setText(settings.value("compression/password").toString());
    qDebug() << "Settings loaded : " << m_login->text() << m_password->text() << m_cheminWinRar;

    checkBoxCheckUpdate->setChecked(settings.value("misc/check_update").toBool());
}

void MainWindow::on_boutonCompressSrc_clicked()
{
    // Récupère le nom du fichier à  compresser
    // Calcule le nombre de parties à  faire pour avoir des fichiers de taille inférieure à  990 Mio
    // Remplit le label avec la taille du fichier en Mio
    // Demande un nom aléatoire pour le fichier

    #ifdef WINDOWS
        QString fichier = QFileDialog::getOpenFileName(
                    this, tr("Ouvrir un fichier"), "C:\\",
                    tr("Tous les fichiers (*.*);;Archives (*.rar *.zip *.7z *.iso *.tar *.gz *.bz *.bz2);;Vidéos (*.avi *.mpg *.mpeg *.mkv *.flv);;Images (*jpg *.jpeg *.png *.bmp)"));
    #endif
    #ifdef LINUX
        QString fichier = QFileDialog::getOpenFileName(
                    this, tr("Ouvrir un fichier"), "/",
                    tr("Tous les fichiers (*.*);;Archives (*.rar *.zip *.7z *.iso *.tar *.gz *.bz *.bz2);;Vidéos (*.avi *.mpg *.mpeg *.mkv *.flv);;Images (*jpg *.jpeg *.png *.bmp)"));
    #endif

    qDebug() << "Compression :: Fichier selectionne :" << fichier;

    if (fichier == "")
        return;

    ajoutFichierCompress(fichier);
}

void MainWindow::ajoutFichierCompress(QString fichier)
{
    // Remplissage du LineEdit (rappel : il est actif mais non éditable)
    lineEditCompressSrc->setText(fichier);

    // Opérations sur la taille du fichier et distinction fichier/dossier
    QFileInfo fileInfo = QFileInfo(fichier);
    double taille = 0; // initialisation par def...

    if (fileInfo.isFile()) {
        // Taille fichier
        taille = conversion(QFileInfo(fichier).size());
    }
    if (fileInfo.isDir()) {
        // Taille dossier et sous répertoires
        taille = conversion(sizeOfdir(fichier));
    }

    // Calcul de la taille en Mio;
    taille = (taille / (1024 * 1024));
    qDebug() << "Compression :: Taille fichier en Mio: " << taille;

    // Mémorisation de la taille dans le label (ce qui permet à  l'utilisateur de voir la taille totale)
    labelCompressTailleFichier->setText(QString::number(taille));

    // On divise par 1990Mio et on sort le nombre de parties + 1
    taille = taille / 1990;
    qDebug() << "Compression :: Nb de parties déduit: " << conversion(taille + 1);
    lineEditCompressNb->setText(QString::number(conversion(taille + 1)));

    // Remplissage automatique d'un nom aléatoire
    lineEditCompressNom->setText(randomName());
}

void MainWindow::on_boutonCompressDest_clicked()
{
    // Récupère le dossier de destination du fichier compressé

    QString dossier = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier..."), "", QFileDialog::ShowDirsOnly);

    qDebug() << "Compression :: Dossier selectionne :" << dossier;

    if (dossier == "") //doit etre testé avant, car la fonction d'enregistrement enregistre par défaut dans le dossier de l'application (meme si dossier="")
        return;

    // Remplissage du LineEdit (rappel : il est actif mais non éditable)
    lineEditCompressDest->setText(dossier);
}

void MainWindow::on_boutonCompress_clicked()
{
    // Fonction analysant le remplissage des champs
    // Modifiant l'interface le temps de la compression
    // Forge la commande qui sera passée à  winrar
    // Démarre le thread et gère ses signaux

    bool mdp = false;

    if (m_cheminWinRar.isEmpty()) {
        QMessageBox::information(this, tr("Erreur"), tr("Vous devez spécifier le chemin de l'application 'rar' dans config.ini !\n"
                                                        "(Cliquez sur \"Sauvegarder les paramètres\" pour y enregistrer un exemple de configuration.)"));
        return;
    }
    if (lineEditCompressSrc->text() == "") {
        QMessageBox::information(this, tr("Erreur"), tr("Veuillez choisir un fichier source !"));
        return;
    }
    else if (lineEditCompressDest->text() == "") {
        QMessageBox::information(this, tr("Erreur"), tr("Veuillez choisir un répertoire de destination !"));
        return;
    }
    else if (lineEditCompressNom->text() == "") {
        QMessageBox::information(this, tr("Erreur"), tr("Veuillez choisir un nom de sortie !"));
        return;
    }
    else if (lineEditCompressNb->text() == "") {
        QMessageBox::information(this, tr("Erreur"), tr("Veuillez choisir le nombre de parties pour le fichier découpé !"));
        return;
    }
    else if (lineEditCompressMdp->text() != "") {
        mdp = true;
    }

    // Désactivation de l'interface pendant la compression
    boutonCompressSrc->setEnabled(false);
    boutonCompressDest->setEnabled(false);
    lineEditCompressNom->setEnabled(false);
    lineEditCompressNb->setEnabled(false);
    lineEditCompressMdp->setEnabled(false);
    boutonCompress->setEnabled(false);

    int     nbParties       = lineEditCompressNb->text().toInt();
    double  tailleFichier   = labelCompressTailleFichier->text().toDouble(); // C'est une valeur en Mio !

    // Conception de la commande winrar
    // PS: On arrondi au Mio supérieur (Attention aux divisions d'int !, De plus la partie 1 doit être de taille supérieure à  la partie 2)
    //a     : append
    //-hp   : password
    //-v    : taille des volumes
    //-m0   : stocker
    //-ep1  : respect de l'arborescence du dossier compressé. Vire tous les dossiers présents avant.
    QStringList arguments;
    arguments     << "a";

    if (mdp) {
        arguments << "-hp" + lineEditCompressMdp->text();
    }

    arguments     << "-v" + QString::number(conversion((tailleFichier / nbParties) + 1)) + "m"
                  << "-m0"
                  << "-ep1"
                  #ifdef WINDOWS
                    << lineEditCompressDest->text() + "\\" + lineEditCompressNom->text()
                  #endif
                  #ifdef LINUX
                    << lineEditCompressDest->text() + "/" + lineEditCompressNom->text()
                  #endif
                  << lineEditCompressSrc->text();

    qDebug() << "Compression :: Commande :" << m_cheminWinRar << arguments <<
                "Taille des parties :" << tailleFichier << "/" << nbParties << "=" << tailleFichier / nbParties;

    // Démarrage du thread
    QThread *thread = new QThread(this);
    m_compression = new Compression(m_cheminWinRar, arguments, NULL);
    m_compression->moveToThread(thread);

    connect(thread, SIGNAL(started()), m_compression, SLOT(demarrage()));
    connect(m_compression, SIGNAL(emissionCompressionAvancement(int)), this, SLOT(receptionCompressionAvancement(int)));
    connect(m_compression, SIGNAL(emissionCompressionEtape(QString)), this, SLOT(receptionCompressionEtape(QString)));
    connect(m_compression, SIGNAL(emissionCompressionEtat(bool)), this, SLOT(receptionCompressionEtat(bool)));

    // Attention : rétablir la possibilité de cliquer à  nouveau sur le bouton de connexion ssi on s'est assuré que l'objet connexion et son thread sont détruits !
    connect(m_compression, SIGNAL(emissionCompressionEtat(bool)), thread, SLOT(quit()));
    connect(m_compression, SIGNAL(emissionCompressionEtat(bool)), m_compression, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(destroyed()), this, SLOT(finThreadCompression()));

    // Démarrage du thread et donc de l'objet qui vit dedans
    thread->start();
}

void MainWindow::receptionCompressionAvancement(int avancementCompression)
{
    // Actualisation de la barre de progression indiquant l'avancée de la compression
    progressBarCompress->setValue(avancementCompression);
}

void MainWindow::receptionCompressionEtat(bool compressionSucess)
{
    // Récupère l'état de la compression
    // Si compressionSucess = 0 => compression ratée
    // Si compressionSucess = 1 => compression réussie
    // Si la compression est réussie, on appelle la fonction creationTableau avec les bons paramètres (contenant le chemin des fichiers compressés)
    // Rétablit l'interface

    // WARNING: sous windows, il est difficile d'obtenir la sortie standard complete de Rar.exe.
    // Du coup cela affiche compression échouée dans tous les cas
    // A la fin de cette fonction je ne tiens pas compte pour windows, de ce que renvoie le thread: je force compressionSucess à true
    #ifdef WINDOWS
        compressionSucess = true;
    #endif

    if (!compressionSucess) {
        // Compression ratée
        QMessageBox::information(this, tr("Compression échouée"), tr("La compression a échoué !"));
    }
    else {
        // Compression réussie
        int i = 1;
        QStringList liste;
        int nbParties = lineEditCompressNb->text().toInt();

        // Récupération des noms de fichiers et de leurs chemins, reconstruits pour l'ajout dans le tableWidget
        if (nbParties != 1) {
            do {
                #ifdef WINDOWS
                    liste << lineEditCompressDest->text() + "\\" + lineEditCompressNom->text() + ".part" + QString::number(i) + ".rar";
                #endif
                #ifdef LINUX
                    liste << lineEditCompressDest->text() + "/" + lineEditCompressNom->text() + ".part" + QString::number(i) + ".rar";
                #endif

                i++;
            } while (i <= nbParties);
        }
        else {
            #ifdef WINDOWS
                liste << lineEditCompressDest->text() + "\\" + lineEditCompressNom->text() + ".rar";
            #endif
            #ifdef LINUX
                liste << lineEditCompressDest->text() + "/" + lineEditCompressNom->text() + ".rar";
            #endif
        }

        // Ajout dans le fichier journal
        #ifdef WINDOWS
            enregistrement(QApplication::applicationDirPath(), lineEditCompressSrc->text() + "\n" + liste.join("\n"));
        #endif
        #ifdef LINUX
            enregistrement(QDir::homePath() + "/" + APP_DIR, lineEditCompressSrc->text() + "\n" + liste.join("\n"));
        #endif

        // Ajout dans le tableau
        creationTableau(AJOUT, liste);
    }

    // Rétablissement de l'interface => fait dans le slot finThreadCompression()
}

void MainWindow::receptionCompressionEtape(QString etapeCompression)
{
    labelCompressEtape->setText(etapeCompression);
}

void MainWindow::finThreadCompression()
{
    // Rétablissement de l'interface
    // On rétablit la possibilité d'édition des champs
    qDebug() << "Compression :: Rétablissement interface après compression";

    m_compression = NULL;
    progressBarCompress->setValue(0);

    boutonCompressSrc->setEnabled(true);
    lineEditCompressSrc->clear();
    boutonCompressDest->setEnabled(true);
    //lineEditCompressDest->clear();
    lineEditCompressNom->setEnabled(true);
    lineEditCompressNom->clear();
    labelCompressTailleFichier->clear();
    lineEditCompressNb->setEnabled(true);
    lineEditCompressNb->clear();
    lineEditCompressMdp->setEnabled(true);
    //lineEditCompressMdp->clear();
    labelCompressEtape->clear();
    boutonCompress->setEnabled(true);

    qDebug() << "Compression :: Fin";
}

void MainWindow::rechercheHebergeurs()
{
    ///////////////////////////////////////////////////////////////////
    // Modification de l'interface après connexion ou après changement des logins

    // On bloque temporairement la capacité de m_login et m_pasword à envoyer des signaux editingFinished
    // => evite de multiples récupérations d'icones lors de l'édition
    // => évite le plantage de l'app
    // On débloque les signaux à la fin de la récupération des hébergeurs
    m_login->blockSignals(true);
    m_password->blockSignals(true);

    // On RAZ la liste des hébergeurs prêts à l'upload en mémoire
    m_hebergeursListe.clear();

    // Réinitialisation du gridboxlayout
    // supprimer de manière récursive le contenu d'un QLayoutItem:
    // http://stackoverflow.com/questions/5395266/removing-widgets-from-qgridlayout
    //qDebug()<< "items en mémoire:" << gridLayoutHebergeurs->count();
    while (gridLayoutHebergeurs->count() > 0) {
        QLayoutItem *item = gridLayoutHebergeurs->takeAt(0);

        delete item->widget();
        delete item;
        //qDebug()<< "plus que:" << gridLayoutHebergeurs->count();
    }
    qDebug()<< "Reinitialisation gridboxlaout: items toujours en mémoire:" << gridLayoutHebergeurs->count();

    // Ajout du loader/spinbox
    label_8->movie()->start();
    label_8->show();

    ///////////////////////////////////////////////////////////////////
    // Création d'une nouvelle instance de thread
    QThread *thread = new QThread(this);

    // Création et envoi de l'objet dans le thread
    // Démarre la récupération des hébergeurs selon l'état de la connexion au site
    // anonyme / connecté
    m_recupHebergeurs = NULL;
    if (m_connecte)
        m_recupHebergeurs = new RecupHebergeurs(m_login->text(), m_password->text());
    else
        m_recupHebergeurs = new RecupHebergeurs(NULL);

    m_recupHebergeurs->moveToThread(thread);


    connect(thread, SIGNAL(started()), m_recupHebergeurs, SLOT(demarrage()));
    connect(m_recupHebergeurs, SIGNAL(emissionRecupHebergeursEtat(int)), this, SLOT(receptionRecupHebergeursEtat(int)));

    connect(m_recupHebergeurs, SIGNAL(emissionRecupHebergeursHebergeurs(QString, QString, bool, QString)), this, SLOT(receptionRecupHebergeursHebergeurs(QString, QString, bool, QString)));
    connect(m_recupHebergeurs, SIGNAL(emissionRecupHebergeursIcones(QByteArray, QString)), this, SLOT(receptionRecupHebergeursIcones(QByteArray, QString)));
    connect(m_recupHebergeurs, SIGNAL(emissionMaxSelectionHebergeurs(int)), this, SLOT(receptionMaxSelectionHebergeurs(int)));
    connect(m_recupHebergeurs, SIGNAL(finished()), thread, SLOT(quit()));

    connect(m_recupHebergeurs, SIGNAL(finished()), m_recupHebergeurs, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(destroyed()), this, SLOT(finThreadRecupHebergeurs()));

    // Démarrage du thread et donc de l'objet qui vit dedans
    thread->start();
}

void MainWindow::finThreadRecupHebergeurs()
{
    // Rétablissement interface
    qDebug() << "RecupHebergeurs :: Retablissement de l'interface";

    m_recupHebergeurs = NULL;

    // Retrait du loader/spinbox
    label_8->movie()->stop();
    label_8->hide();

    // On affiche les checkbox dans un ordre harmonieux (les déjà cochées en premier)
    for(int i = 0; i < m_sortedListCheckBox.count(); i++) {
        gridLayoutHebergeurs->addWidget(m_sortedListCheckBox.at(i), i / 4, i % 4);
    }
    m_sortedListCheckBox.clear();


    // On rétablit la possibilité d'édition des champs
    m_login->setEnabled(true);
    m_password->setEnabled(true);
    boutonConnexion->setEnabled(true);
    boutonSavParametres->setEnabled(true);

    // On avait bloqué temporairement la capacité de m_login et m_pasword à envoyer des signaux editingFinished
    // => evite de multiples récupérations d'icones lors de l'édition
    // => évite le plantage de l'app
    // On débloque les signaux à la fin de la récupération des hébergeurs
    m_login->blockSignals(false);
    m_password->blockSignals(false);

    qDebug() << "RecupHebergeurs :: Fin";
}

void MainWindow::receptionRecupHebergeursEtat(int etatRecupHebergeurs)
{
    if (etatRecupHebergeurs != 1) // Autre cas que Réussite
        QMessageBox::information(this, tr("Erreur"), tr("Echec de récupération des hébergeurs !\nVeuillez relancer le logiciel."));
}

void MainWindow::receptionRecupHebergeursHebergeurs(QString hebergeur, QString hebergeurTexte, bool etat_selection, QString id)
{
    // Fonction assurant la disposition des hébergeurs sur l'interface et le mappage des signaux
    // etat_selection = 1 => cochée
    // id sert à mapper les icones téléchargées en parallèle et reçues dans receptionRecupHebergeursIcones()

    //qDebug() << "RecupHebergeurs :: Reception de l'hebergeur...";

    // Mise en mémoire de l'hébergeur dans la liste des hébergeurs prêts à être utilisés
    // ssi son état de sélection est à true

    if (etat_selection)
        m_hebergeursListe.append(hebergeur);

    // Mise en place des hebergeurs sur l'interface
    // Création d'autant de checkbox que d'hébergeurs récupérés

    //qDebug() << "nouvel id chk:" << id;

    // Mise en place de la checkbox
    QCheckBox *checkbox = new QCheckBox(hebergeurTexte);
    checkbox->setChecked(etat_selection);

    // Ajout de la checkbox à la liste des checkbox
    m_listCheckBoxHebergeurs.insert(checkbox, id);

    // On prépare les checkbox dans un ordre harmonieux (les déjà cochées en premier)
    // => affichage final dans finThreadRecupHebergeurs()
    // Contrairement à un QVector:
    //prepend(): This operation is usually very fast (constant time),
    //because QList preallocates extra space on both sides of its
    //internal buffer to allow for fast growth at both ends of the list.
    (etat_selection) ? m_sortedListCheckBox.prepend(checkbox) : m_sortedListCheckBox.append(checkbox);

    // Mappage du signal
    m_signalMapper->setMapping(checkbox, hebergeur);
    connect(checkbox, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
}

void MainWindow::receptionRecupHebergeursIcones(QByteArray icone, QString id)
{
    // id sert à mapper les checkbox reçues dans receptionRecupHebergeursHebergeurs()
    //qDebug() << "RecupHebergeurs :: Reception de l'icone et affichage...";

    //qDebug() << "nouvel id ico:" << id;
    m_listIconesHebergeurs.insert(icone, id);

    QHashIterator<QCheckBox *, QString> itCheckbox(m_listCheckBoxHebergeurs);
    while (itCheckbox.hasNext()) {
        itCheckbox.next();

        //itCheckbox.key(value) => recup la clé avec une valeur
        //itCheckbox.value(key) => recup la valeur avec une clé

        QHashIterator<QByteArray, QString> itIcone(m_listIconesHebergeurs);
        while(itIcone.hasNext()) {

            itIcone.next();
            if (itIcone.value() == itCheckbox.value()) {
                // Lecture du QByteArray
                QPixmap pixmap;
                if(pixmap.loadFromData(itIcone.key()))
                    itCheckbox.key()->setIcon(pixmap);
                else
                    qWarning() << "Mauvaise conversion QByteArray => QPixmap";

                // Retraits
                //qDebug() << "avant chk" << m_listCheckBoxHebergeurs.count() << "ico" << m_listIconesHebergeurs.count();
                m_listCheckBoxHebergeurs.remove(itCheckbox.key());
                m_listIconesHebergeurs.remove(itIcone.key());
                //qDebug() << "apres chk" << m_listCheckBoxHebergeurs.count() << "ico" << m_listIconesHebergeurs.count();
            }
        }
    }
}

void MainWindow::receptionMaxSelectionHebergeurs(int maxHosts)
{
    // Memorise le nombre max d'hébergeurs autorisés
    m_maxHosts = maxHosts;
}

bool MainWindow::regulationMaxSelectedHosts()
{
    // Prend en compte le nombre max d'hébergeurs autorisés et
    // retourne true si la sélection d'un nouvel hébergeur est autorisée, false sinon
    qDebug() << "total checkbox:" << gridLayoutHebergeurs->count();

    // http://qt-project.org/doc/qt-5/qobject.html#findChildren
    // http://stackoverflow.com/questions/4065378/qt-get-children-from-layout
    // The key point is that a layout can become a child of a widget (because they both inherit QObject),
    //but a widget cannot become a child of a layout.
    //A widget must have another widget as a parent, and QLayout does not inherit QWidget
    // En bref:
    // Possible mais récupère toutes les checkbox de l'appli:
    // QList<QCheckBox*> allCheckbox = parentWidget->findChildren<QCheckBox*>();
    // Pas possible:
    // QList<QCheckBox*> allCheckbox = gridLayoutHebergeurs->findChildren<QCheckBox*>();


    int nb_checked = 0;
    for (int i = 0; i < gridLayoutHebergeurs->count(); i++) {
        // Récupération du QLayoutItem dans le QGridLayout et transtypage en QCheckBox
        // widget() : If this item is a QWidget, it is returned as a QWidget; otherwise 0 is returned.
        QCheckBox *checkbox = qobject_cast<QCheckBox*>(gridLayoutHebergeurs->itemAt(i)->widget());

        if (checkbox) {
            qDebug() << checkbox->text() << "checked ?" << checkbox->isChecked();
            nb_checked = (checkbox->isChecked()) ? nb_checked + 1 : nb_checked;
        }
    }

    qDebug() << "nb checked:" << nb_checked;

    return (nb_checked > m_maxHosts) ? false : true;
}

void MainWindow::checkboxHebergeursClicked(QString hebergeur)
{
    qDebug() << "checkbox" << hebergeur << "cliquée !";

    // Moyen de récup l'objet à l'origine de l'envoi d'un signal
    // QSignalMapper* pCheckBox = qobject_cast<QSignalMapper*>(sender());
    // voir réimplémentation QSignalMapper ou fonction lambda en Qt5
    // http://stackoverflow.com/questions/13989297/how-to-keep-the-source-signals-parameters-while-using-qsignalmapper?rq=1

    qDebug() << "liste hébergeurs" << m_hebergeursListe.count() << m_hebergeursListe;
    if (!regulationMaxSelectedHosts()) {
        // Trop d'hébergeurs sélectionnés

        // Récupération de la checkbox à l'origine du signal
        QCheckBox *chk = qobject_cast<QCheckBox*>(m_signalMapper->mapping(hebergeur));
        if (chk) {
            // Message
            QMessageBox::information(
                        this,
                        tr("Trop d'hébergeurs !"),
                        tr("Vous avez dépassé la limite du nombre d'hébergeurs autorisés (") + QString::number(m_maxHosts) + ").\n\"" +
                        chk->text() + tr("\", ne sera pas conservé."));
            qDebug() << "Trop d'hébergeurs selectionnés" << chk->text();

            // Désélection
            chk->setChecked(false);

            // On s'arrete là
            return;
        }
    }

    // La sélection est autorisée
    if (!m_hebergeursListe.contains(hebergeur)) {
        // Pas trouvé => On ajoute l'hébergeur
        m_hebergeursListe.append(hebergeur);
        //QMessageBox::information(this, "Etat de la checkbox", hebergeur + " cochée\nContient : " + QString::number(m_hebergeursListe.count()));
    }
    else {
        // Trouvé => On retire l'hébergeur
        m_hebergeursListe.removeAll(hebergeur);
        //QMessageBox::information(this, "Etat de la checkbox", hebergeur + " décochée\nContient : " + QString::number(m_hebergeursListe.count()));
    }
}

void MainWindow::editionLogin()
{
    qDebug() << "Edition des logins en cours... (Déconnexion)";

    // Dans tous les cas: Edition des logins = Déconnexion du serveur
    labelNomUtilisateur->setText(tr("Upload Anonyme"));

    if (m_connecte) {
        // Statut changé en déconnecté
        m_connecte = false;

        // Vu que les logins sont changés, on RAZ la liste des hébergeurs et on passe en anonyme
        rechercheHebergeurs();
    }
}

void MainWindow::rechercheRetraitListe(const QString &hebergeur)
{
    // Recherche l'hébergeur supprimé via les checkBox dans la liste et l'enlève
    // Marche aussi, si par erreur, un hébergeur est présent plusieurs fois..

    m_hebergeursListe.removeAll(hebergeur);
}






void MainWindow::on_boutonConnexion_clicked()
{
    // Fonction pr se loguer, récupérer le cookie de connexion ssi le champ de login n'est pas vide,
    if (m_login->text().isEmpty() || m_password->text().isEmpty()) {
        QMessageBox::information(this, tr("Erreur login"), tr("Veuillez remplir tous les champs pour vous connecter !"));
        return;
    }

    // On évite l'édition des champs pendant la connexion...
    m_login->setEnabled(false);
    m_password->setEnabled(false);
    boutonConnexion->setEnabled(false);
    boutonSavParametres->setEnabled(false);

    qDebug() << "Connexion :: Tentative de connexion débutée...";
    qDebug() << "Connexion :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    // Création d'une nouvelle instance de thread
    QThread *thread = new QThread(this);

    // Création et envoi de l'objet dans le thread
    m_connexion = new Connexion(m_login->text(), m_password->text(), NULL);
    m_connexion->moveToThread(thread);


    connect(thread, SIGNAL(started()), m_connexion, SLOT(demarrage()));
    connect(m_connexion, SIGNAL(emissionLoginEtat(int)), this, SLOT(receptionLoginEtat(int)));
    connect(m_connexion, SIGNAL(emissionLoginId(QString)), this, SLOT(receptionLoginId(QString)));
    connect(m_connexion, SIGNAL(finished()), thread, SLOT(quit()));

    // Attention : rétablir la possibilité de cliquer à  nouveau sur le bouton de connexion
    //ssi on s'est assuré que l'objet connexion et son thread sont détruits !
    connect(m_connexion, SIGNAL(finished()), m_connexion, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(destroyed()), this, SLOT(finThreadConnexion()));

    // Démarrage du thread et donc de l'objet qui vit dedans
    thread->start();
}

void MainWindow::finThreadConnexion()
{
    // Rétablissement interface => déplacée dans finThreadRecupHebergeurs()
    // Une connexion entrainant systématiquement l'actualisation des hébergeurs.
    m_upCurl = NULL;

    qDebug() << "Connexion :: Fin";

    // Récupération des hébergeurs
    rechercheHebergeurs();
}

void MainWindow::receptionLoginEtat(int etatConnexion)
{
    switch(etatConnexion)
    {
        case 0:     QMessageBox::information(this, tr("Authentification échouée"), tr("Mauvais login ou mauvais mot de passe"));
                    labelNomUtilisateur->setText(tr("Upload Anonyme"));
                    m_connecte = false;
                    break;

        case 1:     labelNomUtilisateur->setText(tr("Upload en tant que ") + m_login->text());
                    m_connecte = true;
                    break;

        default:    QMessageBox::information(this, tr("Erreur de connexion"), tr("Vérifiez votre connexion internet ou la disponibilité du site..."));
                    labelNomUtilisateur->setText(tr("Upload Anonyme"));
                    m_connecte = false;
                    break;
    }
}

void MainWindow::receptionLoginId(QString id)
{
    // Reception de l'id
    qDebug() << "Connexion :: Réception de l'ID...";
    m_loginId = id;
}

void MainWindow::on_boutonUpload_clicked()
{
    // Si l'upload n'est pas en cours
    if (m_upEnCours == false)
    {
        if (m_cheminsFichiers.count() != 0)
        {
            m_upEnCours = true;
            m_row       = 0;
            m_sizeFaite = 0;

            // Actualiser l'interface
            interfaceUpdate();

            // Appel de la boucle d'upload
            preparationUpload();
        }
        // Aucun fichier dans le tableau => vide
        else
            QMessageBox::information(this, tr("Info"), tr("Veuillez ajouter des fichiers svp !"));

    }
    // Upload en cours => couper l'upload
    else
    {
        // Fonction d'arret, commune à  ce bouton et aux menus contextuels (annuler) du systray et du tableau
        // UpCurl va renvoyer un signal d'arret, son traitement va mener à  preparationUpload qui fait la différence entre m_upEnCours = true ou false
        //Si false: actualisation de l'interface

        m_upEnCours = false;

        boutonUpload->setEnabled(false);

        m_upCurl->arretCurl();
        //emit this->emissionDemandeArretCurl();

        // Actualiser l'interface => l'actualisation se fait maintenant dans preparationUpload();
    }
}

void MainWindow::interfaceUpdate()
{
    //Si l'up est en cours, les options sont grises/innaccessibles
    if (m_upEnCours == true) {
        m_login->setEnabled(false);
        m_password->setEnabled(false);
        boutonConnexion->setEnabled(false);
        // TODO: block boutonSavParametres ?
        groupBoxHebergeurs->setEnabled(false);
        boutonSuppressionTout->setEnabled(false);
        boutonSuppressionAnnules->setEnabled(false);
        boutonSuppressionTermines->setEnabled(false);

        // Modification du bouton d'upload
        boutonUpload->setText(tr("Arreter l'upload !"));
        boutonUpload->setIcon(QIcon(STOPED_IMG));

        //Activer l'option "Annuler" du systray
        actionSysTrayArreter->setEnabled(true);

        //Réinitialisation de la barre de progression
        progressBar->setValue(0);
        #ifdef WINDOWS
            m_taskbarProgress->setVisible(true);
            m_taskbarProgress->setValue(0);
        #endif
    }
    else {
        m_login->setEnabled(true);
        m_password->setEnabled(true);
        boutonConnexion->setEnabled(true);
        // TODO: block boutonSavParametres ?
        groupBoxHebergeurs->setEnabled(true);
        boutonSuppressionTout->setEnabled(true);
        boutonSuppressionAnnules->setEnabled(true);
        boutonSuppressionTermines->setEnabled(true);

        // Modification du bouton d'upload
        boutonUpload->setText(tr("Lancer l'upload !"));
        boutonUpload->setIcon(QIcon(UPLOADING_IMG));
        // On s'assure que le bouton est réactivé.
        //Si SelectionServeur a renvoyé une erreur, le bouton n'est pas réactivé puisque cela se fait dans uploadCurl()
        boutonUpload->setEnabled(true);

        //Désactiver l'option "Annuler" du systray
        actionSysTrayArreter->setEnabled(false);

        //Réinitialisation de la barre de progression
        progressBar->setValue(100);
        #ifdef WINDOWS
            m_taskbarProgress->setValue(100);
            m_taskbarProgress->setVisible(false);
        #endif
    }
}

void MainWindow::preparationUpload()
{
    // Parcourt le tableau et détecte l'état des fichiers présents
    // Détermine si l'upload est fini
    // Détermine l'extinction de l'ordinateur
    if (m_upEnCours == true)
    {
        qDebug() << "parcourt de la grille en cours...";
        if (m_row < m_cheminsFichiers.count())
        {
            // l'upload peut se faire
            // on up uniquement si c'est READY
            if (m_etatsFichiers.at(m_row) == READY)
            {
                remplissageTableau(m_row, 1, PREPROCESSING_TEXT());
                tableWidget->item(m_row, 1)->setIcon(QIcon(PREPROCESSING_IMG));
                m_etatsFichiers[m_row] = PREPROCESSING;

                // Sélection du serveur pour l'upload de ce fichier.
                // Enchainement avec l'up lui même
               selectionServeur();

            }
            // l'upload est STOPED, REMOVED, FAILED_e, FINISHED, PREPROCESSING, UPLOADING
            else {
                qDebug() << "Fichier n'ayant pas le bon statut : fichier suivant";

                // On passe au suivant
                m_row++;
                preparationUpload();
            }
        }
        else
        // le tableau est parcouru il n'y en a plus rien à  effectuer, "Uploads terminés !" (m_row est au max et le tableau a été parcouru)
        {
            qDebug() << "Envois terminés";

            m_upEnCours = false;

            // Actualiser l'interface
            interfaceUpdate();

            // Réinitialisation du systray
            m_sticon->setToolTip(tr("MultiUp MaNaGeR : au repos"));

            // Eteindre l'ordinateur
            if (m_extinction == true)
            {
                //Méthode pour Windows :
                #ifdef WINDOWS
                    //QProcess *extinction = new QProcess(this);
                    //extinction->start("shutdown /s /t 15 /f /c \"Fin d'upload; Extinction programmée de l'ordinateur...\"", QIODevice::ReadOnly);

                    QStringList arguments;
                    arguments << "/s" << "/t" << "15" << "/f" << "/c" << tr("\"Fin d'upload; Extinction programmée de l'ordinateur...\"");
                    QProcess::execute("shutdown", arguments);
                #endif
                //Méthode pour Linux :
                #ifdef LINUX
                    QStringList arguments;
                    arguments << "-h" << "+1" << tr("\"Fin d'upload; Extinction programmée de l'ordinateur...\""); //+ 1 minute
                    QProcess::execute("shutdown", arguments);
                #endif
            }

        }
    }
    else {
        qDebug() << "Envois arretés";

        boutonUpload->setEnabled(true);

        //Actualisation de l'interface lorsque l'upload a été coupé volontairement.
        interfaceUpdate();
    }
}

void MainWindow::selectionServeur()
{
    // Va chercher le serveur d'upload le moins surchargé
    // Désactive le bouton d'arret pendant la recherche du serveur sinon ça fout la merde...
    boutonUpload->setDisabled(true);

    qDebug() << "SelectionServeur :: Tentative de récupération du serveur débutée...";
    qDebug() << "SelectionServeur :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    // Création d'une nouvelle instance de thread
    QThread *thread = new QThread(this);

    // Création et envoi de l'objet dans le thread
    m_selectionServeur = new SelectionServeur(m_tailleFichiers.at(m_row));
    m_selectionServeur->moveToThread(thread);

    // PS : plus de passage de cookie dans la v2
    connect(thread, SIGNAL(started()), m_selectionServeur, SLOT(demarrage()));

    connect(m_selectionServeur, SIGNAL(emissionAdresseIp(QString)), this, SLOT(receptionAdresseIpServeur(QString)));
    connect(m_selectionServeur, SIGNAL(emissionEtatSelectionServeur(int)), this, SLOT(receptionSelectionServeurEtat(int)));
    connect(m_selectionServeur, SIGNAL(finished()), thread, SLOT(quit()));

    // Attention : rétablir la possibilité de cliquer à  nouveau sur le bouton de connexion ssi on s'est assuré que l'objet connexion et son thread sont détruits !
    connect(m_selectionServeur, SIGNAL(finished()), m_selectionServeur, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(destroyed()), this, SLOT(finThreadSelectionServeur()));

    // Démarrage du thread et donc de l'objet qui vit dedans
    thread->start();
}

void MainWindow::receptionAdresseIpServeur(QString adresseIp)
{
    qDebug() << "SelectionServeur :: Réception adresse IP serveur...";
    m_adresseIpServeur = adresseIp;
}

void MainWindow::receptionSelectionServeurEtat(int etatSelectionServeur)
{
    // Si tout s'est bien passé on poursuit l'up par la fonction preUpload(),
    // Si il y a eu une erreur dans la selection du serveur c'est inutile de commencer un up.
    // Les raisons de cette erreur peuvent être: Modification de la page de multiup (0) ou connexion inexistante (2)

    // Ça permet de gagner du temps sur une tentative d'up inutile,
    // et de diffrencier une perte de connexion d'une erreur alatoire lors de l'up dans le rapport d'erreur;
    // Permet aussi de ne pas tomber dans 1 boucle infinie si la fonction de reprise sur erreur est active...

    // On inscrit cette erreur dans la colonne de lien et dans le log automatique,
    // comme dans le slot recuperationLien(QString lien) lorsqu'il reçoit une erreur en provenance du thread UpCurl

    switch(etatSelectionServeur)
    {
        case 0:     m_liensFichiers[m_row] = tr("Erreur sur multiup.org...");
                    break;

        case 1:     // Poursuite de la procédure de préparation des fichiers.
                    // Place une ProgressBar en face du fichier en train de s'uploader
                    if (tableWidget->item(m_row, 2)) // effacement de l'item déjà  présent
                        tableWidget->item(m_row, 2)->~QTableWidgetItem();

                    // J'accroche la barre au tableau (ptetre mieux et plus propre quand on appele la fonction tablewidget.clear)
                    m_item = new QProgressBar(tableWidget);
                    tableWidget->setCellWidget(m_row, 2, m_item);
                    m_item->setRange(0, 100);

                    // Change le statut du fichier traité
                    remplissageTableau(m_row, 1, UPLOADING_TEXT());
                    tableWidget->item(m_row, 1)->setIcon(QIcon(UPLOADING_IMG));
                    m_etatsFichiers[m_row] = UPLOADING;
                    return;
                    break;

        default:    m_liensFichiers[m_row] = tr("Erreur de connexion au site...");
                    break;

    }

    // Si on est pas sorti du switch cela veut dire qu'il y a eu une erreur ou un pb sur le site:
    // Update des icones d'état
    remplissageTableau(m_row, 1, FAILED_TEXT());
    tableWidget->item(m_row, 1)->setIcon(QIcon(FAILED_IMG));
    m_etatsFichiers[m_row] = FAILED_e;

    // Update de l'affichage de l'erreur dans l'interface et dans la fenetre des liens
    remplissageTableau(m_row, 5, m_liensFichiers.at(m_row));
    m_liens += m_liensFichiers.at(m_row) + "\n";

    // Ajoute l'erreur au fichier de sauvegarde
    #ifdef WINDOWS
        enregistrement(QApplication::applicationDirPath(), m_liensFichiers.at(m_row));
    #endif
    #ifdef LINUX
        enregistrement(QDir::homePath() + "/" + APP_DIR, m_liensFichiers.at(m_row));
    #endif

}

void MainWindow::finThreadSelectionServeur()
{
    qDebug() << "SelectionServeur :: Rétablissement interface après obtention du serveur";

    m_selectionServeur = NULL;

    if (m_etatsFichiers.at(m_row) == UPLOADING)
    {
        qDebug() << "=> Poursuite upload";

        // Début de l'upload
        uploadCurl();
    }
    else {
        // On fait comme si l'upload avait pris fin,
        // comme dans le slot uploadFin()
        m_row++;

        // On passe au fichier suivant...
        preparationUpload();
    }
    qDebug() << "SelectionServeur :: Fin";
}

void MainWindow::uploadCurl()
{
    // Démarrage du chronomètre
    // Démarrage du thread avec curl

    m_chronometre.start();
    m_timer->start();

    // Ajout de la taille du fichier en cours pour calculer la progression globale
    m_sizeFaite += m_tailleFichiers.at(m_row);

    qDebug() << "Curl :: Tentative d'upload' débutée...";
    qDebug() << "Curl :: Nous sommes dans :" << Q_FUNC_INFO << QThread::currentThreadId();

    // Création d'une nouvelle instance de thread
    QThread *thread = new QThread(this);

    // Création et envoi de l'objet dans le thread
    std::string curlFileName = m_cheminsFichiers.at(m_row).toStdString();

    // Url V2 : http://SERVER.multiup.org/upload/index.php
    qDebug() << "URL!!!" << m_adresseIpServeur;
    m_adresseIpServeur.replace(QString("https"), QString("http"));
    qDebug() << "URL apres!!!" << m_adresseIpServeur;
    std::string curlUrl = m_adresseIpServeur.toStdString();

    std::string id = m_loginId.toStdString();

    if(m_connecte)
        m_upCurl = new UpCurl(curlFileName, curlUrl, m_hebergeursListe, id, NULL);
    else
        m_upCurl = new UpCurl(curlFileName, curlUrl, m_hebergeursListe, NULL);

    m_upCurl->moveToThread(thread);


    connect(thread, SIGNAL(started()), m_upCurl, SLOT(demarrage()));

    connect(m_upCurl, SIGNAL(emissionLienEtStats(QString,double,double)), this, SLOT(receptionUpCurlLienEtStats(QString,double,double)));
    connect(m_upCurl, SIGNAL(emissionEtatUpCurl(int)), this, SLOT(receptionUpCurlEtat(int)));
    connect(m_upCurl, SIGNAL(emissionProgression(double,double)), this, SLOT(receptionUpCurlProgression(double,double)));
    // /!\ Les signaux ne marchent pas: (pas émis et pas reçus..
    // le seul moyen a priori est de se passer des signaux en appelant directement la méthode arretCurl...
    //connect(this, SIGNAL(emissionDemandeArretCurl()), m_upCurl, SLOT(arretCurl()));
    //connect(this, SIGNAL(emissionDemandeArretCurl()), m_upCurl, SLOT(test));

    connect(m_upCurl, SIGNAL(finished()), thread, SLOT(quit()));
    // WARNING : rétablir la possibilité de cliquer à  nouveau sur le bouton de connexion
    // ssi on s'est assuré que l'objet connexion et son thread sont détruits !
    connect(m_upCurl, SIGNAL(finished()), m_upCurl, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(destroyed()), this, SLOT(finThreadUpCurl()));

    // Démarrage du thread et donc de l'objet qui vit dedans
    thread->start();

    // WARNING : On réactive le bouton d'upload
    // J'estime que l'objet et les signaux sont émis quasi instantanément.
    // Donc peu de risque de pouvoir arreter un objet qui n'a pas encore démarré
    boutonUpload->setEnabled(true);
}

void MainWindow::emissionProgressionSlot()
{
    // /!\ le timer censé envoyer la progression régulièrement est placé dans le constructeur de MainWindow..
    // J'ai des pbs pour rendre bavard l'objet abritant l'upload.
    // le thread est freezé lors de l'up.
    // normalement l'implémentation des threads est meilleure qu'avant,
    // sauf qu'avant la réimplémentation d'une classe QThread permettait l'émission et la réception de signaux..

    // En attendant, je profite du fait que static struct Bricolage bricolo soit globale..
    // et je simule l'émission du signal ici.
    // receptionUpCurlProgression() ne change donc pas.

    // le démarrage de l'appel de ce slot se fait dans : uploadCurl()
    // l'arret de l'appel de ce slot se fait dans : receptionUpCurlEtat()

    receptionUpCurlProgression(bricolo.upTotal, bricolo.upFait);
}

void MainWindow::receptionUpCurlProgression(double TotalToUpload, double NowUploaded)
{

    // 0 valeur par def si le serveur n'envoie pas la valeur totale de la taille du fichier
    if (NowUploaded != 0 && TotalToUpload != 0)
    {
        //---------Gestion du temps
        // Temps écoulé depuis le début de l'up( heure de début mémorisée par m_chronometre.start()
        double tempsEcoule = m_chronometre.elapsed();

        static double NowUploadedTemp;
        QString vitesseInstantanee = QString::number(((NowUploaded - NowUploadedTemp) / 1000), 'f', 1);
        NowUploadedTemp = NowUploaded;

        QString vitesseMoyenne = QString::number(NowUploaded / tempsEcoule, 'f', 1);

        // Estimation du temps restant en sec ( division par 1000 du résultat en ms):
        qint64 tempsEstime = conversion(((m_sizeTot - (m_sizeFaite - TotalToUpload + NowUploaded)) / (NowUploaded / tempsEcoule)) / 1000);

        // Estimation du temps restant en heures/minutes/secondes
        QString heureEstimee = QString::number(tempsEstime / 3600) + "h" +
                QString::number((tempsEstime % 3600) / 60) + "m" +
                QString::number((tempsEstime % 3600) % 60) + "s";


        //---------Progressions
        //Convertir le résultat du calcul en int pour la barre de progression de l'item
        int progressionFichier  = conversion((NowUploaded / TotalToUpload)*100);
        int progressionTotale   = conversion(((m_sizeFaite - TotalToUpload + NowUploaded)*100) / m_sizeTot);


        //---------Interface
        // Barre de progression de l'item
        m_item->setValue(progressionFichier);

        // Barre de progression générale
        progressBar->setValue(progressionTotale);
        #ifdef WINDOWS
            m_taskbarProgress->setValue(progressionTotale);
        #endif

        // Actualisation des colonnes "Taille" et "Vitesse"
        remplissageTableau(m_row, 3, QString::number(NowUploaded / (1024 * 1024), 'f', 1) + "/" + QString::number(TotalToUpload / (1024 * 1024), 'f', 1));
        remplissageTableau(m_row, 4, vitesseInstantanee + " Kio/s");

        // Actualisation du label et de l'icone systray indiquant la vitesse:
        labelTempsEstime->setText(heureEstimee);

        m_sticon->setToolTip("MultiUp MaNaGeR\n" +
                            tr("Temps restant : ") + heureEstimee + "\n" +
                            tr("Fichier en cours : ") + QString::number(progressionFichier) + "%\n" +
                            tr("Total : ") + QString::number(progressionTotale) + "%\n" +
                            tr("Vitesse moyenne : ") + vitesseMoyenne + " Kio/s");


        if ((m_regVitesse) && (m_upCurl != NULL))
        {
            // Je fais une estimation virtuelle de la qtté de données envoyées pendant 4 sec à partir de la vitesse instantanée
            // 4 sec car, impossible de faire des coupures sur intervalle plus petit...
            // Du coup grosso modo, on peut diviser la vitesse par 25,50,75% pas plus ni moins.

            // Produit en croix à partir de la vitesseMoyenne:
            // qttéMoyenne => 4000 msec
            // qttéVoulue  => x
            // x = qttéVoulue * 4000 / qttéMoyenne
            //
            // delai d'attente = 4000 - x

            //PS: horizontalSliderRegVitesse n'est jamais mis à 0

            //int delay(4000 - ((horizontalSliderRegVitesse->value()*16000) / conversion(vitesseInstantanee.toDouble()*4 + 1)));

            int delay(0);
            switch(horizontalSliderRegVitesse->value())
            {
                case 1: delay = 4000;
                        break;

                case 2: delay = 3000;
                        break;

                case 3: delay = 2000;
                        break;

                case 4: delay = 1000;
                        break;

                default: break;
            }

            qDebug() << "Délai attente :" << delay;

            static int i(0);
            i++;

            // > à 250 ,< à 750 => ridicule de pauser pendant 40 ms ou de laisser pendant 40 ms à mon avis... à revoir ?
            if ((delay >= 1000) && (i >= 4)) {
                m_upCurl->pause();
                i = 0;
                // Cette bouse ne marche pas: tj impossible d'utiliser les signaux avec l'objet m_upCurl..
                //QTimer::singleShot(delay, m_upCurl, SLOT(unpause()));
                QTimer::singleShot(delay, this, SLOT(upCurlUnpause()));
            }
        }
    }
}

void MainWindow::upCurlUnpause()
{
    if ((m_upEnCours) && (m_upCurl != NULL)) {
        m_upCurl->unpause();
    }
}

void MainWindow::receptionUpCurlEtat(int etatUpCurl)
{
    qDebug() << "Curl :: Récupération etat Curl";

    // Arret de l'appel de la lecture de l'avancement de l'up
    m_timer->stop();

    switch(etatUpCurl)
    {
        case 0:     remplissageTableau(m_row, 1, STOPED_TEXT()); // Etat du fichier (colonne 2)
                    tableWidget->item(m_row, 1)->setIcon(QIcon(STOPED_IMG));
                    m_etatsFichiers[m_row] = STOPED;
                    remplissageTableau(m_row, 5, STOPED_TEXT()); // Lien (colonne 5)
                    m_liensFichiers[m_row] = STOPED_TEXT();
                    break;

        case 1:     remplissageTableau(m_row, 1, FINISHED_TEXT());
                    tableWidget->item(m_row, 1)->setIcon(QIcon(FINISHED_IMG));
                    m_etatsFichiers[m_row] = FINISHED;
                    m_item->setValue(100); // On force l'affichage à 100% dans la barre
                    break;

        default:    remplissageTableau(m_row, 1, FAILED_TEXT()); // Etat du fichier (colonne 2)
                    tableWidget->item(m_row, 1)->setIcon(QIcon(FAILED_IMG));
                    m_etatsFichiers[m_row] = FAILED_e;
                    remplissageTableau(m_row, 5, FAILED_TEXT()); // Lien (colonne 5)
                    m_liensFichiers[m_row] = FAILED_TEXT();
                    break;
    }
}

void MainWindow::receptionUpCurlLienEtStats(QString lien, double averageSpeed, double dureeTransfert)
{
    qDebug() << "Curl :: Récupération du lien :" << lien;

    UNUSED(dureeTransfert);

    // Update de l'affichage du lien dans l'interface et dans la grille
    m_liensFichiers[m_row] = lien;
    remplissageTableau(m_row, 5, lien);

    // La vitesse obtenue est en bytes / sec => conversion en Kio/sec
    QString vitesseMoyenne = QString::number(averageSpeed / 1024, 'f', 1);
    remplissageTableau(m_row, 4, "~" + vitesseMoyenne + " Kio/s");

    // Utilisation future de la durée...
    //ICI

    // Si fonction d'aide à la présentation des up cochée et que le fichier n'a pas eu d'erreur (forcément vu que on a un lien..)
    if(m_prez) {
        qDebug() << "Curl :: Recherche du fichier :" << m_cheminsFichiers.at(m_row) << ".txt";

        searchPrezFile(QString(m_cheminsFichiers.at(m_row) + ".txt"), lien);
    }
}

void MainWindow::finThreadUpCurl()
{
    qDebug() << "Curl :: Rétablissement interface après upload";

    m_upCurl = NULL;

    // Ajoute le résultat dans la fenêtre des liens
    m_liens += m_liensFichiers.at(m_row) + "\n";

    // Ajoute le lien au fichier de sauvegarde
    QString resultat = m_liensFichiers.at(m_row);

    //  Si la fonction de reprise sur erreur est active et que le fichier a eu une erreur, on remet le fichier dans la file
    if ((m_reprise == true) && (m_etatsFichiers.at(m_row) == FAILED_e)) {
        qDebug() << "Curl :: Fichier remis en file :" << m_cheminsFichiers.at(m_row);
        creationTableau(AJOUT, QStringList(m_cheminsFichiers.at(m_row)));
    }

    //  Si fonction d'effacement des fichiers est cochée et que le fichier n'a pas eu d'erreur.
    if((m_effacement == true) && (m_etatsFichiers.at(m_row) == FINISHED)) {
        qDebug() << "Curl :: Tentative d'effacement du fichier :" << m_cheminsFichiers.at(m_row);

        QFile file(m_cheminsFichiers.at(m_row));
        resultat += "\n";

        if (file.remove())
            resultat += tr("Effacement réussi");
        else
            resultat += tr("Effacement non fait");

        // Ajoute le résultat au fichier de sauvegarde
        // => après.. (économie d'une ouverture du fichier)

    }

     // Ajoute le résultat (lien + effacement) au fichier de sauvegarde
    #ifdef WINDOWS
        enregistrement(QApplication::applicationDirPath(), resultat);
    #endif
    #ifdef LINUX
        enregistrement(QDir::homePath() + "/" + APP_DIR, resultat);
    #endif


    m_row++;

    preparationUpload();

    qDebug() << "Curl :: Fin";
}
