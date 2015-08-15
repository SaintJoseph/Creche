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
    //Layout Horizontale pour l'éditeur de compilation
    LayoutCompilation = new QHBoxLayout;
    LayoutCompilation->setContentsMargins(1,1,1,1);
    LayoutCompilation->setAlignment(Qt::AlignTop);
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
    ButtonNewMode->setFixedHeight(20);
    //Bouton supression d'un mode
    ButtonDelete = new QPushButton(tr("Enreg. et Fermer", "Espace limité pour écrire \"Enregistrer et Fermer\" en entier."));
    ButtonDelete->setFont(font);
    ButtonDelete->setStyleSheet("QPushButton { width: 45px}");
    ButtonDelete->setFixedHeight(20);
    //Bouton pour lancer la compilation
    ButtonCompiler = new QPushButton(tr("Compiler..."));
    ButtonCompiler->setFont(font);
    ButtonCompiler->setFixedHeight(20);
    ButtonCompiler->setStyleSheet("QPushButton { width: 45px}");
    ButtonCompiler->setToolTip(tr("Lancer la compilation de tous les modes d'éclairage ouvert"));
    //Boutton Pour valider la compilation
    ButtonValider = new QPushButton(tr("Valider"));
    ButtonValider->setFont(font);
    ButtonValider->setFixedHeight(20);
    ButtonValider->setStyleSheet("QPushButton { width: 45px}");
    ButtonValider->setToolTip(tr("Valider l'ensemble des fichiers compilé, sélectionez le dossier de destination."));
    //Label pour la liste des instance de compilation
    LabelListeMode = new QLabel(tr("Liste Modes (max 5)", "Espace limiter pour faire une phrase entière."));
    LabelListeMode->setFont(font);
    LabelListeMode->setFixedHeight(18);
    //Label pour la liste des fichier crée
    LabelFichiersCree = new QLabel(tr("Fichier Compilé"));
    LabelFichiersCree->setFont(font);
    LabelFichiersCree->setFixedHeight(18);
    //Widget de base pour accuellir le layout principale
    Base = new QWidget;
    //Widget de taille fixe pour accuellir le main layout dans le srollArea
    WidgetScrollArea = new QWidget;
    WidgetScrollArea->setFixedHeight(10);
    //Affichage de la liste des fichiers crée
    FichierCree = new QListView;
    FichierCree->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //Ajout des bouttons au layout principale
    LayoutBase->addWidget(ButtonNewMode);
    LayoutBase->addWidget(ButtonDelete);
    //Ajout du label liste
    LayoutBase->addWidget(LabelListeMode);
    //Application du main layout sur le widget main
    LayoutBase->addWidget(ScrollArea);
    //Application du bouton compiler au scrollArea
    LayoutBase->addWidget(ButtonCompiler);
    //Application du layout au widget
    LayoutCompilation->addLayout(LayoutBase);
    Base->setLayout(LayoutCompilation);
    //Application du layout main au Widget scrollArea
    WidgetScrollArea->setLayout(MainLayout);
    //Application du widget au scrollArea
    ScrollArea->setWidget(WidgetScrollArea);
    //Ajout du label au layout compilation
    LayoutBase->addWidget(LabelFichiersCree);
    //Ajout de la liste au layout compilation
    LayoutBase->addWidget(FichierCree);
    //Ajout du layout compilation
    LayoutBase->addWidget(ButtonValider);

    //Application du widget au dock
    setWidget(Base);
    //Application du font au dock
    setFont(font);

    ChangeDockAffichage(false);

    //Lors de l'appuis sur le boutton on fait un nouveau mode
    connect(ButtonNewMode, SIGNAL(clicked()), SLOT(NewMode()));
    //connect temporaire pour test
    connect(ButtonDelete, SIGNAL(clicked()), SLOT(DeleteMode()));
    //Bouton pour lancer la compilation
    connect(ButtonCompiler, SIGNAL(clicked()), SLOT(onCompilationAsked()));
    //Double clic sur un nom de fichier dans la liste des fichier compilés
    connect(FichierCree, SIGNAL(doubleClicked(QModelIndex)), SLOT(onEditionRequested(QModelIndex)));
    //Bouton valider pour enregistrer les fichier compilé
    connect(ButtonValider, SIGNAL(clicked()), SLOT(onValidationClicked()));

    //Si une instance Compilation existe déjà on la récupère
    Flag1erInstanceNotInitialised = false;
    if (Compilation::NbInstance() != 0)
    {
        ListeModeOuvertPoint[0] =  Compilation::ReturnInstance(0);
        ListeModeOuvert[0] = ListeModeOuvertPoint[0]->InstanceValue();
        MainLayout->addWidget(ListeModeOuvertPoint[0]);
        int taille = WidgetScrollArea->height();
        WidgetScrollArea->setFixedHeight(taille + 45);
        Compilation::setModeActif(ListeModeOuvert[0]);
        ChangeModeActif(Compilation::ModeActif());
        Flag1erInstanceNotInitialised = true;
        //On connect le signal avec l'instance
        connect(ListeModeOuvertPoint[0], SIGNAL(MouseClickEvent(int)), SLOT(ChangeModeActif(int)));
        connect(ListeModeOuvertPoint[0], SIGNAL(DeleteMe(Compilation*)), SLOT(SupprimerUneInstance(Compilation*)));
    }

    /*********************************************************************
     * Essais pour le compilateur
     * ******************************************************************
    CompilationPreAssemblage test;
    DonneFichier *donnee = new DonneFichier;
    test.DonneDesFichiers.insert("00", donnee);
    Compilation *test2 = new Compilation;
    test2->CompilationCH(donnee, &test.Table);
    delete donnee;
    delete test2;
    /*********************************************************************
     * Essais pour le compilateur
     * ******************************************************************/
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
    affichageqDebug("Fin constructeur");
