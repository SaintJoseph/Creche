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
    //L'écriture d'un condition doit etre: [Debut(Mois , Jour , JourSem, Heure, Min), fin(Mois, Jour, JourSem, Heure, Min) Mode, priorite]
    //Rappel on écrit sur le port Arduino avec un QByteArray
    QDomElement element = doc->documentElement();
    QByteArray byteArray("");
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        if (qde.tagName() == TAG_CONDITION)
        {
            int conteur = 0;
            for (QDomElement qdec = qde.firstChildElement(); !qdec.isNull(); qdec = qdec.nextSiblingElement())
            {
                for (QDomElement qdecs = qdec.firstChildElement(); !qdecs.isNull(); qdecs = qdecs.nextSiblingElement())
                {
                    if (conteur == 0 || conteur == 5)
                    {
                            if (qdecs.tagName() == TAG_MOIS)
                                byteArray.append(qdecs.text().toUInt());
                            else {
                                byteArray.append("\255");
                                conteur++;
                            }
                    }
                    if (conteur == 1 || conteur == 6)
                    {
                            if (qdecs.tagName() == TAG_JOUR)
                                byteArray.append(qdecs.text().toUInt());
                            else {
                                byteArray.append("\255");
                                conteur++;
                            }
                    }
                    if (conteur == 2 || conteur == 7)
                    {
                            if (qdecs.tagName() == TAG_JOURSEM)
                                byteArray.append(qdecs.text().toUInt());
                            else {
                                byteArray.append("\255");
                                conteur++;
                            }
                    }
                            if (qdecs.tagName() == TAG_HEURE && (conteur == 3 || conteur == 8))
                                byteArray.append(qdecs.text().toUInt());
                            if (qdecs.tagName() == TAG_MIN && (conteur == 4 || conteur == 9))
                                byteArray.append(qdecs.text().toUInt());
                    conteur++;
                }
            }
            if (element.tagName() == TAG_MODE)
            {
                byteArray.append(element.attribute(ATTRIBUT_ID).toInt());
                byteArray.append(element.attribute(ATTRIBUT_PRIORITY).toInt());
            }
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return byteArray;
}

//Fonction qui li les conditions de toutes les instances existante du programme
QByteArray Compilation::LireToutesConditions()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QByteArray cond = "";
    for (int i = 0; i < nbInstance; i++)
    {
        cond += (ListeInstance[i] != NULL)?ListeInstance[i]->LireConditions():"";
    }
    cond.append("\255\255\255\255\255\255\255\255\255\255\255\255");
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return cond;
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
    retranslate();

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
    QDomElement nom = addElement(doc, &setmode, TAG_NOM , Nom);
    QDomElement describ = addElement(doc, &setmode, TAG_DESCRIPTION , Description);
    QDomElement data = addElement(doc, &setmode, TAG_DATA, QString::null);
    //Led Initialisation turne to green
    LedIni->setColor(QColor(83,221,0));
    QDomElement element = doc->documentElement();
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
    bool test = false;
    QDomElement element = doc->documentElement();
    for (QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        if (qde.tagName() == TAG_DATA)
        {
            for (QDomElement qdecs1 = qde.firstChildElement(); !qdecs1.isNull(); qdecs1 = qdecs1.nextSiblingElement())
            {
                if (qdecs1.tagName() == TAG_STATE && qdecs1.attribute(ATTRIBUT_ID).toInt() == id)
                {
                    test = false;
                }
            }
            QDomElement setState = addElement(doc, qde, TAG_STATE, QString::null);
            setState.setAttribute(ATTRIBUT_ID, id++);
            addElement(doc, setState, TAG_PAUSE, QString::number(pause));
            test = true;
        }
    }
    //Controle et application des couleurs aux led en fonction
    ControleCompilation();
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return test;
}

