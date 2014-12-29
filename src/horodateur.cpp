#include "horodateur.h"

Horodateur::Horodateur(const QString & title, QWidget *parent , Qt::WindowFlags flags) :
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
    //MainLayout pour le scrollArea
    MainLayout = new QVBoxLayout;
    MainLayout->setContentsMargins(1,1,1,1);
    MainLayout->setSpacing(1);
    //Layout de base
    LayoutBase = new QVBoxLayout;
    LayoutBase->setContentsMargins(0,0,0,0);
    LayoutBase->setSpacing(1);
    //Layout start date
    LayoutStartDate = new QHBoxLayout;
    LayoutStartDate->setContentsMargins(0,0,0,0);
    LayoutStartDate->setSpacing(2);
    //Layout End date
    LayoutEndDate = new QHBoxLayout;
    LayoutEndDate->setContentsMargins(0,0,0,0);
    LayoutEndDate->setSpacing(2);
    //Layout pour la zone de date
    LayoutDateEdite = new QVBoxLayout;
    LayoutDateEdite->setContentsMargins(1,1,1,1);
    LayoutDateEdite->setSpacing(1);
    LayoutDateEdite->setAlignment(Qt::AlignJustify);
    //Layout pour la zone de sélection d'un jour
    LayoutJourSelect = new QVBoxLayout;
    LayoutJourSelect->setContentsMargins(1,1,1,1);
    LayoutJourSelect->setSpacing(1);
    LayoutJourSelect->setAlignment(Qt::AlignJustify);
    //Layout pour la zone de sélection d'heure
    LayoutTimeEdit = new QVBoxLayout;
    LayoutTimeEdit->setContentsMargins(1,1,1,1);
    LayoutTimeEdit->setSpacing(1);
    LayoutTimeEdit->setAlignment(Qt::AlignJustify);
    //Layout Pour la navigation
    LayoutNavig = new QHBoxLayout;
    LayoutNavig->setContentsMargins(0,0,0,0);
    LayoutNavig->setSpacing(5);
    LayoutNavig->setAlignment(Qt::AlignHCenter);
    //Label Index
    LabelIndex = new QLabel("0");
    LabelIndex->setFont(font);
    LabelIndex->setFixedHeight(20);
    //Label date début
    LabelStartDate = new QLabel("");
    LabelStartDate->setFont(font);
    LabelStartDate->setFixedHeight(20);
    //Label date de fin
    LabelEndDate = new QLabel("");
    LabelEndDate->setFont(font);
    LabelEndDate->setFixedHeight(20);
    //Label selectionner un jour de la semaine
    LabelJourSelect = new QLabel("");
    LabelJourSelect->setFont(font);
    LabelJourSelect->setFixedHeight(20);
    //Label sélectionner une heure de début
    LabelTimeStart = new QLabel("");
    LabelTimeStart->setFont(font);
    LabelTimeStart->setFixedHeight(20);
    //Label sélectionner une heure de fin
    LabelTimeEnd = new QLabel("");
    LabelTimeEnd->setFont(font);
    LabelTimeEnd->setFixedHeight(20);
    //Bouton Navigation gauche
    ButtonNavigLf = new QPushButton("<-");
    ButtonNavigLf->setFixedHeight(20);
    ButtonNavigLf->setFixedWidth(22);
    //Boutton Navigation droite
    ButtonNavigRg = new QPushButton("->");
    ButtonNavigRg->setFixedHeight(20);
    ButtonNavigRg->setFixedWidth(22);
    //Boutton pour ouvrir la fenètre de sélection de date start
    ButtonStartDate = new QPushButton("...");
    ButtonStartDate->setFont(font);
    ButtonStartDate->setFixedHeight(20);
    ButtonStartDate->setFixedWidth(22);
    //Boutton pour ouvrir la fenètre de sélection de date end
    ButtonEndDate = new QPushButton("...");
    ButtonEndDate->setFont(font);
    ButtonEndDate->setFixedHeight(20);
    ButtonEndDate->setFixedWidth(22);
    //Boutton pour valider les données introduite
    ButtonValider = new QPushButton;
    ButtonValider->setFont(font);
    ButtonValider->setFixedHeight(20);
    ButtonValider->setVisible(false);
    //Boutton pour supprimer ce qui est programmé
    ButtonDelete = new QPushButton;
    ButtonDelete->setFixedHeight(20);
    ButtonDelete->setFont(font);
    //Widget de base qui accueil le main layout
    Base = new QWidget;
    //Widget de taille appliquer au scrollArea
    WidgetScrollArea = new QWidget;
    WidgetScrollArea->setFixedHeight(213);
    //Widget pour les différentes zones,
    //Chaque widget est rendu visible ou invisible en fonction de la sélection
    //dans la comboBox
    DateEdite = new QWidget;
    DateEdite->setMinimumHeight(80);
    //Widget pour la sélection d'un jour
    SelectJour = new QWidget;
    SelectJour->setMaximumHeight(40);
    //Widget pour la selection d'heures
    TimeEdit = new QWidget;
    TimeEdit->setMinimumHeight(80);
    //ComboBox pour sélectionner le type de condition horaire
    ComboBoxSelect = new QComboBox;
    ComboBoxSelect->addItem(tr("Faire sélection"), Vide);
    ComboBoxSelect->addItem(tr("Période, de date à date"), Periode);
    ComboBoxSelect->addItem(tr("Tous les jours"), Journalier);
    ComboBoxSelect->addItem(tr("Un jour de la semaine"), Hebdomadaire);
    ComboBoxSelect->addItem(tr("View"), View);
    ComboBoxSelect->setFont(font);
    //ComboBox jour de la semaine sélectionné
    JourSelect = new QComboBox;
    JourSelect->addItem(tr("Lundi"), 1);
    JourSelect->addItem(tr("Mardi"), 2);
    JourSelect->addItem(tr("Mercredi"), 3);
    JourSelect->addItem(tr("Jeudi"), 4);
    JourSelect->addItem(tr("Vendredi"), 5);
    JourSelect->addItem(tr("Samedi"), 6);
    JourSelect->addItem(tr("Dimanche"), 7);
    JourSelect->setFont(font);
    JourSelect->setFixedHeight(20);
    //Date edit pour la date de début
    DateEditeStart = new QDateEdit;
    DateEditeStart->setFont(font);
    DateEditeStart->setFixedHeight(20);
    DateEditeStart->setDate(QDate::currentDate());
    //Date edit pour la date de fin
    DateEditeEnd = new QDateEdit;
    DateEditeEnd->setFont(font);
    DateEditeEnd->setFixedHeight(20);
    DateEditeEnd->setDate(QDate::currentDate());
    //Scroll area pour acuellir les layouts avec les champs de travail
    ScrollArea = new QScrollArea;
    ScrollArea->setContentsMargins(0,0,0,0);
    ScrollArea->setWidgetResizable(true);
    ScrollArea->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //Time edit pour l'heure de début
    TimeEditStart = new QTimeEdit;
    TimeEditStart->setFont(font);
    TimeEditStart->setFixedHeight(20);
    //Time edite pour l'heure de fin
    TimeEditEnd = new QTimeEdit;
    TimeEditEnd->setFont(font);
    TimeEditEnd->setFixedHeight(20);
    //View de text edit qui permet de visuliser ce qui est programmé
    TextView = new QPlainTextEdit;
    TextView->setReadOnly(true);
    TextView->setFont(font);
    //Création du calendrier, il reste cependant caché à l'utilisateur
    CalendrierActif = new QCalendarWidget;
    CalendrierActif->setVisible(false);
    CalendrierActif->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    CalendrierActif->setHorizontalHeaderFormat(QCalendarWidget::NoHorizontalHeader);

    //Parti navigation entre les condition horaires
    LayoutNavig->addWidget(ButtonNavigLf);
    LayoutNavig->addWidget(LabelIndex);
    LayoutNavig->addWidget(ButtonNavigRg);
    //Ajout des widgets et layout pour l'ensemble Date
    LayoutStartDate->addWidget(DateEditeStart);
    LayoutStartDate->addWidget(ButtonStartDate);
    LayoutEndDate->addWidget(DateEditeEnd);
    LayoutEndDate->addWidget(ButtonEndDate);
    LayoutDateEdite->addWidget(LabelStartDate);
    LayoutDateEdite->addLayout(LayoutStartDate);
    LayoutDateEdite->addWidget(LabelEndDate);
    LayoutDateEdite->addLayout(LayoutEndDate);
    LayoutDateEdite->addWidget(CalendrierActif);
    DateEdite->setLayout(LayoutDateEdite);
    DateEdite->setVisible(false);
    //Ajout de widget pour l'ensemble jour
    LayoutJourSelect->addWidget(LabelJourSelect);
    LayoutJourSelect->addWidget(JourSelect);
    SelectJour->setLayout(LayoutJourSelect);
    SelectJour->setVisible(false);
    //Ajout des widget pour l'ensemble heure
    LayoutTimeEdit->addWidget(LabelTimeStart);
    LayoutTimeEdit->addWidget(TimeEditStart);
    LayoutTimeEdit->addWidget(LabelTimeEnd);
    LayoutTimeEdit->addWidget(TimeEditEnd);
    TimeEdit->setLayout(LayoutTimeEdit);
    TimeEdit->setVisible(false);
    //Ajout des widgets sur le main layout
    MainLayout->addWidget(DateEdite);
    MainLayout->addWidget(SelectJour);
    MainLayout->addWidget(TimeEdit);
    MainLayout->addWidget(ButtonValider);
    MainLayout->addWidget(TextView);
    MainLayout->addWidget(ButtonDelete);
    //Application du layout principale sur un widget dont ont impose la taille
    WidgetScrollArea->setLayout(MainLayout);
    //Application au layout de base
    ScrollArea->setWidget(WidgetScrollArea);
    LayoutBase->addLayout(LayoutNavig);
    LayoutBase->addWidget(ComboBoxSelect);
    LayoutBase->addWidget(ScrollArea);
    //Application au dock
    Base->setLayout(LayoutBase);
    setWidget(Base);
    setMinimumWidth(110);

    //On met à zero les conditions horaires
    AffichageOnChangeMode();
    //On place tous les labels
    retranslate();

    connect(ComboBoxSelect, SIGNAL(currentIndexChanged(int)), SLOT(ChangeType(int)));
    connect(ButtonValider, SIGNAL(clicked()), SLOT(PrepareToSend()));
    connect(ButtonDelete, SIGNAL(clicked()), SLOT(BouttonDeleteClick()));
    connect(ButtonNavigLf, SIGNAL(clicked()), SLOT(onNavigClickLF()));
    connect(ButtonNavigRg, SIGNAL(clicked()), SLOT(onNavigClickRG()));
    connect(ButtonStartDate, SIGNAL(clicked()), SLOT(onCalendarStartAsked()));
    connect(ButtonEndDate, SIGNAL(clicked()), SLOT(onCalendarEndtAsked()));
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