#endif /* DEBUG_COMANDSAVE */
}

SaveXmlFile::~SaveXmlFile()
{
    if (Editeur)
        delete Editeur;
    delete ButtonCompiler;
    delete ButtonDelete;
    delete ButtonNewMode;
    delete ButtonValider;

#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << "/" << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Initialisation des variables statics
int SaveXmlFile::ListeModeOuvert[] = {-1,-1,-1,-1,-1};
Compilation *SaveXmlFile::ListeModeOuvertPoint[] = {NULL};

char SaveXmlFile::ActiveMode()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << "/" <<std::endl;
#endif /* DEBUG_COMANDSAVE */
    return Compilation::ModeActif();
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
                if (ListeModeOuvert[i] == Compilation::ModeActif())
                    Compilation::setModeActif(Compilation::InstanceActive()->InstanceValue());
                ListeModeOuvert[i] = -1;
                ListeModeOuvertPoint[i] = NULL;
                //Si c'est le mode actif, on l'applique au premier de la liste
                ChangeModeActif(Compilation::ModeActif());
                break;
            }
        }
#ifdef DEBUG_COMANDSAVE
        std::cout << "/" << func_name << std::endl;
        //affichageqDebug("apres supp");
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
    if (ListeModeOuvertPoint[ListeModeOuvert[Compilation::ModeActif()]] != NULL)
    {
        //Désactive la sauvegarde automatique
        ListeModeOuvertPoint[ListeModeOuvert[Compilation::ModeActif()]]->saveAuto = false;
        //Supprime l'instance avec l'id
        SupprimerUneInstance(ListeModeOuvertPoint[ListeModeOuvert[Compilation::ModeActif()]]);
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
        if (Compilation::ReturnInstance(Compilation::ModeActif()) != NULL)
            Compilation::ReturnInstance(Compilation::ModeActif())->TurneLedOn(false);
        if (Compilation::ReturnInstance(Instance) != NULL)
            Compilation::ReturnInstance(Instance)->TurneLedOn(true);
        Compilation::setModeActif(Instance);
        emit ChangeActiveMode();
    }
    else
    {
        Compilation::setModeActif(Compilation::InstanceActive()->InstanceValue());
        if (Compilation::ReturnInstance(Compilation::ModeActif()) != NULL)
            Compilation::ReturnInstance(Compilation::ModeActif())->TurneLedOn(true);
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
    if (Compilation *comp = Compilation::ReturnInstance(Compilation::ModeActif()))
        comp->addCondition(CondH, indice);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui ajoute un state dans l'arbre des états mais pas les entrées du state, et renvois l'id créé
bool SaveXmlFile::addKey(int led, QString Module, int pause, Compilation::LedType Type, QString parametre)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << "/" << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return Compilation::InstanceActive()->addkey(led, Module, pause, Type, parametre);
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
    return Compilation::ReturnInstance(Compilation::ModeActif())->addValidation(idState, Validation, Synchronisation);
}

#ifdef DEBUG_COMANDSAVE
//Fonction pour le debugage
void SaveXmlFile::affichageqDebug(QString text)
{

    //boucle pour l'affichage avec qdebug
    qDebug() << "Contenu de listeModeOuvert[]" << text << "Mode actif" << Compilation::ModeActif();
    for (int i = 0; i < 5; i++)
    {
        qDebug() << "ListeModeOuvert[" << i << "] =" << ListeModeOuvert[i] << ListeModeOuvertPoint[i] << (ListeModeOuvert[i] == Compilation::ModeActif()?"Mode Actif":"");
    }
}
#endif /* DEBUG_COMANDSAVE */

