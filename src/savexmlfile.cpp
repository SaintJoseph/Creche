#include "savexmlfile.h"

SaveXmlFile::SaveXmlFile(const QString & title, QWidget *parent , Qt::WindowFlags flags) :
    QDockWidget(title, parent, flags)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Aplication d'un style au dock
    setStyleSheet("QDockWidget::title {text-align: centre; background: rgb(0,189,138); color: rgb(0,0,138); border-style: inset; border-width: 2px; border-radius: 2px; border-color: rgb(0,0,138); font: 8pt; padding: 0px; height: 20px} QDockWidget::float-button { border: 1px solid; background: rgb(0,189,138); padding: 0px} QDockWidget::float-button:hover { background: rgb(0,0,138) } QDockWidget {border-style: outset; border-width: 2px; border-radius: 4px; border-color: rgb(0,0,138)}");
    setFeatures(QDockWidget::NoDockWidgetFeatures);
    setFeatures(QDockWidget::DockWidgetMovable);
    setFeatures(QDockWidget::DockWidgetFloatable);

    QFont font("Verdana", 9);
    //Layout principale du dock qui accueil les widget compilation
    MainLayout = new QVBoxLayout;
    MainLayout->setContentsMargins(1,1,1,1);
    MainLayout->setSpacing(1);
    MainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    //Layout de base du dock
    LayoutBase = new QVBoxLayout;
    LayoutBase->setContentsMargins(1,1,1,1);
    LayoutBase->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    //Scroll Widget
    ScrollArea = new QScrollArea;
    ScrollArea->setContentsMargins(0,0,0,0);
    ScrollArea->setBackgroundRole(QPalette::Dark);
    ScrollArea->setWidgetResizable(true);
    ScrollArea->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //Boutton Nouveau mode
    ButtonNewMode = new QPushButton(tr("New Mode"));
    ButtonNewMode->setFont(font);
    ButtonNewMode->setStyleSheet("QPushButton { width: 45px}");
    ButtonNewMode->setFixedHeight(18);
    //Bouton supression d'un mode
    ButtonDelete = new QPushButton(tr("Enreg. et Fermer", "Espace limité pour écrire \"Enregistrer et Fermer\" en entier."));
    ButtonDelete->setFont(font);
    ButtonDelete->setStyleSheet("QPushButton { width: 45px}");
    ButtonDelete->setFixedHeight(18);
    //Label pour la liste des instance de compilation
    LabelListeMode = new QLabel(tr("Liste Modes (max 5)", "Espace limiter pour faire une phrase entière."));
    LabelListeMode->setFont(font);
    LabelListeMode->setFixedHeight(18);
    //Widget de base pour accuellir le layout principale
    Base = new QWidget;
    //Widget de taille fixe pour accuellir le main layout dans le srollArea
    WidgetScrollArea = new QWidget;
    WidgetScrollArea->setFixedHeight(10);

    //Ajout des bouttons au layout principale
    LayoutBase->addWidget(ButtonNewMode);
    LayoutBase->addWidget(ButtonDelete);
    //Ajout du label liste
    LayoutBase->addWidget(LabelListeMode);
    //Application du main layout sur le widget main
    LayoutBase->addWidget(ScrollArea);
    //Application du layout au widget
    Base->setLayout(LayoutBase);
    //Application du layout main au Widget scrollArea
    WidgetScrollArea->setLayout(MainLayout);
    //Application du widget au scrollArea
    ScrollArea->setWidget(WidgetScrollArea);
    //Application du widget au dock
    setWidget(Base);
    //Application du font au dock
    setFont(font);
    //Restriction dimensionnelles
    setMinimumWidth(100);
    setMaximumWidth(150);

    //Lors de l'appuis sur le boutton on fait un nouveau mode
    connect(ButtonNewMode, SIGNAL(clicked()), SLOT(NewMode()));
    //connect temporaire pour test
    connect(ButtonDelete, SIGNAL(clicked()), SLOT(DeleteMode()));

    //Si une instance Compilation existe déjà on la récupère
    Flag1erInstanceNotInitialised = false;
    if (Compilation::NbInstance() != 0)
    {
        ListeModeOuvertPoint[0] =  Compilation::ReturnInstance(0);
        ListeModeOuvert[0] = ListeModeOuvertPoint[0]->InstanceValue();
        MainLayout->addWidget(ListeModeOuvertPoint[0]);
        int taille = WidgetScrollArea->height();
        WidgetScrollArea->setFixedHeight(taille + 45);
        ActiveModeNum = ListeModeOuvert[0];
        ChangeModeActif(ActiveModeNum);
        Flag1erInstanceNotInitialised = true;
        //On connect le signal avec l'instance
        connect(ListeModeOuvertPoint[0], SIGNAL(MouseClickEvent(int)), SLOT(ChangeModeActif(int)));
        connect(ListeModeOuvertPoint[0], SIGNAL(DeleteMe(Compilation*)), SLOT(SupprimerUneInstance(Compilation*)));
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
    affichageqDebug("Fin constructeur");
#endif /* DEBUG_COMANDSAVE */
}

