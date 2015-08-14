#include "Compile.h"

//Initialisation du nombre d'instance
int Compilation::nbInstance = 0;
int Compilation::ActiveModeNum = 0;
Compilation *Compilation::ListeInstance[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

//Fonction qui renvois les donnée xml sous forme de QString
QString Compilation::DomDocument()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return doc->toString();
}

//Fonction pour ajouter un élément xml
QDomElement Compilation::addElement(QDomDocument *doc, QDomNode *node, const QString &tag, const QString &value = QString::null )
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
   QDomElement el = doc->createElement( tag );
   node->appendChild( el );
   if ( !value.isNull() ) {
       QDomText txt = doc->createTextNode( value );
       el.appendChild( txt );
   }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
   return el;
}

//Fonction pour ajouter un élément xml
QDomElement Compilation::addElement(QDomDocument *doc, QDomNode node, const QString &tag, const QString &value = QString::null )
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
   QDomElement el = doc->createElement( tag );
   node.appendChild( el );
   if ( !value.isNull() ) {
       QDomText txt = doc->createTextNode( value );
       el.appendChild( txt );
   }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
   return el;
}

//Fonction qui ajoute une condition horaire a un mode existant
bool Compilation::addCondition(CondHoraire *CondH, int ValeurID)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QDomElement element = doc->documentElement();
    for(QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        QDomElement qde = n.toElement();
        if (!n.isNull())
        {
            if (qde.tagName() == TAG_CONDITION)
            {
                if (qde.hasAttribute(ATTRIBUT_ID)) {
                    if (ValeurID == qde.attribute(ATTRIBUT_ID).toInt()){
                        delElement(doc, TAG_CONDITION, ATTRIBUT_ID, ValeurID);
                        break;
                    }
                }
            }
        }
    }
    QDomElement setcond = addElement(doc, element, TAG_CONDITION, QString::null);
    setcond.setAttribute(ATTRIBUT_ID, ValeurID);
    switch (CondH->Type) {
    case Periode:
        setcond.setAttribute(ATTRIBUT_TYPE, QString::number(Periode) + " Periode");
        break;
    case Journalier:
        setcond.setAttribute(ATTRIBUT_TYPE, QString::number(Journalier) + " Journalier");
        break;
    case Hebdomadaire:
        setcond.setAttribute(ATTRIBUT_TYPE, QString::number(Hebdomadaire) + " Hebdomadaire");
        break;
    case Vide:
        setcond.setAttribute(ATTRIBUT_TYPE, QString::number(Vide) + " Vide");
        break;
    case View:
        setcond.setAttribute(ATTRIBUT_TYPE, QString::number(View) + " View");
        break;
    default:
        break;
    }
    QDomElement setstart = addElement(doc, setcond, TAG_SARTTIME, QString::null);
    QDomElement setend = addElement(doc, setcond, TAG_ENDTIME, QString::null);
    if (CondH->DMois > 0) addElement(doc, setstart, TAG_MOIS, QString::number(CondH->DMois));
    if (CondH->DJour > 0) addElement(doc, setstart, TAG_JOUR, QString::number(CondH->DJour));
    if (CondH->DJourSem > 0) addElement(doc, setstart, TAG_JOURSEM, QString::number(CondH->DJourSem));
    addElement(doc, setstart, TAG_HEURE, QString::number(CondH->DHeure));
    addElement(doc, setstart, TAG_MIN, QString::number(CondH->DMinute));
    if (CondH->DMois > 0) addElement(doc, setend, TAG_MOIS, QString::number(CondH->EMois));
    if (CondH->DJour > 0) addElement(doc, setend, TAG_JOUR, QString::number(CondH->EJour));
    if (CondH->DJourSem > 0) addElement(doc, setend, TAG_JOURSEM, QString::number(CondH->EJourSem));
    addElement(doc, setend, TAG_HEURE, QString::number(CondH->EHeure));
    addElement(doc, setend, TAG_MIN, QString::number(CondH->EMinute));
    delete CondH;
    ControleCompilation();
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return true;
}

