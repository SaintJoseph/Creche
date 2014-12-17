#include "Compile.h"

//Initialisation du nombre d'instance
char Compilation::nbInstance = 0;
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
bool Compilation::addCondition(CondHoraire *CondH)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QDomElement element = doc->documentElement();
    int ValeurID = -1;
    for(QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        QDomElement qde = n.toElement();
        if (!n.isNull())
        {
            if (qde.tagName() == TAG_CONDITION)
            {
                if (qde.hasAttribute(ATTRIBUT_ID)) ValeurID = qde.attribute(ATTRIBUT_ID).toInt();
            }
        }
    }
    QDomElement setcond = addElement(doc, element, TAG_CONDITION, QString::null);
    setcond.setAttribute(ATTRIBUT_ID, ValeurID + 1);
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

//Fonction qui lit toute les conditions d'un mode pour envois vers Arduino
QByteArray Compilation::LireConditions()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
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
        QFile file(Nomfichier);
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
    delete doc;
    if (!NewMode)
        delete NewMode;
    ListeInstance[ (int) Instance] = NULL;
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
    LabelId->setText(QString::number(idMode));
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
    setmode.setAttribute(ATTRIBUT_ID, idMode);
    setmode.setAttribute(ATTRIBUT_PRIORITY, priorite);
    addElement(doc, &setmode, TAG_NOM , Nom);
    addElement(doc, &setmode, TAG_DESCRIPTION , Description);
    addElement(doc, &setmode, TAG_DATA, QString::null);
    QStringList ListeDesModules = NewMode->askeModules();
    foreach (QString Module, ListeDesModules) {
        addElement(doc, &setmode, TAG_MODULE, Module);
    }
    //Led Initialisation turne to green
    LedIni->setColor(QColor(83,221,0));
    setToolTip(Description);

    //Controle et application des couleurs aux led en fonction
    ControleCompilation();

    delete NewMode;
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

void Compilation::initialisationFichierDom()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    while (Nomfichier == NULL)
    {
        Nomfichier = QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier 'Effet lumineux Creche' "), "./", tr("Xml Files (*.Xml);;Text Files (*.txt);;All Files (*.*)"));
    }
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
    doc->documentElement().setAttribute(ATTRIBUT_ID, NouvelId());
    LabelId->setText(doc->documentElement().attribute(ATTRIBUT_ID));
    //Extraction du nom du mode
    QDomElement element = doc->documentElement();
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        //Prend le nom et vérifie l'initialisation du mode
        if (qde.tagName() == TAG_NOM)
        {
            LabelNom->setText(qde.text());
            LedIni->setColor(QColor(83,221,0));
        }
        //Extraction de la description
        {
            if (qde.tagName() == TAG_DESCRIPTION)
                setToolTip(qde.text());
        }
    }
    //Controle et application des couleurs aux led en fonction
    ControleCompilation();

    delete NewMode;
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui ajoute un state dans l'arbre des états mais pas les entrées du state, et renvois l'id + 1 si le state est bien créé
bool Compilation::addState(int id, int pause)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */

#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui ajoute un Progressif dans l'arbre des états pour un state donné, et renvois l'id + 1 si le progressif est bien créé
bool Compilation::addProgressif(LedColor id, int idState, int level, const QString &Nom, const QString &Description)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */

#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui ajoute un ensemble TOR dans l'arbre des états pour un state donné, et renvois l'id + 1 si le ensemble TOR est bien créé
bool Compilation::addTOR(int id, int idState, bool tor[], const QString &Nom, const QString &Description)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */

#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
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
            if (element.tagName() == TAG_MODE)
                if (element.attribute(ATTRIBUT_ID).toInt() > valeur)
                    valeur = element.attribute(ATTRIBUT_ID).toInt();
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

//Fonction qui renvois un pointeur vers un instance valide
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

//Fonction qui li et compiles les données d'un mode
QByteArray Compilation::LireData()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */

#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

QByteArray Compilation::LireToutData()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */

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

void Compilation::ControleCompilation()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QDomElement element = doc->documentElement();
    int cond = 0;
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
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
    }
    if (cond > 0)
    {
        if (cond % 2 == 0)
            LedCond->setColor(QColor(0,231,41));
        else
            LedCond->setColor(QColor(255,109,0));
    }
    else
        LedCond->setColor(QColor(180,0,0));
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Demande de lecture des conditions horaire
void Compilation::DemndeLectureCH(CondHoraire *CondH)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QDomElement element = doc->documentElement();
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        if (qde.tagName() == TAG_CONDITION)
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
void Compilation::SupprimeCondHoraire()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    delElement(doc, TAG_CONDITION);
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
    enregistrer->setWindowTitle(tr("Enregistrer fichier Xml", "Titre fenetre"));
    enregistrer->setText(tr("<font color=\"#FF2A2A\">Voulez-vous sauvegarder le fichier suivant:<font>"));
    enregistrer->setButtonText(QMessageBox::Save,tr("Enregistrer"));
    enregistrer->setButtonText(QMessageBox::Discard, tr("Ne pas enregistrer"));
    LabelId->setToolTip(tr("ID attribué à ce mode pour un fonctionnement multi mode"));
    if (NewMode)
        NewMode->retranslate(lang);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}