SaveXmlFile::~SaveXmlFile()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << "/" << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Initialisation des variables statics
int SaveXmlFile::ActiveModeNum = 0;
int SaveXmlFile::ListeModeOuvert[] = {-1,-1,-1,-1,-1};
Compilation *SaveXmlFile::ListeModeOuvertPoint[] = {NULL};

char SaveXmlFile::ActiveMode()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << "/" <<std::endl;
#endif /* DEBUG_COMANDSAVE */
    return ActiveModeNum;
}

void SaveXmlFile::NewMode()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
    affichageqDebug("New Mode debut");
#endif /* DEBUG_COMANDSAVE */
    if (Flag1erInstanceNotInitialised)
    {
        ListeModeOuvertPoint[0]->showNewModeDialog();
        Flag1erInstanceNotInitialised = false;
    }
    else
    {
        if (Compilation::NbInstance() < 5)
        {
            //Création du nouveau mode
            Compilation *LeNouveauMode = new Compilation;

            int Num = 0;
            for (int i = 0; i < 5; i++)
            {
                if (ListeModeOuvert[i] == -1)
                {
                    Num = i;
                    break;
                }
            }
            //On prend en compte le numero d'instance
            ListeModeOuvert[Num] = LeNouveauMode->InstanceValue();
            ListeModeOuvertPoint[Num] = LeNouveauMode;
            //Initialisation du nouveaux mode
            LeNouveauMode->showNewModeDialog();
            //Mise a jour du dock
            MainLayout->addWidget(LeNouveauMode);
            int taille = WidgetScrollArea->height();
            WidgetScrollArea->setFixedHeight(taille + 45);
            //Activation du mode
            ChangeModeActif(ListeModeOuvert[Num]);
            //On connect les signaux avec l'instance
            connect(LeNouveauMode, SIGNAL(MouseClickEvent(int)), SLOT(ChangeModeActif(int)));
            connect(LeNouveauMode, SIGNAL(DeleteMe(Compilation*)), SLOT(SupprimerUneInstance(Compilation*)));
            connect(LeNouveauMode, SIGNAL(CompilationUpdated()), SLOT(onCompilationUpdated()));
            //Signal au mains que l'utilisateur à créé une instance X
            emit NouveauModeCreer();
#ifdef DEBUG_COMANDSAVE
            std::cout << "/" << func_name << std::endl;
            affichageqDebug("New Mode fin");
#endif /* DEBUG_COMANDSAVE */
        }
    }
}

//Permet de supprimer une instance
void SaveXmlFile::SupprimerUneInstance(Compilation * Instance)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
    affichageqDebug("Avant supprimer");
#endif /* DEBUG_COMANDSAVE */
    if (Compilation::NbInstance() > 1)
    {
        //On déconnecte les signaux avant de deleter l'instance
        disconnect(Instance, 0, 0,0);
        //On relève l'id de l'instance pour pouvoir en supprimer toutes les références
        int id = Instance->InstanceValue();
        //Le widget est viré du dock
        MainLayout->removeWidget(Instance);
        int taille = WidgetScrollArea->height();
        WidgetScrollArea->setFixedHeight(taille - 45);
        //On supprime l'instance
        delete Instance;
        //On met a jours la liste de référence
        for (int i = 0; i < 5; i++)
        {
            if (ListeModeOuvert[i] == id)
            {
                if (ListeModeOuvert[i] == ActiveModeNum)
                    ActiveModeNum = Compilation::InstanceActive()->InstanceValue();
                ListeModeOuvert[i] = -1;
                ListeModeOuvertPoint[i] = NULL;
                //Si c'est le mode actif, on l'applique au premier de la liste
                ChangeModeActif(ActiveModeNum);
                break;
            }
        }
#ifdef DEBUG_COMANDSAVE
        std::cout << "/" << func_name << std::endl;
        affichageqDebug("apres supp");
#endif /* DEBUG_COMANDSAVE */
    }
    else
        QMessageBox::critical(this, tr("Supression non authorisée"), tr("Il faut minimum 1 mode pour réaliser un effet lumineux."), QMessageBox::Ok);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Slot pour le boutton supprimer un mode