//Constructeur
Compilation::Compilation(QWidget *parent) : QWidget(parent)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Nouvelle Led témoin initialisation
    LedIni = new HLed;
    LedIni->setColor(QColor(255,0,0));
    LedIni->setFixedHeight(20);
    LedIni->setFixedWidth(20);
    LedIni->turnOff();
    //Nouvelle Led témoin Conditions Horaire
    LedCond = new HLed;
    LedCond->setColor(QColor(255,0,0));
    LedCond->setFixedHeight(20);
    LedCond->setFixedWidth(20);
    LedCond->turnOff();
    LedCond->setToolTip(tr("Conditions horaire programmée"));
    //Nouvelle Led témoin States
    LedState = new HLed;
    LedState->setColor(QColor(255,0,0));
    LedState->setFixedHeight(20);
    LedState->setFixedWidth(20);
    LedState->turnOff();
    //Définition d'une police et d'une palette de couleur pour les labels
    QFont font("Verdana", 9);
    font.setBold(true);
    //Label avec le nom
    LabelNom = new QLabel(tr("Nom à définir"));
    LabelNom->setFont(font);
    LabelNom->setStyleSheet("QLabel {color : black; }");
    //Label avec l'ID
    LabelId = new QLabel(tr("ID"));
    font.setPointSize(14);
    LabelId->setFont(font);
    LabelId->setStyleSheet("QLabel {color : black; }");
    //Layout Principal
    layoutAappliquer = new QVBoxLayout(this);
    layoutAappliquer->setContentsMargins(0,0,0,0);
    layoutAappliquer->setSpacing(1);
    //Layout horizontale titre
    ZoneTitre = new QHBoxLayout;
    ZoneTitre->setContentsMargins(1,1,1,1);
    ZoneTitre->setSpacing(1);
    ZoneTitre->setAlignment(Qt::AlignHCenter);
    //Layout horizontale Info
    ZoneInfo = new QHBoxLayout;
    ZoneInfo->setContentsMargins(1,1,1,1);
    ZoneInfo->setSpacing(1);

    //Application au widget
    setContentsMargins(2,2,2,2);
    layoutAappliquer->addLayout(ZoneTitre);
    layoutAappliquer->addLayout(ZoneInfo);
    ZoneTitre->addWidget(LabelNom);
    ZoneInfo->addWidget(LabelId);
    ZoneInfo->addWidget(LedIni);
    ZoneInfo->addWidget(LedCond);
    ZoneInfo->addWidget(LedState);
    setMinimumHeight(44);

    //Boite de dialogue pour sauvegarder le fichier Xml
    enregistrer = new QMessageBox(this);
    enregistrer->setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
    enregistrer->setIcon(QMessageBox::NoIcon);
    enregistrer->setModal(true);
    QPixmap Econi(":Books");
    enregistrer->setIconPixmap(Econi);

    //Boite de dialogue pour initialiser une instance
    NewMode = new NewModeDialog(this);

    Nomfichier = "";

    //Sauvegarde de l'instance
    ListeInstance[ (int) nbInstance] = this;
    Instance = nbInstance;
    nbInstance++;

    //Application des labels
    retranslate("fr");

    //Lorsque le fenetre de dialogue qui permet de parametre un mode envois un signal c'est qu'elle transmet alors ses infos a l'objet compile.
    connect(NewMode, SIGNAL(BoutonNouveau(QString,QString,QString,int,int)), this, SLOT(initialisationNouveauDom(QString,QString,QString,int,int)));
    connect(NewMode, SIGNAL(rejected()), this, SLOT(initialisationFichierDom()));
    connect(NewMode, SIGNAL(AdapterID()), this, SLOT(ProposeID()));
    connect(NewMode, SIGNAL(BoutonValider(QString,QString,int,int)), this, SLOT(UpdateDom(QString,QString,int,int)));
    connect(this, SIGNAL(AfficheNewMode(QString,QString,int,int,QStringList)), NewMode, SLOT(UpdateAndShow(QString,QString,int,int, QStringList)));
    connect(this, SIGNAL(IDproposer(int)), NewMode, SLOT(IDAdapter(int)));

    //Création de la base de l'arbre Xml
    doc = new QDomDocument("Light_Effect_Crib");
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

Compilation::~Compilation()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
        delete NewMode;

        QFile file(Nomfichier);
        if (!saveAuto) {
            if (!file.exists())
                enregistrer->setInformativeText(tr("<font color=\"#FF2A2A\"><b>Xml file:</b> %1</font>").arg(file.fileName()));
            else
                enregistrer->setInformativeText(tr("<font color=\"#FF2A2A\">Le fichier Xml:<br><b> %1 </b><br><font align=\"left\">existe déjà</font></font><br>The systeme is going to replace it").arg(file.fileName()));
            if (enregistrer->exec() == QMessageBox::Save)
            {
                file.open(QIODevice::WriteOnly | QIODevice::Text);
                QTextStream out(&file);
                out << DomDocument();
                file.close();
            }
        }
        else {
            if (!file.exists()) {
                //Le fichier est créé et sauvegarder quand il n'existe pas
                file.open(QIODevice::WriteOnly | QIODevice::Text);
                QTextStream out(&file);
                out << DomDocument();
                file.close();
            }
            else {
                //Le fichier est sauvegardé avec modification de nom de fichier
                Nomfichier.replace(".xml","_AutoSave.xml", Qt::CaseInsensitive);
                file.setFileName(Nomfichier);
                file.open(QIODevice::WriteOnly | QIODevice::Text);
                QTextStream out(&file);
                out << DomDocument();
                file.close();
            }
        }
    delete doc;
    if (!NewMode)
        delete NewMode;
    ListeInstance[Instance] = NULL;
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

void Compilation::initialisationNouveauDom(QString Nom, QString Description, QString FichierNom, int idMode, int priorite)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    LabelNom->setText(Nom);
    LabelId->setText("-" + QString::number(idMode) + "-");
    //Arbre Xml implémenté
    Nomfichier = FichierNom;
    if (!Nomfichier.contains(".xml"))
        if (!Nomfichier.contains(".XML"))
            if (!Nomfichier.contains(".Xml"))
                Nomfichier.append(".Xml");
    QDomProcessingInstruction instr = doc->createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
    doc->appendChild(instr);
    QDomComment comm = doc->createComment(tr("Fichier avec le détail des effets lumineux programmé pour la creche de l'Eglise Saint Joseph"));
    doc->appendChild(comm);
    QDomElement setmode = addElement(doc, doc, TAG_MODE , QString::null);
    addElement(doc, &setmode, TAG_PRIORITY, QString::number(priorite));
    addElement(doc, &setmode, TAG_ID, QString::number(idMode));
    addElement(doc, &setmode, TAG_NOM , Nom);
    addElement(doc, &setmode, TAG_DESCRIPTION , Description);
    addElement(doc, &setmode, TAG_DATA, QString::null);
    QStringList ListeDesModules = NewMode->askeModules();
    foreach (QString Module, ListeDesModules) {
        addElement(doc, &setmode, TAG_MODULE, Module);
    }
    setToolTip(Description);
    //On indique à NewMode la fin de l'initialisation et on la cache
    NewMode->FinInitialisation();
    NewMode->hide();

    //Controle et application des couleurs aux led en fonction
    ControleCompilation();

#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