//Fonction qui ajoute un Progressif dans l'arbre des états pour un state donné, et renvois l'id + 1 si le progressif est bien créé
bool Compilation::addProgressif(LedColor id, int idState, int level, const QString &Nom, const QString &Description)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QDomElement element = doc->documentElement();
    for (QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        if (qde.tagName() == TAG_DATA)
        {
            for (QDomElement qdec = element.firstChildElement(); !qdec.isNull(); qdec = qdec.nextSiblingElement())
            {
                if (qdec.tagName() == TAG_DATA)
                {
                    for (QDomElement qdecs = qdec.firstChildElement(); !qdecs.isNull(); qdecs = qdecs.nextSiblingElement())
                    {
                        if (qdecs.tagName() == TAG_STATE && qdecs.attribute(ATTRIBUT_ID).toInt() == idState)
                        {
                            for (QDomElement qdecs1 = qdecs.firstChildElement(); !qdecs1.isNull(); qdecs1 = qdecs1.nextSiblingElement())
                            {
                                if (qdecs1.tagName() == TAG_PROGRESSIF && qdecs1.attribute(ATTRIBUT_ID).toInt() == id)
                                {
#ifdef DEBUG_COMANDSAVE
                                    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
                                    return false;
                                }
                            }
                            QDomElement setProgr = addElement(doc, qdecs, TAG_PROGRESSIF, QString::null);
                            setProgr.setAttribute(ATTRIBUT_ID, (int) id);
                            addElement(doc, setProgr, TAG_NOM, Nom);
                            addElement(doc, setProgr, TAG_DESCRIPTION, Description);
                            addElement(doc, setProgr, TAG_LEVEL, QString::number(level));
                            QDomComment comm = doc->createComment(tr("L'Id des Led Blanche circuit 1 doit etre = 1,\
Id des Led Blanche circuit 2 = 2, Id des Led Blanche circuit 3 = 3, Id des Led Rouge = 4, Id des Led Orange = 5, Id des Led Bleus = 6, Id de la lune = 9\n\
a chaque state on définit l'intensité lumineuse instantanée de chaque circuit de LED. Le module arduino calcule alors la rampe de variation"));
                            setProgr.appendChild(comm);
#ifdef DEBUG_COMANDSAVE
                            std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
                            return true;
                        }
                    }
                }
            }
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return false;
}

//Fonction qui ajoute un ensemble TOR dans l'arbre des états pour un state donné, et renvois l'id + 1 si le ensemble TOR est bien créé
bool Compilation::addTOR(int id, int idState, bool tor[], const QString &Nom, const QString &Description)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QDomElement element = doc->documentElement();
    for (QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        if (qde.tagName() == TAG_DATA)
        {
            for (QDomElement qdec = element.firstChildElement(); !qdec.isNull(); qdec = qdec.nextSiblingElement())
            {
                if (qdec.tagName() == TAG_DATA)
                {
                    for (QDomElement qdecs = qdec.firstChildElement(); !qdecs.isNull(); qdecs = qdecs.nextSiblingElement())
                    {
                        if (qdecs.tagName() == TAG_STATE && qdecs.attribute(ATTRIBUT_ID).toInt() == idState)
                        {
                            for (QDomElement qdecs1 = qdecs.firstChildElement(); !qdecs1.isNull(); qdecs1 = qdecs1.nextSiblingElement())
                            {
                                if (qdecs1.tagName() == TAG_TOR && qdecs1.attribute(ATTRIBUT_ID).toInt() == id)
                                {
#ifdef DEBUG_COMANDSAVE
                                    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
                                    return false;
                                }
                            }
                            QDomElement setTor = addElement(doc, qdecs, TAG_TOR, QString::null);
                            setTor.setAttribute(ATTRIBUT_ID, id);
                            addElement(doc, setTor, TAG_NOM, Nom);
                            addElement(doc, setTor, TAG_DESCRIPTION, Description);
                            QDomComment comm = doc->createComment(tr("L'Id des Led TOR pour le portique doit etre = 7, détail des spots (20°):2 vert Id 0 et 1, 1 jaune Id 2, 5 blanc chaud Id 3 - 7, 1 Rouge Id 8", "TOR = Tout Ou Rien"));
                            setTor.appendChild(comm);
                            for (int i = 0; i < 16; i++)
                            {
                                QDomElement TorA = addElement(doc, setTor, TAG_LED, ((tor[i])?"On":"Off"));
                                TorA.setAttribute(ATTRIBUT_ID, i);
                            }
#ifdef DEBUG_COMANDSAVE
                            std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
                            return true;
                        }
                    }
                }
            }
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return false;
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
    //Controle des données
    //Extraction
    //Renvois
    QDomElement element = doc->documentElement();
    QByteArray byteArray("");
    int nbstate = 0;
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        if (qde.tagName() == TAG_DATA)
        {
            for(QDomElement dataElement = qde.firstChildElement(); !dataElement.isNull(); dataElement = dataElement.nextSiblingElement())
            {
                //La variable conteur permet de déterminer la position d'un information car chaque info a une position précise
                int Compteur = 1, lop = 0;
                //lop permet de limiter le nombre de boucle au cas ou il y a une erreur dans le fichier
                while(lop < 110) //Normalement avec 100 state possibles on devrais mettre 100 + sécurité = 110, mais c'est beaucoup trop
                {
                    //La compilation prend en compte chaque state 1 seule fois. Pour cela il controle le num de state avec nbstate avant compilation
                    if (dataElement.tagName() == TAG_STATE && dataElement.attribute(ATTRIBUT_ID).toInt() == nbstate)
                    {
                        nbstate++;
                        //Pour chaque state, le premier chiffre indique le mode auquel il appartient
                        byteArray += element.attribute(ATTRIBUT_ID).toInt();
                        Compteur++;
                        //Le deuxième chiffre indique l'id du state
                        byteArray += dataElement.attribute(ATTRIBUT_ID).toInt();
                        Compteur++;
                        int loop = 0;
                        while (loop < 40 && Compteur < 32)
                        {
                            int ValeurCompteur = Compteur;
                            for(QDomElement stateElement = dataElement.firstChildElement(); !stateElement.isNull(); stateElement = stateElement.nextSiblingElement())
                            {
                                switch (Compteur) {
                                case 3: //le 3 chiffre indique le temps de pause
                                    if (stateElement.tagName() == TAG_PAUSE)
                                    {
                                        byteArray += (char) stateElement.text().toInt();
                                    }
                                    else byteArray.append((char)0);
                                    Compteur++;
                                    break;
                                case 4: //Blanc 1 , level pos 4
                                    if (stateElement.tagName() == TAG_PROGRESSIF && stateElement.attribute(ATTRIBUT_ID).toInt() == 1)
                                    {
                                        compileProgressif(&byteArray, &stateElement, &Compteur);
                                    }
                                    break;
                                case 5: //Blanc 2 , level pos 5
                                    if (stateElement.tagName() == TAG_PROGRESSIF && stateElement.attribute(ATTRIBUT_ID).toInt() == 2)
                                    {
                                        compileProgressif(&byteArray, &stateElement, &Compteur);
                                    }
                                    break;
                                case 6: //Blanc 3 , level pos 6
                                    if (stateElement.tagName() == TAG_PROGRESSIF && stateElement.attribute(ATTRIBUT_ID).toInt() == 3)
                                    {
                                        compileProgressif(&byteArray, &stateElement, &Compteur);
                                    }
                                    break;
                                case 7: //Rouge , level pos 7
                                    if (stateElement.tagName() == TAG_PROGRESSIF && stateElement.attribute(ATTRIBUT_ID).toInt() == 4)
                                    {
                                        compileProgressif(&byteArray, &stateElement, &Compteur);
                                    }
                                    break;
                                case 8: //Orange , level pos 8
                                    if (stateElement.tagName() == TAG_PROGRESSIF && stateElement.attribute(ATTRIBUT_ID).toInt() == 5)
                                    {
                                        compileProgressif(&byteArray, &stateElement, &Compteur);
                                    }
                                    break;
                                case 9: //Bleu , level pos 9
                                    if (stateElement.tagName() == TAG_PROGRESSIF && stateElement.attribute(ATTRIBUT_ID).toInt() == 6)
                                    {
                                        compileProgressif(&byteArray, &stateElement, &Compteur);
                                    }
                                    break;
                                case 10: //TOR portique , value 10, value 11
                                    if (stateElement.tagName() == TAG_TOR && stateElement.attribute(ATTRIBUT_ID).toInt() == 7)
                                    {
                                        int cont = 0, loop = 0;
                                        unsigned char mask = 0x01, data = 0x00;
                                        while (cont < 16) {
                                            loop++;
                                            for(QDomElement tor = stateElement.firstChildElement(); !tor.isNull(); tor = tor.nextSiblingElement())
                                            {
                                                if (tor.tagName() == TAG_LED)
                                                {
                                                    if (tor.attribute(ATTRIBUT_ID).toInt() == cont)
                                                    {
                                                        //Les booleens des 16 led TOR programmées sont convertir sur 2 octets distincs. Chacun des octets est alors inseré dans la chaine de transfert
                                                        if (tor.text() == "On") data |= (mask << cont % 8);
                                                        cont++;
                                                        //On a recopier les binaire du 1er octet
                                                        if (cont == 8)
                                                        {
                                                            byteArray += data;
                                                            data = 0x00;
                                                            Compteur++;
                                                        }
                                                        if (cont == 16)
                                                        {
                                                            byteArray += data;
                                                            Compteur++;
                                                        }
                                                    }
                                                }
                                            }
                                            if (loop > 32) break;
                                        }
                                    }
                                    break;
                                case 12: //Libre
                                    byteArray += "\1\1";
                                    Compteur += 2;
                                    break;
                                case 13: //Lune level pos 13
                                    if (stateElement.tagName() == TAG_PROGRESSIF && stateElement.attribute(ATTRIBUT_ID).toInt() == 9)
                                    {
                                        compileProgressif(&byteArray, &stateElement, &Compteur);
                                    }
                                    break;
                                case 14: //Libre
                                    byteArray += "\1\1";
                                    Compteur += 2;
                                    break;
                                case 15: //Libre
                                    byteArray += "\1\1";
                                    Compteur += 2;
                                    break;
                                case 16: //Libre
                                    byteArray += "\1\1";
                                    Compteur += 2;
                                    break;
                                case 17: //Libre
                                    byteArray += "\1\1";
                                    Compteur += 2;
                                    break;
                                case 18: //Validation pos 30, validation de synchro pos 31
                                    //Valeur booleénne. il y a 4 validations par cycle, et 1 validation de synchronisation par cycle
                                    if (stateElement.tagName() == TAG_SYNC)
                                    {
                                        byteArray += stateElement.attribute(ATTRIBUT_SYNC).toInt();
                                        byteArray += stateElement.attribute(ATTRIBUT_VALID).toInt();
                                        Compteur += 2;
                                    }
                                    break;
                                default:
                                    break;
                                }
                            }
                            //Condition qui vient recopier la valeur du state précédent lorsqu'une LED avec un id défini est manquant dans un state autre que le state initiale
                            if (ValeurCompteur == Compteur && dataElement.attribute(ATTRIBUT_ID) > 0)
                            {
                                //S'il faut faire un recopiage de validation la valeur est tjr null (pas de recopiage)
                                if (Compteur == 18)
                                {
                                    //Pour ajouter des 0 en fin de chaine, je suis obliger de passer par une variable sinon, il considère que j'ai un carractère de terminaison de chaine
                                    int a = 0;
                                    byteArray += a;
                                    byteArray += a;
                                    Compteur += 2;
                                }
                                else
                                {
                                   //Comme l'état des led est défini par 1 char alors on le recopie directement
                                   int pos = byteArray.count() - 19;
                                   byteArray += byteArray.at((pos < 0)?0:pos);
                                   Compteur++;
                                }
                            }
                            loop++;
                        }
                        if (Compteur < 19 && dataElement.attribute(ATTRIBUT_ID) == 0)
                        {

#ifdef DEBUG_COMANDSAVE
                            qDebug() << "Compilation impossible : l'éclairage avec l'Id " << (Compteur - 2 - Compteur % 2) / 2 << ", pour le state " << dataElement.attribute(ATTRIBUT_ID) << "n'a pas été trouvé.";
                            qDebug() << "Le state initial doit avoir une définition pour chacun des éclairages";
                            std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */

                            return (QByteArray) "\0";
                        }
                    }
                    lop++;
                }
            }
        }
    }

#ifdef DEBUG_COMANDSAVE
    if (nbstate == 0) qDebug() << "Compilation du mode " << element.attribute(ATTRIBUT_ID) << "echouée, state 0 introuvable.";
    else qDebug() << "Compilation du mode " << element.attribute(ATTRIBUT_ID) << "terminé avec succet, " << nbstate << "state trouvés et compilé.";
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */

    return byteArray;
}

bool Compilation::compileProgressif(QByteArray *byteArray, QDomElement *element, int *position)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    int cont = 0, loop = 0;
    while (cont < 1) {
        loop++;
        for(QDomElement progressif = element->firstChildElement(); !progressif.isNull(); progressif = progressif.nextSiblingElement())
        {
            if (progressif.tagName() == TAG_LEVEL && cont == 0)
            {
                *byteArray += progressif.text().toInt();
                *position = *position + 1;
                cont++;
            }
        }
        if (loop > 3) break;
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return ((cont == 1)?true:false);
}

QByteArray Compilation::LireToutData()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QByteArray byteArray("");
    for (int i = 0; i < nbInstance; i++)
    {
        byteArray += (ListeInstance[i] != NULL)?ListeInstance[i]->LireData():"";
    }
    byteArray.append("\255\255\255\255\255\255\255\255\255\255\255\255");
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return byteArray;
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
     QDomElement element = doc->documentElement();
     for (QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
     {
         if (qde.tagName() == TAG_DATA)
         {
             for (QDomElement qdec = element.firstChildElement(); !qdec.isNull(); qdec = qdec.nextSiblingElement())
             {
                 if (qdec.tagName() == TAG_DATA)
                 {
                     for (QDomElement qdecs = qdec.firstChildElement(); !qdecs.isNull(); qdecs = qdecs.nextSiblingElement())
                     {
                         if (qdecs.tagName() == TAG_STATE && qdecs.attribute(ATTRIBUT_ID).toInt() == idState)
                         {
                             for (QDomElement qdecs1 = qdecs.firstChildElement(); !qdecs1.isNull(); qdecs1 = qdecs1.nextSiblingElement())
                             {
                                 if (qdecs1.tagName() == TAG_SYNC && qdecs1.hasAttribute(ATTRIBUT_VALID) && qdecs1.hasAttribute(ATTRIBUT_SYNC))
                                 {
                                     return false;
#ifdef DEBUG_COMANDSAVE
                                     std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
                                 }
                             }
                             QDomElement setTor = addElement(doc, qdecs, TAG_SYNC, QString::null);
                             setTor.setAttribute(ATTRIBUT_SYNC, Synchronisation);
                             setTor.setAttribute(ATTRIBUT_VALID, Validation);
                             QDomComment comm = doc->createComment(tr("La synchronisation permet de mettre en phase le système avec l'ancien système. La validation a lieu 4 fois par cycle et la synchronisation 1 fois par cycle (en meme temps qu'une validation)"));
                             setTor.appendChild(comm);
#ifdef DEBUG_COMANDSAVE
                             std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
                             return true;
                         }
                     }
                 }
             }
         }
     }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
     return false;
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

//Fonction surchargée d'évènement souris
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
    //nbstate compte le nombre de state
    int nbstate = 0;
    int cond = 0;
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        if (qde.tagName() == TAG_DATA)
        {
            for(QDomElement dataElement = qde.firstChildElement(); !dataElement.isNull(); dataElement = dataElement.nextSiblingElement())
            {
                //lop permet de limiter le nombre de boucle au cas ou il y a une erreur dans le fichier
                int lop = 0;
                while(lop < 110) //Normalement avec 100 state possibles on devrais mettre 100 + sécurité = 110, mais c'est beaucoup trop
                {
                    //La compilation prend en compte chaque state 1 seule fois. Pour cela il controle le num de state avec nbstate avant compilation
                    if (dataElement.tagName() == TAG_STATE && dataElement.attribute(ATTRIBUT_ID).toInt() == nbstate)
                        nbstate++;
                    lop++;
                }
            }
        }
        if (qde.tagName() == TAG_CONDITION)
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
    if (nbstate == 0)
        LedState->setColor(QColor(180,0,0));
     else
    {
        if (nbstate > 0 && nbstate < 5)
            LedState->setColor(QColor(255,109,0));
        else
            if (nbstate > 4)
                LedState->setColor(QColor(0,231,41));
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

//Fonction qui retourne les conditions horaire de l'instance active
QString Compilation::ReturnCondHoraire()
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
    if (test)
        return str;
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //return "Vide"
    return Const_QString(1);
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
void Compilation::retranslate()
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
//    if (NewMode)
//        NewMode->retranslate();
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}