void SaveXmlFile::DeleteMode()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //permet de déterminer l'instance active à supprimer
    if (ListeModeOuvertPoint[ListeModeOuvert[ActiveModeNum]] != NULL)
    {
        //Supprime l'instance avec l'id
        SupprimerUneInstance(ListeModeOuvertPoint[ListeModeOuvert[ActiveModeNum]]);
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Changement de mode actif
void SaveXmlFile::ChangeModeActif(int Instance)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
    affichageqDebug("Avant changement Mode");
#endif /* DEBUG_COMANDSAVE */
    if (Compilation::NbInstance() > 1)
    {
        if (Compilation::ReturnInstance(ActiveModeNum) != NULL)
            Compilation::ReturnInstance(ActiveModeNum)->TurneLedOn(false);
        if (Compilation::ReturnInstance(Instance) != NULL)
            Compilation::ReturnInstance(Instance)->TurneLedOn(true);
        ActiveModeNum = Instance;
        emit ChangeActiveMode();
    }
    else
    {
        ActiveModeNum = Compilation::InstanceActive()->InstanceValue();
        if (Compilation::ReturnInstance(ActiveModeNum) != NULL)
            Compilation::ReturnInstance(ActiveModeNum)->TurneLedOn(true);
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
    affichageqDebug("Après changement Mode");
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui ajoute une condition horaire a un mode existant
void SaveXmlFile::addCondition(CondHoraire *CondH, int indice)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Ici l'égalité est une recopie, on test le pointeur recopié
    if (Compilation *comp = Compilation::ReturnInstance(ActiveModeNum))
        comp->addCondition(CondH, indice);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui ajoute un state dans l'arbre des états mais pas les entrées du state, et renvois l'id créé
bool SaveXmlFile::addKey(int id, int led, QString Module, int pause, Compilation::LedType Type, QString parametre)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << "/" << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return Compilation::InstanceActive()->addkey(id, led, Module, pause, Type, parametre);
}

//Fonction qui ajoute une led dans l'arbre des états
bool SaveXmlFile::addLed(int id, QString Module, const QString &Description)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << "/" << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return Compilation::InstanceActive()->addLed(id, Module, Description);
}

//Fonction qui ajoute une synchronisation
bool SaveXmlFile::addValidation(int idState, bool Validation, bool Synchronisation)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << "/" << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return Compilation::ReturnInstance(ActiveModeNum)->addValidation(idState, Validation, Synchronisation);
}

#ifdef DEBUG_COMANDSAVE
//Fonction pour le debugage
void SaveXmlFile::affichageqDebug(QString text)
{

    //boucle pour l'affichage avec qdebug
    qDebug() << "Contenu de listeModeOuvert[]" << text << "Mode actif" << ActiveModeNum;
    for (int i = 0; i < 5; i++)
    {
        qDebug() << "ListeModeOuvert[" << i << "] =" << ListeModeOuvert[i] << ListeModeOuvertPoint[i] << (ListeModeOuvert[i] == ActiveModeNum?"Mode Actif":"");
    }
}
#endif /* DEBUG_COMANDSAVE */

//Fonction qui retourne les condition horaire de linstance active
Compilation* SaveXmlFile::InstanceActive()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << "/" << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Ici l'égalité est une recopie, on test le pointeur recopié
    if (Compilation *comp = Compilation::ReturnInstance(ActiveModeNum))
        return comp;
    return NULL;
}

//Fonction Globale pour les const QString
QString SaveXmlFile::Const_QString(int type)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Variable globale pour les const QString
    static const char* TitreEtConstante[] = {
       QT_TR_NOOP("Vide")
    };
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return tr(TitreEtConstante[type]);
}

//Fonction pour supprimer les conditions horaire
void SaveXmlFile::SupprimerCondHoraire()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << "/" << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Ici l'égalité est une recopie, on test le pointeur recopié
    if (Compilation *comp = Compilation::ReturnInstance(ActiveModeNum))
        comp->SupprimeCondHoraire();
}

//Fonction qui applique et réapplique les labels pour introduire leur traduction quand c'est nécessaire
void SaveXmlFile::retranslate(QString lang)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name<< std::endl;
#endif /* DEBUG_COMANDSAVE */
    LabelListeMode->setText(tr("Liste Modes (max 5)", "Espace limiter pour faire une phrase entière."));
    ButtonDelete->setText(tr("Enreg. et Fermer", "Espace limité pour écrire \"Enregistrer et Fermer\" en entier."));
    ButtonNewMode->setText(tr("New Mode"));
    for (int i = 0; i < 5; i++)
        if(ListeModeOuvertPoint[i])
            ListeModeOuvertPoint[i]->retranslate(lang);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Sur demande des condition Horaire pour le dock Horaire
void SaveXmlFile::onDemandeCHToPlainText(QPlainTextEdit *ZoneDeTexte)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    InstanceActive()->DemndeLectureCH(ZoneDeTexte);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Sur demande des condition Horaire pour le dock Horaire
void SaveXmlFile::onDemandeCHToPlainText(CondHoraire *CondH, int indice)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    InstanceActive()->DemndeLectureCH(CondH, indice);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Le mode actif envois un signal de mise a jour
void SaveXmlFile::onCompilationUpdated()
{
    emit CompilationUpdated();
}