void Compilation::initialisationFichierDom()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    if(!NewMode->InitialisationMode())
    {
        Nomfichier = QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier 'Effet lumineux Creche' "), "./", tr("Xml Files (*.Xml);;Text Files (*.txt);;All Files (*.*)"));
        if (Nomfichier == NULL){
            showNewModeDialog();
        }
        else
        {
            //On indique à NewMode la fin de l'initialisation et on la cache
            NewMode->FinInitialisation();
            //On cache la fenètre de paramétrisation
            NewMode->hide();
            //On controle le fichier a ouvrir
            if (!Nomfichier.contains(".xml"))
                if (!Nomfichier.contains(".XML"))
                    if (!Nomfichier.contains(".Xml"))
                        Nomfichier.append(".Xml");

            QFile* file = new QFile(Nomfichier);
            /* If we can't open it, let's show an error message. */
            if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
                QMessageBox::critical(this, tr("Erreur a l'ouverture du fichier"),tr("Une erreur est survenue lors de l'ouverture du fichier."),QMessageBox::Ok);
                delete enregistrer;
                emit DeleteMe(this);
            }

            if (!doc->setContent(file))
            {
                QMessageBox::critical(this, tr("Erreur a la lecture du fichier"),tr("Une erreur est survenue pendant la lecture du fichier."),QMessageBox::Ok);
                delete enregistrer;
                emit DeleteMe(this);
            }
            file->close();
            delete file;
            //Attribution d'un nouvel id
            delElement(doc, TAG_ID);
            //Extraction du nom du mode
            QDomElement element = doc->documentElement();
            addElement(doc, element, TAG_ID, QString::number(NouvelId()));
            for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
            {
                //Prend le nom et vérifie l'initialisation du mode
                if (qde.tagName() == TAG_NOM)
                {
                    LabelNom->setText(qde.text());
                }
                //Prend l'id et vérifie l'initialisation du mode
                if (qde.tagName() == TAG_ID)
                {
                    LabelId->setText("-" + qde.text() + "-");
                }
                //Extraction de la description
                {
                    if (qde.tagName() == TAG_DESCRIPTION)
                        setToolTip(qde.text());
                }
            }
            //Si le fichier provient d'une sauvegarde automatique, on adapte le nom du fichier
            if(Nomfichier.contains("AutoSave")){
                if(Nomfichier.contains("_AutoSave.xml"))
                    Nomfichier.replace("_AutoSave.xml",".xml");
                if(Nomfichier.contains("AutoSave.xml"))
                    Nomfichier.replace("AutoSave.xml",".xml");
                if(Nomfichier.contains("_AutoSave"))
                    Nomfichier.replace("_AutoSave","_");
                if(Nomfichier.contains("AutoSave"))
                    Nomfichier.replace("AutoSave","_");
            }
            //Controle et application des couleurs aux led en fonction
            ControleCompilation();
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui ajoute un state dans l'arbre des états mais pas les entrées du state, et renvois l'id + 1 si le state est bien créé
bool Compilation::addkey(int led, QString Module, int pause, LedType Type, QString param)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    bool test = false;
    QDomElement element = doc->documentElement();
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        if (qde.tagName() == TAG_DATA)
        {
            for(QDomElement qdea = qde.firstChildElement(); !qdea.isNull(); qdea = qdea.nextSiblingElement())
            {
                if (qdea.tagName() == TAG_LED && qdea.attribute(ATTRIBUT_MODULE) == Module && qdea.attribute(ATTRIBUT_ID).toInt() == led)
                {
                    int ID = 0;
                    for(QDomElement qdeab = qdea.firstChildElement(); !qdeab.isNull(); qdeab = qdeab.nextSiblingElement())
                    {
                        if (qdeab.tagName() == TAG_STATE && qdeab.attribute(ATTRIBUT_ID).toInt() > ID)
                        {
                            ID = qdeab.attribute(ATTRIBUT_ID).toInt();
                        }
                    }
                    //ID prend le numéro de la key que l'on va ajouter maintenant
                    ID++;
                    QDomElement setkey = addElement(doc, qdea, TAG_STATE, QString::null);
                    setkey.setAttribute(ATTRIBUT_ID, ID);
                    addElement(doc,setkey, TAG_PAUSE, QString::number(pause));
                    switch (Type) {
                    case TOR :
                        setkey.setAttribute(ATTRIBUT_TYPE, QString::number(Type) + " TOR");
                        break;
                    case Progressif :
                        setkey.setAttribute(ATTRIBUT_TYPE, QString::number(Type) + " Progressif");
                        break;
                    case ProgressifDebut :
                        setkey.setAttribute(ATTRIBUT_TYPE, QString::number(Type) + " ProgressifDebut");
                        break;
                    case ClignotantFranc :
                        setkey.setAttribute(ATTRIBUT_TYPE, QString::number(Type) + " ClignotantFranc");
                        break;
                    case ClignotantProgr :
                        setkey.setAttribute(ATTRIBUT_TYPE, QString::number(Type) + " ClignotantProgr");
                        break;
                    case Vacillement :
                        setkey.setAttribute(ATTRIBUT_TYPE, QString::number(Type) + " Vacillement");
                        break;
                    }
                    addElement(doc,setkey, TAG_PARAMETRE, param);
                    test = true;
                }
            }
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return test;
}

//Fonction qui ajoute une led dans l'arbre des données
bool Compilation::addLed(int id, QString Module, const QString &Description)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    bool test = false;
    QDomElement element = doc->documentElement();
    //Controle du module référencé
    for(QDomElement qqde = element.firstChildElement(); !qqde.isNull(); qqde = qqde.nextSiblingElement())
    {
        if (qqde.tagName() == TAG_MODULE)
        {
            if (qqde.text() == Module){
                //Ajout de la led indiquée
                for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
                {
                    if (qde.tagName() == TAG_DATA)
                    {
                        QDomElement setled = addElement(doc, qde, TAG_LED, QString::null);
                        setled.setAttribute(ATTRIBUT_ID, id);
                        setled.setAttribute(ATTRIBUT_MODULE, Module);
                        addElement(doc, setled, TAG_DESCRIPTION, Description);
                        test = true;
                    }
                }
                break;
            }
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return test;
}

//Fonction qui renvois une valeur ID pour le mode, proposition faite à l'utilisateur,
int Compilation::NouvelId()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    int valeur = -1;
    for (int i = 0; i < nbInstance; i++)
    {
        if (ListeInstance[i] != NULL){
            QDomElement element = ListeInstance[i]->doc->documentElement();
            for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
            {
                if (qde.tagName() == TAG_ID)
                {
                    if (qde.text().toInt() > valeur)
                        valeur = qde.text().toInt();
                }
            }
        }
    }
    valeur++;
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return valeur;
}

//C'est un slot qui renvois un signal avec une valeur ID en réponse
void Compilation::ProposeID()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    emit IDproposer(NouvelId());
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui retourne le nombre d'instance en cours d'execution
int Compilation::NbInstance()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    int valeur = 0;
    for (int i = 0; i < nbInstance; i++)
    {
        if (ListeInstance[i] != NULL){
            valeur++;
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return valeur;
}

//Fonction qui renvois un pointeur vers une instance valide
Compilation* Compilation::InstanceActive()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    for (int i = 0; i < nbInstance; i++)
    {
        if (ListeInstance[i] != NULL){
            return ListeInstance[i];
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return NULL;
}

//Slot qui rend visible la fenetre de parametrisation d'une nouvelles class
void Compilation::showNewModeDialog()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    NewMode->open();
    NewMode->setModal(true);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction Globale pour les const QString
QString Compilation::Const_QString(int type)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Variable globale pour les const QString
    static const char* TitreEtConstante[] = {
       QT_TR_NOOP("   "),
       QT_TR_NOOP("Vide")
    };
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return tr(TitreEtConstante[type]);
}

//Fonction qui ajoute une validation de synchronisation a un state donné (Maximum 4 validations / mode et 1 synchronisation / mode)
bool Compilation::addValidation(int idState, bool Validation, bool Synchronisation)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */

#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui retourne la valeur de l'instance
int Compilation::InstanceValue()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return Instance;
}

//Fonction qui renvois un pointeur vers l'instance d'indice x
Compilation *Compilation::ReturnInstance(int x)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    for (int i = 0; i <= nbInstance; i++)
    {
        if (ListeInstance[i] != NULL){
            if (ListeInstance[i]->Instance == x)
            {
#ifdef DEBUG_COMANDSAVE
                std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
                return ListeInstance[i];
            }
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return NULL;
}

//Fonction surchargée, d'évènement souris
void Compilation::mouseDoubleClickEvent(QMouseEvent *e)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    if (ActiveModeNum == Instance){
        emit AfficheNewMode(InstanceNom(), InstanceDescription(), InstanceIndice(), InstancePriorite(), InstanceModules());
    }
    else
        emit MouseClickEvent(Instance);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui allume et eteint les LED
void Compilation::TurneLedOn(bool On)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
        LedCond->turnOn(On);
        LedIni->turnOn(On);
        LedState->turnOn(On);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui dessine une bordure autour du widget
void Compilation::paintEvent(QPaintEvent * /*event*/)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Pour dessiner un rectangle autour du widget, a faire lors d'un QpaintEvent
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QSize Taille = size();
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(Qt::blue));
    //On construit le rectangle extérieur = rectangle intérieur + bordure
    p.drawRect(-2, //x
               -2,   //y
               Taille.width() + 2,  //width
               Taille.height() + 2); //height
    p.setBrush(QBrush(Qt::lightGray));
    p.drawRect(2, //x
               2,   //y
               Taille.width() - 4,  //width
               Taille.height() - 4); //height
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction de controle, returne true, ssi toutes les test sont positif
//A COMPLETER
bool Compilation::ControleCompilation()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    bool test;
    ControleInstance = true;
    QDomElement element = doc->documentElement();
    int cond = 0;
    bool prio = false, crtId = false;
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        //Controle sur les condition Horaire
        if (qde.tagName() == TAG_CONDITION && qde.hasAttribute(ATTRIBUT_TYPE))
        {
            for(QDomElement dataElement = qde.firstChildElement(); !dataElement.isNull(); dataElement = dataElement.nextSiblingElement())
            {
                if (dataElement.tagName() == TAG_SARTTIME)
                    cond++;
                if (dataElement.tagName() == TAG_ENDTIME)
                    cond++;
            }
        }
        if (qde.tagName() == TAG_PRIORITY){
            prio = true;
        }
        if (qde.tagName() == TAG_ID){
            crtId = true;
        }
    }
    //Début du test des conditions horaire
    if (cond > 0)
    {
        if (cond % 2 == 0) {
            //Il y a au moin une condition valable
            LedCond->setColor(QColor(0,231,41));
            test = true;
        }
        else {
            LedCond->setColor(QColor(255,109,0));
            test = false;
        }
    }
    else {
        LedCond->setColor(QColor(180,0,0));
        test = false;
    }
    //Fin du test condition horaire
    if (ControleInstance)
        ControleInstance = test;
    //Controle sur les modules
    test = true;
    QStringList liste = InstanceModules();
    //Teste les propriétés  priorité et Id
    if (liste.contains("M01") && liste.contains("M04") && prio && crtId) { //M01 et M04 sont indispensable au fonctionnement du système
        //M02 et M03 doivent être présent en même temps ou tous les deux absent
        if (liste.contains("M02")) {
            if (!liste.contains("M03"))
                test = false;
        }
        if (liste.contains("M03")) {
            if (!liste.contains("M02"))
                test = false;
        }
    }
    else
        test = false;
    if (test) //Led Initialisation turne to green
        LedIni->setColor(QColor(83,221,0));
    else //Led turne to red
        LedIni->setColor(QColor(255, 0, 0));
    if (ControleInstance)
        ControleInstance = test;
    //Teste sur les leds...
    emit CompilationUpdated();
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Demande de lecture des conditions horaire
void Compilation::DemndeLectureCH(CondHoraire *CondH, int indice)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QDomElement element = doc->documentElement();
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        if (qde.tagName() == TAG_CONDITION && qde.attribute(ATTRIBUT_ID).toInt() == indice)
        {
            if (qde.hasAttribute(ATTRIBUT_TYPE)) {
                switch (qde.attribute(ATTRIBUT_TYPE).mid(0,1).toInt()) {
                case Periode:
                    CondH->Type = Periode;
                    break;
                case Journalier:
                    CondH->Type = Journalier;
                    break;
                case Hebdomadaire:
                    CondH->Type = Hebdomadaire;
                    break;
                case Vide:
                    CondH->Type = Vide;
                    break;
                case View:
                    CondH->Type = View;
                    break;
                default:
                    break;
                }
            }
            for(QDomElement qdea = qde.firstChildElement(); !qdea.isNull(); qdea = qdea.nextSiblingElement())
            {
                if (qdea.tagName() == TAG_SARTTIME)
                {
                    for(QDomElement qdeac = qdea.firstChildElement(); !qdeac.isNull(); qdeac = qdeac.nextSiblingElement())
                    {
                        if (qdeac.tagName() == TAG_MOIS)
                            CondH->DMois = qdeac.text().toInt();
                        if (qdeac.tagName() == TAG_JOUR)
                            CondH->DJour = qdeac.text().toInt();
                        if (qdeac.tagName() == TAG_JOURSEM)
                            CondH->DJourSem = qdeac.text().toInt();
                        if (qdeac.tagName() == TAG_HEURE)
                            CondH->DHeure = qdeac.text().toInt();
                        if (qdeac.tagName() == TAG_MIN)
                            CondH->DMinute = qdeac.text().toInt();
                    }
                }
                if (qdea.tagName() == TAG_ENDTIME)
                {
                    for(QDomElement qdeac = qdea.firstChildElement(); !qdeac.isNull(); qdeac = qdeac.nextSiblingElement())
                    {
                        if (qdeac.tagName() == TAG_MOIS)
                            CondH->EMois = qdeac.text().toInt();
                        if (qdeac.tagName() == TAG_JOUR)
                            CondH->EJour = qdeac.text().toInt();
                        if (qdeac.tagName() == TAG_JOURSEM)
                            CondH->EJourSem = qdeac.text().toInt();
                        if (qdeac.tagName() == TAG_HEURE)
                            CondH->EHeure = qdeac.text().toInt();
                        if (qdeac.tagName() == TAG_MIN)
                            CondH->EMinute = qdeac.text().toInt();
                    }
                }
            }
        }
    }
    //On emet toujour le signal de check UpDate Pour que l'affichage soit adapté
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Demande de lecture des conditions horaire
void Compilation::DemndeLectureCH(QPlainTextEdit *CondH)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    bool test = false;
    QString str;
    QTextStream stream(&str);
    QDomElement element = doc->documentElement();
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        if (qde.tagName() == TAG_CONDITION)
        {
            qde.save(stream, 1);
            //qDebug() << "Return cond Horaire" << str;
            test = true;
        }
    }
    if (test) {
        //On peut modifier le contenu du plainTextEdit
        CondH->clear();
        CondH->insertPlainText(str);
    }
    else {
        CondH->clear();
        CondH->insertPlainText(Const_QString(1));
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui supprime les condition Horaire
void Compilation::SupprimeCondHoraire(int ValeurID)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    delElement(doc, TAG_CONDITION, ATTRIBUT_ID, ValeurID);
    QDomElement element = doc->documentElement();
    ValeurID = 0;
    for (int i = 0; i < 11; i++) {
        for(QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling())
        {
            QDomElement qde = n.toElement();
            if (!n.isNull())
            {
                if (qde.tagName() == TAG_CONDITION)
                {
                    if (qde.hasAttribute(ATTRIBUT_ID)) {
                        if (i == qde.attribute(ATTRIBUT_ID).toInt()){
                            QDomAttr AttributID = qde.attributeNode(ATTRIBUT_ID);
                            AttributID.setValue(QString::number(ValeurID++));
                            break;
                        }
                    }
                }
            }
        }
    }
    ControleCompilation();
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui enlève un élément de l'arbre xml
void Compilation::delElement( QDomDocument *doc, const QString &tag, const QString &attribut, int val)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QDomElement Element = doc->documentElement();
    for(QDomNode node = Element.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        QDomElement element = node.toElement();
        if (element.tagName() == tag )
        {
#ifdef DEBUG_COMANDSAVE
            qDebug() << attribut << val << element.hasAttribute(attribut) << element.attribute(attribut).toInt();
#endif /* DEBUG_COMANDSAVE */
            if (attribut == QString::null || (element.hasAttribute(attribut) && element.attribute(attribut).toInt() == val))
            {
                Element.removeChild(node);
#ifdef DEBUG_COMANDSAVE
                qDebug() << "Condition remove OK";
#endif /* DEBUG_COMANDSAVE */
            }
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui applique et réapplique les labels pour introduire leur traduction quand c'est nécessaire
void Compilation::retranslate(QString lang)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    LedIni->setToolTip(tr("Initialisation du mode"));
    LedCond->setToolTip(tr("Conditions horaire programmée"));
    LedState->setToolTip(tr("Effet lumineux (1 ou plus)"));
    if (enregistrer) {
        enregistrer->setWindowTitle(tr("Enregistrer fichier Xml", "Titre fenetre"));
        enregistrer->setText(tr("<font color=\"#FF2A2A\">Voulez-vous sauvegarder le fichier suivant:<font>"));
        enregistrer->setButtonText(QMessageBox::Save,tr("Enregistrer"));
        enregistrer->setButtonText(QMessageBox::Discard, tr("Ne pas enregistrer"));
    }
    LabelId->setToolTip(tr("ID attribué à ce mode pour un fonctionnement multi mode"));
 //   if (NewMode)
 //       NewMode->retranslate(lang);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction pour la compilation des conditions Horaire
bool Compilation::CompilationCH(QString NomVaribleTest, DonneFichier *DataToFill, TableUsedRAM *TableRAM)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //définition du nom du fichier
    DataToFill->ModeNom = "C";
    DataToFill->ModeNom.append(QString::number(Instance));
    QString Commande;
    //Création des variables test de condition
    DataToFill->ListeIstruction.append("<M01M01ERM01VAA" + AddToRamTable(TableRAM, QString("M01VA")) + "V0000>");
    int indice = 0;
    //On récupère les données dans la structure adéquate, pour façiliter les opérations
    CondHoraire CondH;
    DataToFill->ListeIstruction.append("<M01M01ER" + NomVaribleTest + "A" + AddToRamTable(TableRAM, NomVaribleTest) + "V0000>");
    //Si on a des valeurs retournée
    while (indice < 10) {
        CondH = {0,0,0,0,0,0,0,0,0,0};
        CondH.Type = Vide;
        DemndeLectureCH(&CondH, indice++);
        if (CondH.Type == Vide)
            break;
        switch (CondH.Type) {
        case Periode:
            //Variable de test
            DataToFill->ListeIstruction.append("<M01M01ERM01CAA" + AddToRamTable(TableRAM, QString("M01CA")) + "V0000>");
            //Ajout de la condition sur la date
            CompilationDate(&CondH, DataToFill, TableRAM);
            //Ajout de la condition sur l"heure (de tel heure à tel heure pendant toute la période)
            CompilationHeure(&CondH, DataToFill, TableRAM);
            //Controle du test
            DataToFill->ListeIstruction.append("<M01M01RG" + AddToRamTable(TableRAM, QString("M01CA")) + "DV0002L?R+2>");
            //Validation du test
            DataToFill->ListeIstruction.append("<M01M01ERM01CBA" + AddToRamTable(TableRAM, QString("M01CB")) + "V0001>");
            break;
        case Journalier:
            DataToFill->ListeIstruction.append("<M01M01ERM01CAA" + AddToRamTable(TableRAM, QString("M01CA")) + "V0000>");
            //Ajout de la condition sur l"heure (de tel heure à tel heure pendant toute la période)
            CompilationHeure(&CondH, DataToFill, TableRAM);
            //Controle du test
            DataToFill->ListeIstruction.append("<M01M01RG" + AddToRamTable(TableRAM, QString("M01CA")) + "DV0001L?R+2>");
            //Validation du test
            DataToFill->ListeIstruction.append("<M01M01ERM01CBA" + AddToRamTable(TableRAM, QString("M01CB")) + "V0001>");
            break;
        case Hebdomadaire:
            DataToFill->ListeIstruction.append("<M01M01ERM01CAA" + AddToRamTable(TableRAM, QString("M01CA")) + "V0000>");
            //Ajout de la condition sur le jour de la semaine
            CompilationSem(&CondH, DataToFill, TableRAM);
            //Ajout de la condition sur l"heure (de tel heure à tel heure pendant toute la période)
            CompilationHeure(&CondH, DataToFill, TableRAM);
            //Controle du test
            DataToFill->ListeIstruction.append("<M01M01RG" + AddToRamTable(TableRAM, QString("M01CA")) + "DV0002L?R+2>");
            //Validation du test
            DataToFill->ListeIstruction.append("<M01M01ERM01CBA" + AddToRamTable(TableRAM, QString("M01CB")) + "V0001>");
            break;
        default:
            break;
        }
    }
    //Finalisation des tests des CH
    DataToFill->ListeIstruction.append("<M01M01RF" + AddToRamTable(TableRAM, QString("M01CB")) + "DV0001F00>");
    DataToFill->ListeIstruction.append("<M01M01RO" + AddToRamTable(TableRAM, NomVaribleTest) + "PV0001>");
    DataToFill->ListeIstruction.append("<M01M01RF" + AddToRamTable(TableRAM, NomVaribleTest) + "EV0001F00>");
#ifdef DEBUG_ARDUINO
    Commande = ("<M01M01Z C" + QString::number(Instance) + " erreur et stop prog>");
    Commande.append(QString::number(DataToFill->Commentaire.size()));
    DataToFill->ListeIstruction.append(Commande);
    DataToFill->Commentaire.insert(Commande, tr("Stop de l'executable en cas d'erreur"));
    DataToFill->ListeIstruction.append("<M01M01RA0>");
#else
    Commande = "<M01M01RA0>";
    Commande.append(QString::number(DataToFill->Commentaire.size()));
    DataToFill->ListeIstruction.append(Commade);
    DataToFill->Commentaire.insert(Commande, tr("Stop de l'executable en cas d'erreur"))
#endif

#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui retourne le nombre de jours d'un mois donné
int Compilation::nbjourmois(int mois){
  if (mois == 1 || mois == 3 || mois == 5 || mois == 7 || mois == 8 || mois == 10 || mois == 12) return 31;
  if (mois == 4 || mois == 6 || mois == 9 || mois == 11) return 30;
  if (mois == 2) return 28;
  return 0;
}

//Fonction pour ajouter ou retourner une adresse RAM
QString Compilation::AddToRamTable(TableUsedRAM *TableRAM, QString Data)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    if (Data.length() != 5)
        return QString("EEEE");
    QString RamAdresse = TableRAM->value(Data);
    if (RamAdresse.isEmpty()) {
        TableRAM->insert(Data, IntToQString(TableRAM->count() + 1));
        RamAdresse = TableRAM->value(Data);
    }
    return RamAdresse;
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui retourne l'état actuel du controle de compilation
bool Compilation::onControleCompilation()
{
    return ControleInstance;
}

//Fonction qui converti un nombre en Qstring de 4 carractères
QString Compilation::IntToQString(int Value)
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

//Fonction qui compile une condition sur la date
void Compilation::CompilationDate(CondHoraire *CondH, DonneFichier *DataToFill, TableUsedRAM *TableRAM)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Date actuelle
    QString Commande = "<M01M01LDX" + AddToRamTable(TableRAM, QString("M01DX")) + ">";
    //On ajoute un numéro unique qui n'est pas interprété pour unifier la commande
    Commande.append(QString::number(DataToFill->Commentaire.size()));
    DataToFill->ListeIstruction.append(Commande);
    DataToFill->Commentaire.insert(Commande, tr("Condition sur la date :") + QString::number(CondH->DJour) + "/" + QString::number(CondH->DMois) + tr(" to ") + QString::number(CondH->EJour) + "/" + QString::number(CondH->EMois));
    //Date de début
    for (int i = 1; i < CondH->DMois; i ++)
        CondH->DJour += nbjourmois(i);
    DataToFill->ListeIstruction.append("<M01M01ERM01AAA" + AddToRamTable(TableRAM, QString("M01AA"))+ "V" + IntToQString(CondH->DJour) + ">");
    //Date de fin
    for (int i = 1; i < CondH->EMois; i ++)
        CondH->EJour += nbjourmois(i);
    DataToFill->ListeIstruction.append("<M01M01ERM01ABA" + AddToRamTable(TableRAM, QString("M01AB"))+ "V" + IntToQString(CondH->EJour) + ">");
    //Test sur le dépassement
    DataToFill->ListeIstruction.append("<M01M01RG" + AddToRamTable(TableRAM, QString("M01AB")) + "SA" + AddToRamTable(TableRAM, QString("M01AA")) + "L?R+4>");
    //Ajoute 1 année sur la date de fin
    DataToFill->ListeIstruction.append("<M01M01RO" + AddToRamTable(TableRAM, QString("M01AB")) + "PV016D>");
    //Test sur le dépassement
    DataToFill->ListeIstruction.append("<M01M01RG" + AddToRamTable(TableRAM, QString("M01DX")) + "SA" + AddToRamTable(TableRAM, QString("M01AA")) + "L?R+2>");
    //Ajoute 1 année sur la date actuel
    DataToFill->ListeIstruction.append("<M01M01RO" + AddToRamTable(TableRAM, QString("M01DX")) + "PV016D>");
    //1er Test sur la date actuel
    DataToFill->ListeIstruction.append("<M01M01RG" + AddToRamTable(TableRAM, QString("M01DX")) + "IA" + AddToRamTable(TableRAM, QString("M01AA")) + "L?R+3>");
    //2eme Test sur la date actuel
    DataToFill->ListeIstruction.append("<M01M01RG" + AddToRamTable(TableRAM, QString("M01DX")) + "SA" + AddToRamTable(TableRAM, QString("M01AB")) + "L?R+2>");
    //Test validé, on enregistre le résultat
    DataToFill->ListeIstruction.append("<M01M01RO" + AddToRamTable(TableRAM, QString("M01CA")) + "PV0001>");
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui compile une condition sur l'heure
void Compilation::CompilationHeure(CondHoraire *CondH, DonneFichier *DataToFill, TableUsedRAM *TableRAM)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Date actuelle
    QString Commande = "<M01M01LAZ" + AddToRamTable(TableRAM, QString("M01AZ")) + ">";
    //On ajoute un numéro unique qui n'est pas interprété pour unifier la commande
    Commande.append(QString::number(DataToFill->Commentaire.size()));
    DataToFill->ListeIstruction.append(Commande);
    DataToFill->Commentaire.insert(Commande, tr("Condition sur l'heure :") + QString::number(CondH->DHeure) + ":" + QString::number(CondH->DMinute) + tr(" to ") + QString::number(CondH->EHeure) + ":" + QString::number(CondH->EMinute));
    //Heure de début
    CondH->DMinute += CondH->DHeure * 60;
    DataToFill->ListeIstruction.append("<M01M01ERM01AEA" + AddToRamTable(TableRAM, QString("M01AE"))+ "V" + IntToQString(CondH->DMinute) + ">");
    //Heure de fin
    CondH->EMinute += CondH->EHeure * 60;
    DataToFill->ListeIstruction.append("<M01M01ERM01AFA" + AddToRamTable(TableRAM, QString("M01AF"))+ "V" + IntToQString(CondH->EMinute) + ">");
    //Test sur le dépassement
    DataToFill->ListeIstruction.append("<M01M01RG" + AddToRamTable(TableRAM, QString("M01AF")) + "SA" + AddToRamTable(TableRAM, QString("M01AE")) + "L?R+4>");
    //Ajoute 1 jour sur l'heure de fin
    DataToFill->ListeIstruction.append("<M01M01RO" + AddToRamTable(TableRAM, QString("M01AF")) + "PV05A0>");
    //Test sur le dépassement
    DataToFill->ListeIstruction.append("<M01M01RG" + AddToRamTable(TableRAM, QString("M01AZ")) + "SA" + AddToRamTable(TableRAM, QString("M01AE")) + "L?R+2>");
    //Ajoute 1 jour sur l'heure actuel
    DataToFill->ListeIstruction.append("<M01M01RO" + AddToRamTable(TableRAM, QString("M01AZ")) + "PV05A0>");
    //1er Test sur l'heure actuel
    DataToFill->ListeIstruction.append("<M01M01RG" + AddToRamTable(TableRAM, QString("M01AZ")) + "IA" + AddToRamTable(TableRAM, QString("M01AE")) + "L?R+3>");
    //2eme Test sur l'heure actuel
    DataToFill->ListeIstruction.append("<M01M01RG" + AddToRamTable(TableRAM, QString("M01AZ")) + "SA" + AddToRamTable(TableRAM, QString("M01AF")) + "L?R+2>");
    //Test validé, on enregistre le résultat
    DataToFill->ListeIstruction.append("<M01M01RO" + AddToRamTable(TableRAM, QString("M01CA")) + "PV0001>");
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui compile une condition sur les jours de la semaine
void Compilation::CompilationSem(CondHoraire *CondH, DonneFichier *DataToFill, TableUsedRAM *TableRAM)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Date actuelle
    QString Commande = "<M01M01LDW" + AddToRamTable(TableRAM, QString("M01DW")) + ">";
    //On ajoute un numéro unique qui n'est pas interprété pour unifier la commande
    Commande.append(QString::number(DataToFill->Commentaire.size()));
    DataToFill->ListeIstruction.append(Commande);
    DataToFill->Commentaire.insert(Commande, tr("Condition sur le jour de la semaine :") + QString::number(CondH->DJourSem) + tr(" to ") + QString::number(CondH->EJourSem));
    //semaine de début
    DataToFill->ListeIstruction.append("<M01M01ERM01AIA" + AddToRamTable(TableRAM, QString("M01AI"))+ "V" + IntToQString(CondH->DJourSem) + ">");
    //semaine de fin
    DataToFill->ListeIstruction.append("<M01M01ERM01AJA" + AddToRamTable(TableRAM, QString("M01AJ"))+ "V" + IntToQString(CondH->EJourSem) + ">");
    //Test sur le dépassement
    DataToFill->ListeIstruction.append("<M01M01RG" + AddToRamTable(TableRAM, QString("M01AJ")) + "SA" + AddToRamTable(TableRAM, QString("M01AI")) + "L?R+4>");
    //Ajoute 1 semaine sur la semaine de fin
    DataToFill->ListeIstruction.append("<M01M01RO" + AddToRamTable(TableRAM, QString("M01AJ")) + "PV0007>");
    //Test sur le dépassement
    DataToFill->ListeIstruction.append("<M01M01RG" + AddToRamTable(TableRAM, QString("M01DW")) + "SA" + AddToRamTable(TableRAM, QString("M01AI")) + "L?R+2>");
    //Ajoute 1 semaine sur la semaine actuel
    DataToFill->ListeIstruction.append("<M01M01RO" + AddToRamTable(TableRAM, QString("M01DW")) + "PV0007>");
    //1er Test sur la semaine actuel
    DataToFill->ListeIstruction.append("<M01M01RG" + AddToRamTable(TableRAM, QString("M01DW")) + "IA" + AddToRamTable(TableRAM, QString("M01AI")) + "L?R+3>");
    //2eme Test sur la semaine actuel
    DataToFill->ListeIstruction.append("<M01M01RG" + AddToRamTable(TableRAM, QString("M01DW")) + "SA" + AddToRamTable(TableRAM, QString("M01AJ")) + "L?R+2>");
    //Test validé, on enregistre le résultat
    DataToFill->ListeIstruction.append("<M01M01RO" + AddToRamTable(TableRAM, QString("M01CA")) + "PV0001>");
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Retourne la priorité d'un mode
int Compilation::InstancePriorite()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QDomElement element = doc->documentElement();
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        if (qde.tagName() == TAG_PRIORITY)
        {
            return qde.text().toInt();
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return 0;
}

//Retourne L'indice d'un mode
int Compilation::InstanceIndice()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QDomElement element = doc->documentElement();
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        if (qde.tagName() == TAG_ID)
        {
            return qde.text().toInt();
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return 0;
}

//Retourne le nom d'un mode
QString Compilation::InstanceNom()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QDomElement element = doc->documentElement();
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        if (qde.tagName() == TAG_NOM)
        {
            return qde.text();
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return QString::null;
}

//Retourne La description d'un mode
QString Compilation::InstanceDescription()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QDomElement element = doc->documentElement();
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        if (qde.tagName() == TAG_DESCRIPTION)
        {
            return qde.text();
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return QString::null;
}

//Fonction pour modifier le mode actif
void Compilation::setModeActif(int ModeActive)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    ActiveModeNum = ModeActive;
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Renvois le mode actif
int Compilation::ModeActif()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return ActiveModeNum;
}

//Slot pour appliquer les modifications provenant de newmodedialog
void Compilation::UpdateDom(QString Nom, QString Description, int idMode, int priorite)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QDomElement el = doc->documentElement();
    delElement(doc, TAG_ID);
    addElement(doc, &el, TAG_ID, QString::number(idMode));
    delElement(doc, TAG_PRIORITY);
    addElement(doc, &el, TAG_PRIORITY, QString::number(priorite));
    delElement(doc, TAG_NOM);
    addElement(doc, &el, TAG_NOM, Nom);
    delElement(doc, TAG_DESCRIPTION);
    addElement(doc, &el, TAG_DESCRIPTION, Description);
    //Pareillement pour la liste des modules
    QStringList ListeDesModules = NewMode->askeModules();
    //On supprime les modules existants du DOM
    for (int i = 0; i < 15; i++)
        delElement(doc, TAG_MODULE);
    //On vient remettre la liste des modules demandés
    foreach (QString Module, ListeDesModules)
        addElement(doc, &el, TAG_MODULE, Module);
    //On cache alors la fenêtre
    NewMode->hide();
    LabelNom->setText(Nom);
    LabelId->setText("-" + QString::number(idMode) + "-");
    setToolTip(Description);
    ControleCompilation();
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Retourne la liste des modules utilisés par un mode lumineux
QStringList Compilation::InstanceModules()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QStringList ListeModules;
    QDomElement element = doc->documentElement();
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        if (qde.tagName() == TAG_MODULE)
        {
            ListeModules.append(qde.text());
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return ListeModules;
}
