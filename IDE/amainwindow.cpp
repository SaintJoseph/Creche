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
    languages = ui->menuBar->addMenu(tr("Langues"));

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
        QAction* action = languages->addAction(name);
        action->setData(avail);
        action->setCheckable(true);
        actions->addAction(action);
    }

    //Dock pour la communication
    DockArduino = new ComArduino(tr("Communication Arduino"), this);
    //Dock pour l'enregistrement
    DockSave = new SaveXmlFile(tr("Liste des modes"), this);
    //Dock pour les condition horaire
    DockHorodateur = new Horodateur(tr("Condition horaire"), this);

    //Signaux a connecter définitivement entre les docks
    connect(DockHorodateur, SIGNAL(SendCondHoraire(CondHoraire*, int)), DockSave, SLOT(addCondition(CondHoraire*, int)));
    connect(DockHorodateur, SIGNAL(DemandeSuppCondHoraire(int)), DockSave, SLOT(SupprimerCondHoraire(int)));
    connect(DockSave, SIGNAL(ChangeActiveMode()), SLOT(OnModeChange()));
    //Le dock horodateur demande de lire les infos contenu dans l'arbre xml du mode actif, soit en String soit en Structure
    connect(DockHorodateur, SIGNAL(DemandeCHstring(QPlainTextEdit*)), DockSave, SLOT(onDemandeCHToPlainText(QPlainTextEdit*)));
    connect(DockHorodateur, SIGNAL(DemandeCHstring(CondHoraire*, int)), DockSave, SLOT(onDemandeCHToPlainText(CondHoraire*, int)));
    connect(DockSave, SIGNAL(CompilationUpdated()), DockHorodateur, SLOT(onModeUpdate()));
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
    languages->setTitle(tr("Language"));
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
