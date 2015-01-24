#include "amainwindow.h"
#include "ui_amainwindow.h"
#include "src/application.h"

AMainWindow::AMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AMainWindow)
{
    ui->setupUi(this);

    //Fonction qui initialise les dock et qui réalise la liste des langues (QAction)
    fillLanguages();

    MenuVisbilite = ui->menuBar->addMenu(tr("Fenêtre"));
    ActionComm = MenuVisbilite->addAction(tr("Communication USB/Serial"));
    ActionComm->setCheckable(true);
    ActionComm->setIcon(QIcon(":Comm"));
// ALEXIS -> Ajouter ActionComm à la barre d'outil
    connect(ActionComm, SIGNAL(triggered()), SLOT(AfficherDockComm()));
    ActionHorodateur = MenuVisbilite->addAction(tr("Horodateur"));
    ActionHorodateur->setCheckable(true);
    ActionHorodateur->setIcon(QIcon(":CHoraire"));
// ALEXIS -> Ajouter ActionHorodateur à la barre d'outil
    connect(ActionHorodateur, SIGNAL(triggered()), SLOT(AfficherDockHorodateur()));

    //Dock pour la communication
    DockArduino = new ComArduino(tr("Communication Arduino"), this);
    AfficherDockComm();
    addDockWidget(Qt::BottomDockWidgetArea, DockArduino);
    //Dock pour l'enregistrement
    DockSave = new SaveXmlFile(tr("Liste des modes"), this);
    addDockWidget(Qt::LeftDockWidgetArea, DockSave);
    //Dock pour les condition horaire
    DockHorodateur = new Horodateur(tr("Condition horaire"), this);
    AfficherDockHorodateur();
    addDockWidget(Qt::LeftDockWidgetArea, DockHorodateur);
}

AMainWindow::~AMainWindow()
{
    delete ui;
    delete DockArduino;
    delete DockSave;
    delete DockHorodateur;
}

//Fonction qui initialise les dock et qui réalise la liste des langues (QAction)
void AMainWindow::fillLanguages()
{
    Menulanguages = ui->menuBar->addMenu(tr("Langues"));

    // make a group of language actions
    QActionGroup* actions = new QActionGroup(this);
    connect(actions, SIGNAL(triggered(QAction*)), this, SLOT(setLanguage(QAction*)));

    foreach (QString avail, Application::availableLanguages())
    {
        // figure out nice names for locales
        QLocale locale(avail);
        QString language = QLocale::languageToString(locale.language());
        QString country = QLocale::countryToString(locale.country());
        QString name = language + " (" + country + ")";

        // construct an action
        QAction *action = Menulanguages->addAction(name);
        action->setData(avail);
        action->setCheckable(true);
        actions->addAction(action);
    }

    //Signaux a connecter définitivement entre les docks
    connect(DockHorodateur, SIGNAL(SendCondHoraire(CondHoraire*, int)), DockSave, SLOT(addCondition(CondHoraire*, int)));
    connect(DockHorodateur, SIGNAL(DemandeSuppCondHoraire(int)), DockSave, SLOT(SupprimerCondHoraire(int)));
    connect(DockSave, SIGNAL(ChangeActiveMode()), SLOT(OnModeChange()));
    //Le dock horodateur demande de lire les infos contenu dans l'arbre xml du mode actif, soit en String soit en Structure
    connect(DockHorodateur, SIGNAL(DemandeCHstring(QPlainTextEdit*)), DockSave, SLOT(onDemandeCHToPlainText(QPlainTextEdit*)));
    connect(DockHorodateur, SIGNAL(DemandeCHstring(CondHoraire*, int)), DockSave, SLOT(onDemandeCHToPlainText(CondHoraire*, int)));
    connect(DockSave, SIGNAL(CompilationUpdated()), DockHorodateur, SLOT(onModeUpdate()));
    //Lors du lancement de la compilation
    connect(DockSave, SIGNAL(CompilationStart(bool)), SLOT(CompilationStarted(bool)));
}

//Fonction qui lance le changement de langue
void AMainWindow::setLanguage(QAction* action)
{
    Application::setLanguage(action->data().toString());
}

//Surchage de ChangeEvent(QEvent *event)
void AMainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        // apply the new language to the MainWindows
        retranslate();
        // all designed forms have retranslateUi() method
        ui->retranslateUi(this);
        //call translate function of all child
        if (DockHorodateur)
            DockHorodateur->retranslate();
        //Le dock arduino nécéssite de connaitre la langue utilisée (pour les données xml)
        if (DockArduino)
            DockArduino->retranslate(Application::currentLanguage());
        if (DockSave)
            DockSave->retranslate(Application::currentLanguage());
    }
    QMainWindow::changeEvent(event);
}

//Fonction qui applique et réapplique les labels pour introduire leur traduction quand c'est nécessaire
void AMainWindow::retranslate()
{
    Menulanguages->setTitle(tr("Language"));
    MenuVisbilite->setTitle(tr("Fenêtre"));
    DockHorodateur->setWindowTitle(tr("Condition horaire"));
    DockArduino->setWindowTitle(tr("Communication Arduino"));
    DockSave->setWindowTitle(tr("Liste des modes"));
}

//Slot qui fait toute les actions de mise a jours suite à un changement de mode demandé par l'utilisateur
void AMainWindow::OnModeChange()
{
    //On reset l'affichage du dock condition horaire
    DockHorodateur->AffichageOnChangeMode();
}

//Slot pour affiche ou cacher le dock Horodateur
void AMainWindow::AfficherDockHorodateur()
{
    if (ActionHorodateur->isChecked() && DockSave->onEditionRequested()) {
        DockHorodateur->setVisible(true);
    }
    else {
        DockHorodateur->setVisible(false);
        ActionHorodateur->setChecked(false);
    }
}

//Slot pour afficher ou cacher le dock Communication
void AMainWindow::AfficherDockComm()
{
    if (!ActionComm->isChecked() && !DockArduino->isConnected()) {
        DockArduino->setVisible(false);
    }
    else {
        DockArduino->setVisible(true);
        ActionComm->setChecked(true);
    }
}

//Slot lors du lancement ou fin de la compilation
void AMainWindow::CompilationStarted(bool Start)
{
    if (Start) {
        ActionHorodateur->setChecked(false);
        AfficherDockHorodateur();
// ALEXIS -> désactive ou cache le widget d'edition des effets lumineux
    }
    else {
// ALEXIS -> activer ou afficher le widget d'edition des effets lumineux
    }
}

//Slot lors de la connection d'un module Arduino
void AMainWindow::onArduinoConnected()
{
    ActionComm->setChecked(true);
    AfficherDockComm();
}