//Fonction qui retourne les condition horaire de l'instance active
Compilation* SaveXmlFile::InstanceActive()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << "/" << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Ici l'égalité est une recopie, on test le pointeur recopié
    if (Compilation *comp = Compilation::ReturnInstance(Compilation::ModeActif()))
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
void SaveXmlFile::SupprimerCondHoraire(int ValeurID)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << "/" << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Ici l'égalité est une recopie, on test le pointeur recopié
    if (Compilation *comp = Compilation::ReturnInstance(Compilation::ModeActif()))
        comp->SupprimeCondHoraire(ValeurID);
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
    ButtonValider->setToolTip(tr("Valider l'ensemble des fichiers compilé, sélectionez le dossier de destination."));
    ButtonCompiler->setToolTip(tr("Lancer la compilation de tous les modes d'éclairage ouvert"));
    ButtonCompiler->setText(tr("Compiler..."));
    ButtonValider->setText(tr("Valider"));
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
    Compilation *CActive = InstanceActive();
    if (CActive != NULL)
        CActive->DemndeLectureCH(ZoneDeTexte);
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
    Compilation *CActive = InstanceActive();
    if (CActive != NULL)
        CActive->DemndeLectureCH(CondH, indice);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Le mode actif envois un signal de mise a jour
void SaveXmlFile::onCompilationUpdated()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    emit CompilationUpdated();
    //Le boutton de compilation apparait uniquement si tous les modes ouverts sont complet
    bool test = true, mode = false;
    for (int i = 0; i < 5; i++) {
        Compilation *Instance = ListeModeOuvertPoint[i];
        if (Instance) {
            mode = true;
            if (!Instance->onControleCompilation()) {
                test = false;
                break;
            }
        }
    }
    if (test && mode && ControlePriorite())
        ButtonCompiler->setVisible(true);
    else
        ButtonCompiler->setVisible(false);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Lancement de la compilation, fonction qui ordonne les opérations de compilation