Horodateur::~Horodateur()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    delete TimeEditStart;
    delete TimeEditEnd;
    delete DateEditeStart;
    delete DateEditeEnd;
    delete ComboBoxSelect;
    delete JourSelect;
    delete ButtonEndDate;
    delete ButtonStartDate;
    delete ButtonValider;
    delete LabelEndDate;
    delete LabelJourSelect;
    delete LabelStartDate;
    delete LabelTimeEnd;
    delete LabelTimeStart;
    delete LabelIndex;
    delete ButtonNavigLf;
    delete ButtonNavigRg;
    delete LayoutNavig;
    delete LayoutStartDate;
    delete LayoutEndDate;
    delete LayoutDateEdite;
    delete LayoutJourSelect;
    delete LayoutTimeEdit;
    delete DateEdite;
    delete SelectJour;
    delete TimeEdit;
    delete MainLayout;
    delete WidgetScrollArea;
    delete ScrollArea;
    delete LayoutBase;
    delete Base;
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui adapte la zone visible du dock
void Horodateur::ChangeType(int index)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //On récupère la valeur de la comboBox et on caste suivant Type (enum)
    TypeCondHoraire Select = static_cast<TypeCondHoraire>(ComboBoxSelect->itemData(index).toInt());

    switch(Select)
    {
    case Periode:
        WidgetScrollArea->setFixedHeight(129);
        DateEdite->setVisible(true);
        SelectJour->setVisible(false);
        TimeEdit->setVisible(false);
        ButtonValider->setVisible(true);
        TextView->setVisible(false);
        ButtonDelete->setVisible(true);
        break;
    case Vide:
        WidgetScrollArea->setFixedHeight(42);
        DateEdite->setVisible(false);
        SelectJour->setVisible(false);
        TimeEdit->setVisible(false);
        ButtonValider->setVisible(false);
        TextView->setVisible(false);
        ButtonDelete->setVisible(false);
        break;
    case Hebdomadaire:
        WidgetScrollArea->setFixedHeight(172);
        DateEdite->setVisible(false);
        SelectJour->setVisible(true);
        TimeEdit->setVisible(true);
        ButtonValider->setVisible(true);
        TextView->setVisible(false);
        ButtonDelete->setVisible(true);
        break;
    case Journalier:
        WidgetScrollArea->setFixedHeight(129);
        DateEdite->setVisible(false);
        SelectJour->setVisible(false);
        TimeEdit->setVisible(true);
        ButtonValider->setVisible(true);
        TextView->setVisible(false);
        ButtonDelete->setVisible(true);
        break;
    case View:
        if (Compilation::NbInstance() > 0)
        {
            WidgetScrollArea->setFixedHeight(500);
            DateEdite->setVisible(false);
            SelectJour->setVisible(false);
            TimeEdit->setVisible(false);
            ButtonValider->setVisible(false);
            TextView->setVisible(true);
            ButtonDelete->setVisible(false);
            TextView->clear();
            TextView->moveCursor(QTextCursor::End);
            emit DemandeCHstring(TextView);
        }
        else
        {
            WidgetScrollArea->setFixedHeight(42);
            DateEdite->setVisible(false);
            SelectJour->setVisible(false);
            TimeEdit->setVisible(false);
            ButtonValider->setVisible(false);
            TextView->setVisible(false);
            ButtonDelete->setVisible(false);
            ComboBoxSelect->setCurrentIndex(Vide);
        }
        break;
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Slot qui prepare les conditions horaire pour la compilation
void Horodateur::PrepareToSend()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //On récupère la valeur de la comboBox et on caste suivant Type (enum)
    TypeCondHoraire Select = static_cast<TypeCondHoraire>(ComboBoxSelect->itemData(ComboBoxSelect->currentIndex()).toInt());
    //Initialisation des variables
    CondHoraire *CondH = new CondHoraire{0,0,0,0,0,0,0,0,0,0};
    CondH->Type = Select;
    switch(Select)
    {
    case Periode:
        CondH->DMois = DateEditeStart->date().month();
        CondH->DJour = DateEditeStart->date().day();
        CondH->EMois = DateEditeEnd->date().month();
        CondH->EJour = DateEditeEnd->date().day();
        break;
    case Vide:
        QMessageBox::information(this, tr("Pas de condition Horaire"), tr("Il n'y a condition horaire programmée"), QMessageBox::Ok);
        return;
    case Hebdomadaire:
        CondH->DHeure = TimeEditStart->time().hour();
        CondH->DMinute = TimeEditStart->time().minute();
        CondH->EHeure = TimeEditEnd->time().hour();
        CondH->EMinute = TimeEditEnd->time().minute();
        CondH->DJourSem = JourSelect->currentData().toInt();
        CondH->EJourSem = JourSelect->currentData().toInt();
        break;
    case Journalier:
        CondH->DHeure = TimeEditStart->time().hour();
        CondH->DMinute = TimeEditStart->time().minute();
        CondH->EHeure = TimeEditEnd->time().hour();
        CondH->EMinute = TimeEditEnd->time().minute();
        break;
    case View:
#ifdef DEBUG_COMANDSAVE
        std::cout << "/" << func_name << "(View)" << std::endl;
#endif /* DEBUG_COMANDSAVE */
        delete CondH;
        return;
    }
#ifdef DEBUG_COMANDSAVE
    qDebug() << "Condition horaire:" << CondH->DMois << CondH->EMois << CondH->DJour << CondH->EJour << CondH->DJourSem << CondH->EJourSem << CondH->DHeure << CondH->EHeure << CondH->DMinute << CondH->EMinute;
#endif /* DEBUG_COMANDSAVE */
    emit SendCondHoraire(CondH, LabelIndex->text().toInt());
    //Normalement CondH est supprimé après sa lecture, on le delete seulement s'il n'a pas été correctement supprimé
    if (!CondH)
#ifdef DEBUG_COMANDSAVE
    {
        qDebug() << "Demande de delete CondHoraire";
        delete CondH;
    }
#else /* DEBUG_COMANDSAVE */
        delete CondH;
#endif /* DEBUG_COMANDSAVE */
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Slot pour le boutton delete
void Horodateur::BouttonDeleteClick()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    emit DemandeSuppCondHoraire(LabelIndex->text().toInt());
    TextView->clear();
    TextView->moveCursor(QTextCursor::End);
    //Le slot se charge de modifier le contenu du TextView
    emit DemandeCHstring(TextView);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction pour lorsque l'affichage doit changer, pour autre chose qu la nav.
void Horodateur::AffichageOnChangeMode()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //on utilise la fonction de navigation avec le param de début
    onNavigClick(First);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction pour appliquer et réappliquer les labels suite a une demande de traduction
void Horodateur::retranslate()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    LabelStartDate->setText(tr("Date de début"));
    LabelEndDate->setText(tr("Date de fin"));
    LabelJourSelect->setText(tr("Choisir un jour"));
    LabelTimeStart->setText(tr("Heure de début"));
    LabelTimeEnd->setText(tr("Heure de fin"));
    ButtonValider->setText(tr("Valider"));
    ButtonDelete->setText(tr("Delete"));
    ComboBoxSelect->setItemText(0, tr("Faire sélection"));
    ComboBoxSelect->setItemText(1, tr("Période, de date à date"));
    ComboBoxSelect->setItemText(2, tr("Tous les jours"));
    ComboBoxSelect->setItemText(3, tr("Un jour de la semaine"));
    ComboBoxSelect->setItemText(4, tr("View"));
    JourSelect->setItemText(0,tr("Lundi"));
    JourSelect->setItemText(1,tr("Mardi"));
    JourSelect->setItemText(2,tr("Mercredi"));
    JourSelect->setItemText(3,tr("Jeudi"));
    JourSelect->setItemText(4,tr("Vendredi"));
    JourSelect->setItemText(5,tr("Samedi"));
    JourSelect->setItemText(6,tr("Dimanche"));
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Slot pour les bouton de navigation
void Horodateur::onNavigClick(Navigation Side)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Initialisation des variables
    CondHoraire *CondH = new CondHoraire{0,0,0,0,0,0,0,0,0,0};
    CondH->Type = Vide;
    if (Side == First) {
        LabelIndex->setText("0");
    }
    if (Side == Left && LabelIndex->text().toInt() > 0) {
        LabelIndex->setText(QString::number(LabelIndex->text().toInt() - 1));
    }
    if (Side == Right && LabelIndex->text().toInt() < 10) {
        LabelIndex->setText(QString::number(LabelIndex->text().toInt() + 1));
    }
    //On fait une requete pour connaitre le type et les valeurs de condition horaire pour l'index
    ComboBoxSelect->setCurrentIndex(4);
    emit DemandeCHstring(CondH, LabelIndex->text().toInt());
    //On adapte l'affichage
    switch(CondH->Type)
    {
    case Periode:
        ComboBoxSelect->setCurrentIndex(1);
        break;
    case Vide:
        ComboBoxSelect->setCurrentIndex(0);
        break;
    case Hebdomadaire:
        ComboBoxSelect->setCurrentIndex(3);
        break;
    case Journalier:
        ComboBoxSelect->setCurrentIndex(2);
        break;
    case View:
        //On ne met pas le mode View, mais le mode Vide, entre autre pour l'allumage
        //Le mode view est la uniquement pour le débugage
        ComboBoxSelect->setCurrentIndex(0);
    }
    JourSelect->setCurrentIndex(JourSelect->findData(CondH->DJourSem));
    QDate DateStart, DateEnd;
    QTime TimeStart, TimeEnd;
    DateStart.setDate(QDate::currentDate().year(), CondH->DMois, CondH->DJour);
    DateEnd.setDate(QDate::currentDate().year(), CondH->EMois, CondH->EJour);
    TimeStart.setHMS(CondH->DHeure, CondH->DMinute, 0);
    TimeEnd.setHMS(CondH->EHeure, CondH->EMinute, 0);
    DateEditeStart->setDate(DateStart);
    DateEditeEnd->setDate(DateEnd);
    TimeEditStart->setTime(TimeStart);
    TimeEditEnd->setTime(TimeEnd);
    //On met jour les boutton (actif ou inactif)
    if (LabelIndex->text().toInt() == 0) {
        ButtonNavigLf->setEnabled(false);
    }
    else
        ButtonNavigLf->setEnabled(true);
    if (LabelIndex->text().toInt() == 10 || CondH->Type == Vide) {
        ButtonNavigRg->setEnabled(false);
    }
    else
        ButtonNavigRg->setEnabled(true);
    //finalement on supprime l'objet qui a permi la mise a jour de l'affichage
    delete CondH;
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Navig Left
void Horodateur::onNavigClickLF()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    onNavigClick(Left);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Navig Right
void Horodateur::onNavigClickRG()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    onNavigClick(Right);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Mise a jour de l'affichage pour un nouveau mode
void Horodateur::onModeUpdate() {
    onNavigClick(First);
}

//Création d'une fenetre de calendrier
void Horodateur::onCalendarStartAsked()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //On initialise le pointeur vers le bon editeur de date
    DateRecup = DateEditeStart;
    CalendrierActif->setSelectedDate(DateRecup->date());
    connect(CalendrierActif, SIGNAL(selectionChanged()), SLOT(onCalendarReturn()));
    LabelStartDate->setVisible(false);
    LabelEndDate->setVisible(false);
    DateEditeStart->setVisible(false);
    DateEditeEnd->setVisible(false);
    ButtonStartDate->setVisible(false);
    ButtonEndDate->setVisible(false);
    CalendrierActif->setVisible(true);
    WidgetScrollArea->setFixedHeight(250);
    setMinimumWidth(420);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Création d'une fenetre de calendrier
void Horodateur::onCalendarEndtAsked()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //On initialise le pointeur vers le bon editeur de date
    DateRecup = DateEditeEnd;
    CalendrierActif->setSelectedDate(DateRecup->date());
    connect(CalendrierActif, SIGNAL(selectionChanged()), SLOT(onCalendarReturn()));
    LabelStartDate->setVisible(false);
    LabelEndDate->setVisible(false);
    DateEditeStart->setVisible(false);
    DateEditeEnd->setVisible(false);
    ButtonStartDate->setVisible(false);
    ButtonEndDate->setVisible(false);
    CalendrierActif->setVisible(true);
    WidgetScrollArea->setFixedHeight(250);
    setMinimumWidth(420);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Retour d'une date par le calendrier
void Horodateur::onCalendarReturn()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //On applique la date renvoyé au widget correspondant
    DateRecup->setDate(CalendrierActif->selectedDate());
    disconnect(CalendrierActif);
    LabelStartDate->setVisible(true);
    LabelEndDate->setVisible(true);
    DateEditeStart->setVisible(true);
    DateEditeEnd->setVisible(true);
    ButtonStartDate->setVisible(true);
    ButtonEndDate->setVisible(true);
    CalendrierActif->setVisible(false);
    WidgetScrollArea->setFixedHeight(129);
    setMinimumWidth(110);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}