void SaveXmlFile::onCompilationAsked()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    if (QMessageBox::warning(this, tr("Lancement de la compilation"), tr("<font color=\"#FF2A2A\"><b>ATTENTION:</b></font><br>Tous les modes d'éclairage ouvert, seront compilé.<br>Après confirmation il ne sera plus possible de modifier les données. Il sera possible d'éditer les fichiers compiler avant leur sauvegarde."), QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel) return;
    ChangeDockAffichage(true);
    //Création de la structure qui va accueillir les données compilées
    CompilationAssemblage = new CompilationPreAssemblage;
    //On récupère les modules utilisés par chaque mode, sous la forme de QstringList
    for (int i = 0; i < 5; i++){
        Compilation *Instance = ListeModeOuvertPoint[i];
        if (Instance) {
            CompilationAssemblage->ListeModules.append(Instance->InstanceModules());
        }
    }
    //La liste est triée et on enlève les doublons
    CompilationAssemblage->ListeModules.removeDuplicates();
    CompilationAssemblage->ListeModules.sort();
    //On attribue directement les variables avec adresse fixe pour éviter leur sur-écriture
    foreach (QString Module, CompilationAssemblage->ListeModules) {
        QString Ref = Module + "XP";
        AddToRamTable(&CompilationAssemblage->Table, Ref, QString::number(Module.remove(0,1).toInt() + 256, 16));
    }
    //Création du fichier 00
    DonneFichier *Fichier = new DonneFichier;
    CompilationFichierCommun(Fichier, &CompilationAssemblage->Table);
    CompilationAssemblage->DonneDesFichiers.insert(Fichier->ModeNom, Fichier);
    //Création du fichier CP (Contrôle Présence
    Fichier = new DonneFichier;
    CompilationControlePrence(Fichier, &CompilationAssemblage->Table);
    CompilationAssemblage->DonneDesFichiers.insert(Fichier->ModeNom, Fichier);
    //Appel des fonctions de compilation de chaque mode,
    for (int i = 0; i < 5; i++){
        Compilation *Instance = ListeModeOuvertPoint[i];
        if (Instance) {
            DonneFichier *Fichier = new DonneFichier;
            Instance->CompilationCH("M01MC" ,Fichier, &CompilationAssemblage->Table);
            CompilationAssemblage->DonneDesFichiers.insert(Fichier->ModeNom, Fichier);
        }
    }
    //Lorsque la compilation pré Assemblage est terminée, on affiche les fichiers qui vont être créé
    //Puis à la validation des ces fichiers, il sont enregistrer sur la carte micro SD(ou ailleur)
    ListeModel = new QStringListModel(QStringList (CompilationAssemblage->DonneDesFichiers.keys()));
    FichierCree->setModel(ListeModel);
    emit CompilationStart(true);
    TableUsedRAM::const_iterator i = CompilationAssemblage->Table.constBegin();
    while (i != CompilationAssemblage->Table.constEnd()) {
        std::cout << "RAM : " << i.key().toStdString() << ": " << i.value().toStdString() << std::endl;
        i++;
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction pour ajouter ou retourner une adresse RAM
QString SaveXmlFile::AddToRamTable(TableUsedRAM *TableRAM, QString Data, QString SpecialRef)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    if (Data.length() != 5)
        return QString("EEEE");
    QString RamAdresse = TableRAM->value(Data);
    if (RamAdresse.isEmpty()) {
        if (SpecialRef == RAM_DEFAUT_VALUE) {
            RamAdresse = QString::number(TableRAM->count() + 1,16);
        }
        else
            RamAdresse = SpecialRef;
    if (RamAdresse.length() == 1)
        RamAdresse.prepend("000");
    else if (RamAdresse.length() == 2)
        RamAdresse.prepend("00");
    else if (RamAdresse.length() == 3)
        RamAdresse.prepend("0");
    TableRAM->insert(Data, RamAdresse);
    }
    return RamAdresse;
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction pour changer l'affichage du dock en fonction si on est en édition ou en compilation
void SaveXmlFile::ChangeDockAffichage(bool mode)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    CompileMode = mode;
    if (!CompileMode) {
        ButtonDelete->setVisible(true);
        ButtonNewMode->setVisible(true);
        LabelListeMode->setVisible(true);
        ScrollArea->setVisible(true);
        //Restriction dimensionnelles
        setMinimumWidth(100);
        setMaximumWidth(150);
        LabelFichiersCree->setVisible(false);
        FichierCree->setVisible(false);
        ButtonValider->setVisible(false);
        //Le boutton de compilation apparait uniquement si tous les modes ouverts sont complet
        bool test = true, mode = false;
        for (int i = 0; i < 5; i++) {
            Compilation *Instance = ListeModeOuvertPoint[i];
            if (Instance) {
                mode = true;
                if (!Instance->onControleCompilation()) {
                    test = false;
                    break;
                }
            }
        }
        if (test && mode)
            ButtonCompiler->setVisible(true);
        else
            ButtonCompiler->setVisible(false);
    }
    else {
        ButtonCompiler->setVisible(false);
        ButtonDelete->setVisible(false);
        ButtonNewMode->setVisible(false);
        LabelListeMode->setVisible(false);
        ScrollArea->setVisible(false);
        LabelFichiersCree->setVisible(true);
        FichierCree->setVisible(true);
        ButtonValider->setVisible(true);
        setMaximumWidth(600);
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Edition d'un fichier compilé
void SaveXmlFile::onEditionRequested(QModelIndex index)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    if (!Editeur) {
        QVariant File = ListeModel->data(index, Qt::DisplayRole);
        DonneFichier *Donnee = CompilationAssemblage->DonneDesFichiers.value(File.toString());
        Editeur = new EditeurProg(Donnee);
        LayoutCompilation->addWidget(Editeur);
    }
    else {
        QVariant File = ListeModel->data(index, Qt::DisplayRole);
        DonneFichier *Donnee = CompilationAssemblage->DonneDesFichiers.value(File.toString());
        Editeur->setDonneeFichier(Donnee);
    }

#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Validation de la compilation
void SaveXmlFile::onValidationClicked()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QString PathToSDCard = QFileDialog::getExistingDirectory(this, tr("Sauvegarde sur la carte micro SD"));
    QFile File;
    QString TextFinal, Comment, TextUsedRam;
    DonneFichier *FileData;
    foreach (QString FileNom, ListeModel->stringList()) {
        FileData = CompilationAssemblage->DonneDesFichiers.value(FileNom);
        TextFinal = "#Fichier Exe_" + FileNom + ".cre\n#Créé le " + QDate::currentDate().toString() + " T " + QTime::currentTime().toString() + "\n";
        int CompteurLigne = 1;
        File.setFileName(PathToSDCard + "/Exe_" + FileNom + ".cre");
        foreach (QString Ligne, FileData->ListeIstruction) {
           if (!Ligne.isEmpty()) {
               Comment = FileData->Commentaire.value(Ligne, QString("123"));
               if (Comment != "123") {
                   TextFinal.append("#" + Comment + "\n\0");
               }
               if (Ligne.at(0) == QChar('#')) {
                   TextFinal.append(Ligne + QString("\n\0"));
               }
               else if (Ligne.contains("#")) {
                   TextFinal.append(Ligne.mid(Ligne.indexOf('#')) + QString("\n"));
               }
               else if (Ligne.contains("<") && Ligne.contains(">")) {
                   //Identification des saut de lignes
                   if (Ligne.contains(QChar('?'))) {
                       int index = Ligne.indexOf('?'), LigneSaut;
                       bool MoreNine = false;
                       switch (Ligne.at(index+1).toLatin1()) {
                       case 'R': //Saut de ligne relatif
                           if (Ligne.at(index+2).toLatin1() == '+') { //Saut en avant
                               if (Ligne.at(index+4).toLatin1() >= '0' && Ligne.at(index+4).toLatin1() <= '9') {
                                   LigneSaut = Ligne.mid(index+3, 2).toInt();
                                   MoreNine = true;
                               }
                               else {
                                   LigneSaut = Ligne.mid(index+3, 1).toInt();
                               }
                               LigneSaut = CompteurLigne + LigneSaut * PROGLIGNEITERATOR;
                           }
                           if (Ligne.at(index+2).toLatin1() == '-') { //Saut en arrière
                               if (Ligne.at(index+4).toLatin1() >= '0' && Ligne.at(index+4).toLatin1() <= '9') {
                                   LigneSaut = Ligne.mid(index+3, 2).toInt();
                                   MoreNine = true;
                               }
                               else {
                                   LigneSaut = Ligne.mid(index+3, 1).toInt();
                               }
                               LigneSaut = CompteurLigne - LigneSaut * PROGLIGNEITERATOR;
                           }
                           break;
                       default:
                           break;
                       }
                       if (MoreNine)
                           Ligne.replace(index, 5, IntToQString(LigneSaut).toUpper());
                       else
                           Ligne.replace(index, 4, IntToQString(LigneSaut).toUpper());
                   }
                   TextFinal.append(IntToQString(CompteurLigne).toUpper() + QString(" ") + Ligne.mid(Ligne.indexOf('<'), Ligne.indexOf('>') - Ligne.indexOf('<') + 1) + QString("\n"));
                   CompteurLigne += PROGLIGNEITERATOR;
               }
               else {
                   TextFinal.append(QString("#") + Ligne + QString("\n\0"));
               }
           }
        }
        File.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&File);
        out << TextFinal;
        File.close();
    }
    //Création du fichier avec l'utilisation de la RAM
    TableUsedRAM::const_iterator i = CompilationAssemblage->Table.constBegin();
    LECommandeModules Interpreteur;
    TextUsedRam = "#Fichier Used_RAM\n#Créé le " + QDate::currentDate().toString() + " T " + QTime::currentTime().toString() + "\nListe des adresses RAM uilisée, description ascociées, spécification de leur utilisation.\n";
    while (i != CompilationAssemblage->Table.constEnd()) {
        //On utilise la fonction d'interprétation des commandes du "commande module" pour traduire ce à quoi correspond chaque adresse RAM utilisée
        TextUsedRam.append(i.key() + " : " + i.value() + " : " + Interpreteur.Interpretation("<M00" + i.key() + ">").remove("Pour Programme PC,").replace("provenant de","De"));
        i++;
    }
    File.setFileName(PathToSDCard + "/Used_RAM");
    File.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&File);
    out << TextUsedRam;
    File.close();
    //Retour vers l'état de démarrage
    ChangeDockAffichage(false);
    //On ferme toute les instances anciennes
    //Cas où il n'y a qu'un mode lumineux
    if (Compilation::NbInstance() == 1) {
        //On crée une instance vide à un emplacement non utilisé
        NewMode();
        for (int i = 0; i < 5; i++) {
            if (ListeModeOuvertPoint[i]) {
                //Sauvegarde automatique sauvegardé, et fermeture
                ListeModeOuvertPoint[i]->saveAuto = true;
                //Dans ce cas on supprime toutes les instances existantes, avec forçage, car il y a 1 nouveau mode de créé
                SupprimerUneInstance(ListeModeOuvertPoint[i]);
            }
        }
    }
    //Cas où il y en a plusieurs
    else {
        for (int i = 0; i < 5; i++) {
            if (ListeModeOuvertPoint[i]) {
                //Sauvegarde automatique sauvegardé, et fermeture
                ListeModeOuvertPoint[i]->saveAuto = true;
                //Dans ce cas on supprime toutes les instances existantes, avec forçage, car il y a 1 nouveau mode de créé
                SupprimerUneInstance(ListeModeOuvertPoint[i]);
            }
            //On crée une instance vide à un emplacement non utilisé
            if (i == 0) {
                NewMode();
            }
        }
    }
    emit CompilationStart(false);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui converti un nombre en Qstring de 4 carractères
QString SaveXmlFile::IntToQString(int Value)
{
    QString StringValeur = QString::number(Value,16);
    if (StringValeur.length() == 1)
        StringValeur.prepend("000");
    else if (StringValeur.length() == 2)
        StringValeur.prepend("00");
    else if (StringValeur.length() == 3)
        StringValeur.prepend("0");
    return StringValeur;
}

//Fonction qui génère le fichier de base 00, lors de la compilation
void SaveXmlFile::CompilationFichierCommun(DonneFichier *DataToFill, TableUsedRAM *TableRAM)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //définition du nom du fichier
    DataToFill->ModeNom = "00";
    //Variable locale avec le nombre d'instances
    int NbInstance = Compilation::NbInstance();
    //Tableau avec les priorités
    int TabPriorite[5];
    for (int i = 0; i < 5; i++){
        if (ListeModeOuvertPoint[i])
            TabPriorite[i] = ListeModeOuvertPoint[i]->InstancePriorite();
    }
    //Permutation des pointeurs pour avoir un classement en fonction des priorités
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (ListeModeOuvertPoint[i] && ListeModeOuvertPoint[i + 1]) {
                if (TabPriorite[i] < TabPriorite[i+1]) {
                    //Permutation des valeurs
                    int Permut = TabPriorite[i+1];
                    TabPriorite[i+1] = TabPriorite[i];
                    TabPriorite[i] = Permut;
                    Compilation *Permutation = ListeModeOuvertPoint[i + 1];
                    ListeModeOuvertPoint[i+1] = ListeModeOuvertPoint[i];
                    ListeModeOuvertPoint[i] = Permutation;
                }
            }
            else {
                //cas où nous avons un pointeur Null et pas le suivant
                if (ListeModeOuvertPoint[i+1]) {
                    ListeModeOuvertPoint[i] = ListeModeOuvertPoint[i + 1];
                }
            }
        }
    }
#ifdef DEBUG_ARDUINO
    DataToFill->ListeIstruction.append("<M00M01Z Start 00>");
#endif
    //mise à 0 de M01MB sytématiquement avec le fichier 00, permet la comparaison de modules
    QString Commande = "<M01M01ERM01MBA" + AddToRamTable(TableRAM, QString("M01MB")) + "V0000>";
    //On ajoute un numéro unique qui n'est pas interprété pour unifier la commande
    Commande.append(QString::number(DataToFill->Commentaire.size()));
    DataToFill->ListeIstruction.append(Commande);
#ifdef DEBUG_ARDUINO
    DataToFill->Commentaire.insert(Commande, tr("mise à 0 de M01MB sytématique avec fichier 00"));
#endif

    //appel controle présence
    CompilationAppelRegSousProg("M01VP","CP",1,DataToFill, &CompilationAssemblage->Table);
    //appel des conditions d'effet lumineux
    CompilationAppelRegSousProg("M01M" + QString(QChar('C' + 0)),"C0",4,DataToFill, &CompilationAssemblage->Table);

    //Fin de l'exe
#ifdef DEBUG_ARDUINO
    Commande = "<M01M00Z Fin exe normal avec 00>";
    //On ajoute un numéro unique qui n'est pas interprété pour unifier la commande
    Commande.append(QString::number(DataToFill->Commentaire.size()));
    DataToFill->ListeIstruction.append(Commande);
    DataToFill->Commentaire.insert(Commande, tr("Fin de l'exe"));
    Commande = "<M01M00RA0>";
    DataToFill->ListeIstruction.append(Commande);
#else
    Commande = "<M01M00RA0>";
    //On ajoute un numéro unique qui n'est pas interprété pour unifier la commande
    Commande.append(QString::number(DataToFill->Commentaire.size()));
    DataToFill->ListeIstruction.append(Commande);
    DataToFill->Commentaire.insert(Commande, tr("Fin de l'exe"));
#endif


#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Autorisation d'éditer les effets lumineux
bool SaveXmlFile::onEditionRequested()
{
    bool test = false;
    for (int i = 0; i < 5; i++) {
        if (ListeModeOuvertPoint[i]) {
            test = true;
            break;
        }
    }
    if (!CompileMode && test)
        return true;
    else
        return false;
}

//Controle des priorites
bool SaveXmlFile::ControlePriorite()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Chaque niveau de priorité entre 0 et 9 est accepté 1 X
    //Le test est faux si la valeur de la priorité n'est comprise entre ces valeurs
    //Le test est également faut si plusieurs mode ont la même priorité
    //Tableau booléen pour chaque niveau de priorité
    bool LesPrioriteTest[10] = {0,0,0,0,0,0,0,0,0,0};
    //Boucle qui test les priorités
    for (int i = 0; i < 5; i++){
        if (ListeModeOuvertPoint[i]){
            int Prio = ListeModeOuvertPoint[i]->InstancePriorite();
            if (Prio < 10){
                if (Prio > -1){
                    if (!LesPrioriteTest[Prio])
                        LesPrioriteTest[Prio] = true;
                    else
                       return false;
                }
                else
                    return false;
            }
            else
                return false;
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return true;
}

//Compilation, programme un appel de sous programme pour le 00
void SaveXmlFile::CompilationAppelRegSousProg(QString variableRAM, QString fichier, int bouclage, DonneFichier *DataToFill, TableUsedRAM *TableRAM)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Comparaison de module entre M01MA et M01MB
    QString Commande = "<M01M01RG" + AddToRamTable(TableRAM, variableRAM) + "MA" + AddToRamTable(TableRAM, QString("M01MB")) + "L?R+2>";
    //On ajoute un numéro unique qui n'est pas interprété pour unifier la commande
#ifdef DEBUG_ARDUINO
    Commande.append(QString::number(DataToFill->Commentaire.size()));
#endif
    DataToFill->ListeIstruction.append(Commande);
#ifdef DEBUG_ARDUINO
    DataToFill->Commentaire.insert(Commande, tr("Comparaison de modules"));
#endif

    //Si nok mise à zero de M01MA (valeur supérieur à la valeur de bouclage
    Commande = "<M01M01ER" + variableRAM + "A" + AddToRamTable(TableRAM, variableRAM) + "V" + IntToQString(bouclage + 1) + ">";
    //On ajoute un numéro unique qui n'est pas interprété pour unifier la commande
#ifdef DEBUG_ARDUINO
    Commande.append(QString::number(DataToFill->Commentaire.size()));
#endif
    DataToFill->ListeIstruction.append(Commande);
#ifdef DEBUG_ARDUINO
    DataToFill->Commentaire.insert(Commande, tr("Si nok mise à zero Variable principale"));
#endif

    //Controle sur le nombre d'appels de la fonction
    Commande = "<M01M01RG" + AddToRamTable(TableRAM, variableRAM) + "IV" + IntToQString(bouclage) + "L?R+2>";
    //On ajoute un numéro unique qui n'est pas interprété pour unifier la commande
#ifdef DEBUG_ARDUINO
    Commande.append(QString::number(DataToFill->Commentaire.size()));
#endif
    DataToFill->ListeIstruction.append(Commande);
#ifdef DEBUG_ARDUINO
    DataToFill->Commentaire.insert(Commande, tr("Controle sur le nombre d'appels"));
#endif

    //Appels du sous programme souhaité, condition tjr vraie
    Commande = "<M01M01RF" + AddToRamTable(TableRAM, QString("M01MB")) + "EV0000F" + fichier + ">";
    //On ajoute un numéro unique qui n'est pas interprété pour unifier la commande
#ifdef DEBUG_ARDUINO
    Commande.append(QString::number(DataToFill->Commentaire.size()));
#endif
    DataToFill->ListeIstruction.append(Commande);
#ifdef DEBUG_ARDUINO
    DataToFill->Commentaire.insert(Commande, tr("Appel du sous programme"));
#endif

#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Compilation fichier commun controle présence CP
void SaveXmlFile::CompilationControlePrence(DonneFichier *DataToFill, TableUsedRAM *TableRAM)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //définition du nom du fichier
    DataToFill->ModeNom = "CP";
    QString Commande;
    //On reset chaque adresse mémoire lié à la présence d'un module sur le réseau
    foreach (QString Module, CompilationAssemblage->ListeModules) {
        //Dans la fonction AddToRamTable on ne précise plus l'adresse particuliaire car elle à déjà été définie dans la fonction main de la compilation
        Commande = "<M01M01ER" + Module + "XPA" + AddToRamTable(TableRAM, QString(Module + "XP")) + "V0000>";
        DataToFill->ListeIstruction.append(Commande);
        Commande = "<M01M01ER" + Module + "VAA" + AddToRamTable(TableRAM, QString(Module + "VA")) + "V0000>";
        DataToFill->ListeIstruction.append(Commande);
    }
    Commande = "<A00M01XP>";
#ifdef DEBUG_ARDUINO
    Commande.append(QString::number(DataToFill->Commentaire.size()));
#endif
    DataToFill->ListeIstruction.append(Commande);
#ifdef DEBUG_ARDUINO
    DataToFill->Commentaire.insert(Commande, tr("Demande de présence pour tous"));
#endif
    Commande = "<M01M01RD03E8>";
    DataToFill->ListeIstruction.append(Commande);
    int index = 0, nbLigne = CompilationAssemblage->ListeModules.size();
    //Test de la présence de chaque module sur le réseau
    foreach (QString Module, CompilationAssemblage->ListeModules) {
        //Dans la fonction AddToRamTable on ne précise plus l'adresse particuliaire car elle à déjà été définie dans la fonction main de la compilation
        Commande = "<M01M01RG" + AddToRamTable(TableRAM, QString(Module + "XP")) + "V0001L?R+" + QString::number(nbLigne + index++ * 3 + 5) + ">";
        DataToFill->ListeIstruction.append(Commande);
    }
    Commande = "<M01M00ZTous modules present>";
#ifdef DEBUG_ARDUINO
    Commande.append(QString::number(DataToFill->Commentaire.size()));
#endif
    DataToFill->ListeIstruction.append(Commande);
#ifdef DEBUG_ARDUINO
    DataToFill->Commentaire.insert(Commande, tr("Tous les modules sont présent"));
#endif
    Commande = "<M01M01ERM01VPA" + AddToRamTable(TableRAM, "M01VP") + "V0000>";
    DataToFill->ListeIstruction.append(Commande);
    Commande = "<M01M01RF" + AddToRamTable(TableRAM, "M01VA") + "EV0000F00>";
    DataToFill->ListeIstruction.append(Commande);
    Commande = "<M01M00ZErreur dans l'exe de CP>";
    DataToFill->ListeIstruction.append(Commande);
    Commande = "<M01M01RA0>";
    DataToFill->ListeIstruction.append(Commande);
    //Liste d'instruction spécifique pour chaque module en cas d'absence de ce module sur le réseau
    index = 0;
    foreach (QString Module, CompilationAssemblage->ListeModules) {
        //Dans la fonction AddToRamTable on ne précise plus l'adresse particuliaire car elle à déjà été définie dans la fonction main de la compilation
        Commande = "<M01M00Z" + Module + " ne répond pas>";
        DataToFill->ListeIstruction.append(Commande);
        Commande = "<M01M01RO" + AddToRamTable(TableRAM, QString(Module + "VA")) + "PV0001>";
        DataToFill->ListeIstruction.append(Commande);
        Commande = "<M01M01RG" + AddToRamTable(TableRAM, QString(Module + "VA")) + "IV0005L?R-" + QString::number(nbLigne + index * 4 + 8) + ">";
        DataToFill->ListeIstruction.append(Commande);
        //Attention après la dernière utilisation de l'index on l'incrémente pour la boucle suivante
        Commande = "<M01M01RG" + AddToRamTable(TableRAM, QString(Module + "VA")) + "EV0005L?R+" + QString::number((nbLigne - index++ - 1) * 4 + 1) + ">";
        DataToFill->ListeIstruction.append(Commande);
    }
    Commande = "<M01M00Z Fin exe, 1 M ne répond pas>";
    DataToFill->ListeIstruction.append(Commande);
    Commande = "<M01M01RA0>";
    DataToFill->ListeIstruction.append(Commande);

/*
 * #Fichier de controle des présences
#On met toutes les présences à zero
0001 <M01M01ERM01XPA0101V0000>
0002 <M01M01ERM04XPA0104V0000>
0003 <M01M01ERM02XPA0102V0000>
0004 <M01M01ERM03XPA0103V0000>

0010 <M01M01ERM01VCA0004V0000>
#Demande de présence
0011 <A00M01XP>
0012 <M01M01RD03E8>

#Test sur les présences
0020 <M01M01RG0104DV0001L0033>
0021 <M01M01RG0102DV0001L003B>
0022 <M01M01RG0103DV0001L0043>

#Tous les modules sont présent
0030 <M01M01ZTous modules present>
0031 <M01M01RF0001EV0000F00>
0032 <M01M01ZErreur dans l'exe de CP>

#fin de controle des présences
0033 <M01M00Z M4 ne répond pas>
0035 <M01M01RO0004PV0001>
0037 <M01M01RG0004IV0005L0012>
0039 <M01M01RG0004EV0005L0049>
003B <M01M00Z M2 ne répond pas>
003D <M01M01RO0004PV0001>
003F <M01M01RG0004IV0005L0012>
0041 <M01M01RG0004EV0005L0049>
0043 <M01M00Z M3 ne répond pas>
0045 <M01M01RO0004PV0001>
0047 <M01M01RG0004IV0005L0012>

0049 <M01M01Z Fin exe, 1 M ne répond pas>
004B <M01M01RA0>
 */
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

