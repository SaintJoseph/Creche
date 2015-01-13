#include "lecommandemodules.h"

LECommandeModules::LECommandeModules(QWidget *parent):
     QWidget(parent)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Initialisations des widgets composant le dock
    //Définition d'une police pour le dock
    QFont font("Verdana", 9);
    setFont(font);
    //Label Module
    LabelModule = new QLabel;
    LabelModule->setFont(font);
    //Label Destinataire
    LabelDesti = new QLabel;
    LabelDesti->setFont(font);
    //Label Provenance
    LabelProv = new QLabel;
    LabelProv->setFont(font);
    //Label type de commande
    LabelTypeCmd = new QLabel;
    LabelTypeCmd->setFont(font);
    LabelTypeCmd->setHidden(true);
    //Label Commande
    LabelCmd = new QLabel;
    LabelCmd->setFont(font);
    LabelCmd->setHidden(true);
    //Label instruction
    LabelInstruction = new QLabel;
    LabelInstruction->setFont(font);
    LabelInstruction->setHidden(true);
    //Label aide pour le parametre
    LabelAideParametre = new QLabel;
    LabelAideParametre->setFont(font);
    LabelAideParametre->setHidden(true);
    //Layout V pour placer les boutons de communication
    LayoutEnvoisRecep = new QVBoxLayout;
    LayoutEnvoisRecep->setSpacing(1);
    LayoutEnvoisRecep->setContentsMargins(1,1,1,1);
    LayoutEnvoisRecep->setAlignment(Qt::AlignTop);
    //Layout modules
    LayoutModules = new QHBoxLayout;
    LayoutModules->setSpacing(1);
    LayoutModules->setContentsMargins(0,0,0,0);
    LayoutModules->setAlignment(Qt::AlignCenter);
    //Layout Desti
    LayoutDesti = new QVBoxLayout;
    LayoutDesti->setSpacing(1);
    LayoutDesti->setContentsMargins(1,1,1,1);
    LayoutDesti->setAlignment(Qt::AlignTop);
    //Layout Prov
    LayoutProv = new QVBoxLayout;
    LayoutProv->setSpacing(1);
    LayoutProv->setContentsMargins(1,1,1,1);
    LayoutProv->setAlignment(Qt::AlignTop);
    //Ligne d'édition pour instroduire les paramètres complémentaires
    LineEditInstruction = new QLineEdit;
    LineEditInstruction->setFont(font);
    LineEditInstruction->setFixedHeight(22);
    LineEditInstruction->setHidden(true);
    //ComboBox selection Module
    QComboModul = new QComboBox;
    QComboModul->setFont(font);
    QComboModul->addItem("");
    //ComboBox Sélection Module Provenance
    QComboProv = new QComboBox;
    QComboProv->setFont(font);
    QComboProv->addItem("");
    //ComboBox selection Type de commande
    QComboTypeCmd = new QComboBox;
    QComboTypeCmd->setFont(font);
    QComboTypeCmd->setHidden(true);
    QComboTypeCmd->addItem("");
    //Boutton Valider (Quand il n'y a pas de paramètre)
    BouttonValider = new QPushButton;
    BouttonValider->setFont(font);
    BouttonValider->setFixedHeight(22);
    BouttonValider->setHidden(true);
    //ComboBox selection de commande
    QComboCmd = new QComboBox;
    QComboCmd->setFont(font);
    QComboCmd->setHidden(true);
    QComboCmd->addItem("");
    //Fenetre convertisseur Décimal vers Hexadécimal
    Convert = new ConvertisseurDecHexa;
    Convert->setHidden(true);

    //Dessin du widget en attribuant les références de parents enfants
    //Label Module
    LayoutEnvoisRecep->addWidget(LabelModule);
    LayoutEnvoisRecep->addLayout(LayoutModules);
    //Labels et ComboBox Modules
    LayoutDesti->addWidget(LabelDesti);
    LayoutDesti->addWidget(QComboModul);
    LayoutProv->addWidget(LabelProv);
    LayoutProv->addWidget(QComboProv);
    //Label et QcomboBox TypeCmd
    LayoutEnvoisRecep->addWidget(LabelTypeCmd);
    LayoutEnvoisRecep->addWidget(QComboTypeCmd);
    //Label et QcomboBox Cmd
    LayoutEnvoisRecep->addWidget(LabelCmd);
    LayoutEnvoisRecep->addWidget(QComboCmd);
    //Labels et QligneEdit Paramètre
    LayoutEnvoisRecep->addWidget(LabelInstruction);
    LayoutEnvoisRecep->addWidget(LineEditInstruction);
    LayoutEnvoisRecep->addWidget(LabelAideParametre);
    LayoutEnvoisRecep->addWidget(Convert);
    //Boutton Valider
    LayoutEnvoisRecep->addWidget(BouttonValider);
    //Inclusion des layout pour mettre les deux comboBox en parrallèle
    LayoutModules->addLayout(LayoutDesti);
    LayoutModules->addLayout(LayoutProv);
    //Application du tout au widget
    setLayout(LayoutEnvoisRecep);

    //Création l'arbre Xml avec la liste des commandes que l'on peut envoyer aux modules
    docCommandeModule = new QDomDocument();
    QFile *file = new QFile(":cmd.xml");
#ifdef DEBUG_COMANDSAVE
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Erreur a l'ouverture du fichier"),tr("Une erreur à l'ouverture du fichier \"CommandeModule.xml\"."),QMessageBox::Ok);
    }
    if (!docCommandeModule->setContent(file)) {
        QMessageBox::critical(this, tr("Erreur a la lecture du fichier"),tr("Une erreur à la lecture du fichier \"CommandeModule.xml\"."),QMessageBox::Ok);
    }
#else
    file->open(QIODevice::ReadOnly | QIODevice::Text);
    docCommandeModule->setContent(file);
#endif /* DEBUG_COMANDSAVE */
    file->close();
    delete file;

    //On applique les labels et peuple les comboBox
    retranslate("fr");

    //Largeur du widget
    setMaximumWidth(400);
    setMinimumWidth(200);
    //On applique des valeurs par défaut
    setProvModifiable(true, 0);

    //Signaux pour la gestion des combobox pour envoyer des messages système vers les modules arduino
    connect(QComboModul, SIGNAL(currentIndexChanged(int)), SLOT(onModuleChanged(int)));
    connect(QComboTypeCmd, SIGNAL(currentIndexChanged(int)), SLOT(onTypeCmdChanged(int)));
    connect(QComboCmd, SIGNAL(currentIndexChanged(int)), SLOT(onCmdChanged(int)));
    connect(Convert, SIGNAL(HexaVal(QString)), SLOT(onHexaCame(QString)));
    connect(LineEditInstruction, SIGNAL(returnPressed()), SLOT(onValidMesg()));
    connect(BouttonValider, SIGNAL(clicked()), SLOT(onValidMesg()));

#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

LECommandeModules::~LECommandeModules()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    delete LineEditInstruction;
    delete LabelInstruction;
    delete LabelCmd;
    delete LabelModule;
    delete LabelTypeCmd;
    delete LabelAideParametre;
    delete QComboModul;
    delete QComboProv;
    delete LabelDesti;
    delete LabelProv;
    delete BouttonValider;
    delete QComboTypeCmd;
    delete QComboCmd;
    delete docCommandeModule;
    delete Convert;
    delete LayoutDesti;
    delete LayoutProv;
    delete LayoutModules;
    delete LayoutEnvoisRecep;
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Slot sur un changement de module pour envoyer un message système vers un module
void LECommandeModules::onModuleChanged(int index)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    if (index == 0) {
        QComboTypeCmd->setHidden(true);
        QComboCmd->setHidden(true);
        LineEditInstruction->setHidden(true);
        LabelTypeCmd->setHidden(true);
        LabelCmd->setHidden(true);
        LabelInstruction->setHidden(true);
        LabelAideParametre->setHidden(true);
        Convert->setHidden(true);
        BouttonValider->setHidden(true);
    }
    else {
        QComboTypeCmd->setHidden(true);
        QComboCmd->setHidden(true);
        LineEditInstruction->setHidden(true);
        LabelTypeCmd->setHidden(true);
        LabelCmd->setHidden(true);
        LabelInstruction->setHidden(true);
        LabelAideParametre->setHidden(true);
        Convert->setHidden(true);
        BouttonValider->setHidden(true);
        QComboTypeCmd->clear();
        QComboTypeCmd->addItem("");
        //On peuple la comboboxTypeCmd
        QDomElement element = docCommandeModule->documentElement();
        for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
        {
            //Prend le nom et vérifie l'initialisation du mode
            if (qde.attribute("Nom:" + indLangue) == QComboModul->currentText())
            {
                for(QDomElement qdea = qde.firstChildElement(); !qdea.isNull(); qdea = qdea.nextSiblingElement())
                {
                    if (qdea.tagName() == "TypeCmd")
                    {
                        QComboTypeCmd->setHidden(false);
                        LabelTypeCmd->setHidden(false);
                        QComboTypeCmd->addItem(qdea.attribute("Nom:" + indLangue), qdea.attribute("Cmd"));
                    }
                }
            }
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Slot sur un changement de type de commande pour envoyer un message système vers un module
void LECommandeModules::onTypeCmdChanged(int index)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    if (index == 0) {
        QComboCmd->setHidden(true);
        LineEditInstruction->setHidden(true);
        LabelCmd->setHidden(true);
        LabelInstruction->setHidden(true);
        LabelAideParametre->setHidden(true);
        Convert->setHidden(true);
        BouttonValider->setHidden(true);
    }
    else {
        QComboCmd->setHidden(true);
        LineEditInstruction->setHidden(true);
        LabelCmd->setHidden(true);
        LabelInstruction->setHidden(true);
        LabelAideParametre->setHidden(true);
        Convert->setHidden(true);
        BouttonValider->setHidden(true);
        QComboCmd->clear();
        QComboCmd->addItem("");
        //On peuple la comboboxTypeCmd
        QDomElement element = docCommandeModule->documentElement();
        for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
        {
            //Prend le nom et vérifie l'initialisation du mode
            if (qde.attribute("Nom:" + indLangue) == QComboModul->currentText())
            {
                for(QDomElement qdea = qde.firstChildElement(); !qdea.isNull(); qdea = qdea.nextSiblingElement())
                {
                    if (qdea.attribute("Nom:" + indLangue) == QComboTypeCmd->currentText())
                    {
                        for(QDomElement qdeab = qdea.firstChildElement(); !qdeab.isNull(); qdeab = qdeab.nextSiblingElement())
                        {
                            if (qdeab.tagName() == "Commande") {
                                QComboCmd->setHidden(false);
                                LabelCmd->setHidden(false);
                                QComboCmd->addItem(qdeab.attribute("Nom:" + indLangue), qdeab.attribute("Cmd"));
                            }
                        }
                    }
                }
            }
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Slot sur un changement de commande pour envoyer un message système vers un module
void LECommandeModules::onCmdChanged(int index)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    if (index == 0) {
        LineEditInstruction->setHidden(true);
        LineEditInstruction->setInputMask("");
        LabelInstruction->setHidden(true);
        LabelAideParametre->setHidden(true);
        Convert->setHidden(true);
        BouttonValider->setHidden(true);
    }
    else {
        LineEditInstruction->setHidden(true);
        LineEditInstruction->setInputMask("");
        LabelInstruction->setHidden(true);
        LabelAideParametre->setHidden(true);
        Convert->setHidden(true);
        BouttonValider->setHidden(true);
        //On peuple la comboboxTypeCmd
        QDomElement element = docCommandeModule->documentElement();
        for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
        {
            //Prend le nom et vérifie l'initialisation du mode
            if (qde.attribute("Nom:" + indLangue) == QComboModul->currentText())
            {
                for(QDomElement qdea = qde.firstChildElement(); !qdea.isNull(); qdea = qdea.nextSiblingElement())
                {
                    if (qdea.attribute("Nom:" + indLangue) == QComboTypeCmd->currentText())
                    {
                        for(QDomElement qdeab = qdea.firstChildElement(); !qdeab.isNull(); qdeab = qdeab.nextSiblingElement())
                        {
                            if (qdeab.attribute("Nom:" + indLangue) == QComboCmd->currentText())
                            {
                                //Par défaut on affiche le bouton pour valider
                                BouttonValider->setHidden(false);
                                for(QDomElement qdeabc = qdeab.firstChildElement(); !qdeabc.isNull(); qdeabc = qdeabc.nextSiblingElement())
                                {
                                    if (qdeabc.tagName() == "Parametre") {
                                        //Quand il y a des paramètre on cache le bouton valider
                                        BouttonValider->setHidden(true);
                                        LineEditInstruction->setHidden(false);
                                        LineEditInstruction->clear();
                                        if (qdeabc.hasAttribute("Format")) {
                                           QString Format = qdeabc.attribute("Format");
                                           LineEditInstruction->setInputMask(Format);
                                        }
                                        LabelInstruction->setHidden(false);
                                        LabelAideParametre->setHidden(false);
                                        LabelAideParametre->setText(qdeabc.attribute("Aide:" + indLangue));
                                        if (qdeabc.attribute("Hexa") == "true") {
                                            Convert->setHidden(false);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Reception d'une valeur hexadécimal
void LECommandeModules::onHexaCame(QString Val)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QString Laligne = LineEditInstruction->text();
    QString Format = LineEditInstruction->inputMask();
    if (Format != "") {
       //Application du format à LaLigne avec la nouvelle Val Valeur
        if (Val.length() == 1 || Val.length() == 3)
            Val.prepend("0");
        int index = Laligne.indexOf(' ');
        Laligne.prepend(Val);
    }
    else {
        if (Val.length() == 1 || Val.length() == 3)
            Laligne.append("0");
        Laligne.append(Val);
    }
    LineEditInstruction->setText(Laligne.toUpper());
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui traduit un message reçu pour le rendre plus lisible à l'utilisateur
QString LECommandeModules::Interpretation(QString Message)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QString Destination = "Pour ", ValeurAcomparer = "", Provenance = "", TextComplet = "";
    //On recherche le destinataire du message
    ValeurAcomparer = Message.mid(1,3);
    QDomElement element = docCommandeModule->documentElement();
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        //On compare les commandes
        if (qde.attribute("Cmd") == ValeurAcomparer && (qde.tagName() == "Module" || qde.tagName() == "ProgrammePC"))
        {
            Destination.append(qde.attribute("Nom:" + indLangue));
            for(QDomElement qdea = qde.firstChildElement(); !qdea.isNull(); qdea = qdea.nextSiblingElement())
            {
                //On compare les type de commandes
                if (qdea.attribute("Cmd") == Message.mid(7,1))
                {
                    TextComplet.append(qdea.attribute("Nom:" + indLangue));
                    TextComplet.append(", ");
                    for(QDomElement qdeab = qdea.firstChildElement(); !qdeab.isNull(); qdeab = qdeab.nextSiblingElement())
                    {
                        //On compare les type de commandes
                        bool test = false;
                        //Si Cmd correspond on procéde à l'affichage des données
                        if (qdeab.attribute("Cmd") == Message.mid(8,1)) test = true;
                        //Si il y a un attribu de type Provenance, on le vérifie et éventuellement annule l'affichage
                        //C'est pour les cas où la commande est identique pour des choses différentes sur des modules différent
                        if (qdeab.hasAttribute("Prov"))
                            if (qdeab.attribute("Prov") != Message.mid(4,3))
                                if (test) test = false;
                        if (test)
                        {
                            TextComplet.append(qdeab.attribute("Nom:" + indLangue));
                            TextComplet.append(": ");
                            //Ajouter si possible le convertisseur Hexa vers décimal
                            if (qdeab.attribute("Hexa") == "true")
                                TextComplet.append(ConvertHexaDec(qdeab.attribute("Format"), Message.mid(9)));
                            //En XML l'attribu "Hexa" si la chaine contient du binaire ou de l'Hexa
                            else {
                                TextComplet.append(Message.mid(9));
                                TextComplet.chop(1);
                            }
                            TextComplet.append(" ( ");
                            TextComplet.append(qdeab.firstChildElement().attribute("Aide:" + indLangue));
                            TextComplet.append(" )");
                        }
                    }
                }
            }
        }
    }
    //On recherche l'envoyeur
    ValeurAcomparer = Message.mid(4,3);
    Provenance.append(tr(", provenant de "));
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        //On compare les commandes
        if (qde.attribute("Cmd") == ValeurAcomparer && (qde.tagName() == "Module" || qde.tagName() == "ProgrammePC"))
        {
            Provenance.append(qde.attribute("Nom:" + indLangue));
        }
    }
    Provenance.append(", "),
    Destination.append(Provenance);
    Destination.append(TextComplet);
    Destination.append("\n");
    TextComplet = Destination;
    return TextComplet;
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Converti les valeurs hexa contenue dans Value en décimal
QString LECommandeModules::ConvertHexaDec(QString Format, QString Value)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //On transforme la string format pour indiquer uniquement les booléen et les hexa
    //FormatCorr => identifie les valeurs a remplacée, FormatCopy => Caractère de séparation
    QString FormatCorr = "", FormatCopy = "", result = "";
    //length longeur de la chaine, offset => lorsque l'on rencontre un \, il y a un décalage de 1 char dans le format
    int length = Format.length(), offset = 0;
    //La lettre 'C' est inscrite dans les cases vide pour les identifiers proprement
    for (int i = 0; i < length; i++) {
        switch (Format.at(i + offset).toLatin1()) {
        case 'H':
            FormatCorr.append(Format.at(i + offset));
            FormatCopy.append('C');
            break;
        case 'B':
            FormatCorr.append(Format.at(i + offset));
            FormatCopy.append('C');
            break;
        case 'A':
            FormatCorr.append(Format.at(i + offset));
            FormatCopy.append('C');
            break;
        case 'N':
            FormatCorr.append(Format.at(i + offset));
            FormatCopy.append('C');
            break;
        case '\\':
            //Ce carractère annule le carractère suivant
            offset += 1;
            length -= 1;
            FormatCorr.append('C');
            FormatCopy.append(Format.at(i + offset));
            break;
        default:
            FormatCorr.append('C');
            if (Value.at(i) == Format.at(i)) {
               FormatCopy.append(Format.at(i + offset));
            }
            else {
                FormatCopy.append('D');
            }
            break;
        }
    }
    //result.append("<");
    //result.append(Format);
    //result.append("><");
    //result.append(FormatCorr);
    //result.append("><");
    //result.append(FormatCopy);
    //result.append(">");
    //Traitement de la chaine
    for (int i = 0; i < length; i++) {
        if (FormatCorr.at(i).toLatin1() == 'H') {
            result.append(" ");
            if (i + 4 <= length) {
                if (FormatCorr.at(i + 2).toLatin1() == 'H') {
                    bool ok;
                    result.append(QString::number(Value.mid(i,4).toInt(&ok, 16)));
                    //result.append(Value.mid(i,4));
                    i += 3;
                }
                else {
                    bool ok;
                    result.append(QString::number(Value.mid(i,2).toInt(&ok, 16)));
                    //result.append(Value.mid(i,2));
                    i += 1;
                }
            }
            else {
                bool ok;
                result.append(QString::number(Value.mid(i,2).toInt(&ok, 16)));
                //result.append(Value.mid(i,2));
                i += 1;
            }
            result.append(" ");
        }
        else if (FormatCorr.at(i).toLatin1() == 'B') {
            if (Value.at(i).toLatin1() == '1')
                result.append(" On ");
            else
                result.append(" Off ");
        }
        else if (FormatCorr.at(i).toLatin1() == 'A' || FormatCorr.at(i).toLatin1() == 'N') {
            result.append(" ");
            result.append(Value.at(i).toLatin1());
            while (FormatCorr.length() > i + 1) {
               if (FormatCorr.at(i).toLatin1() == FormatCorr.at(i + 1).toLatin1()) {
                   i++;
                   result.append(Value.at(i).toLatin1());
               }
               else {
                   result.append(" ");
                   break;
               }
            }
        }
        else {
            result.append(Value.at(i).toLatin1());
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return result;
}

//Fonction qui compose le message a l'aide des commandes
void LECommandeModules::onValidMesg()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QByteArray Message = "<";
    Message.append(QComboModul->currentData().toString().toLatin1());
    Message.append(QComboProv->currentData().toString().toLatin1());
    Message.append(QComboTypeCmd->currentData().toString().toLatin1());
    if (QComboCmd->currentIndex() != 0) Message.append(QComboCmd->currentData().toString().toLatin1());
    if (!LineEditInstruction->text().isEmpty()) Message.append(LineEditInstruction->text().toLatin1().toUpper());
    Message.append(">");
    LineEditInstruction->clear();
    emit Commande(Message);
    QComboCmd->setCurrentIndex(0);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Rend la sélection de provenance éditable ou inversément
void LECommandeModules::setProvModifiable (bool Modifiable, int index)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    ModifProv = Modifiable;
    indexProv = index;
    if (!Modifiable) {
        QComboProv->setCurrentIndex(index);
        QComboProv->setHidden(true);
        LabelProv->setText(tr("Provenance<br>") + QComboProv->currentText());
    }
    else {
        QComboProv->setCurrentIndex(0);
        QComboProv->setHidden(false);
        LabelProv->setText(tr("Provenance"));
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction pour appliquer et réappliquer tous les labels en fonction des demande de traduction
void LECommandeModules::retranslate(QString lang)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    LabelModule->setText(tr("Module"));
    LabelDesti->setText(tr("Destinataire"));
    LabelProv->setText(tr("Provenance"));
    LabelTypeCmd->setText(tr("Type d'instruction"));
    LabelCmd->setText(tr("Instruction"));
    LabelInstruction->setText(tr("Paramètre complémentaire"));
    BouttonValider->setText(tr("Valider"));
    //On récupère uniquement la langue et pas la country
    indLangue = lang.mid(0,2);
    //On peuple directement les comboboxModule
    QComboModul->clear();
    QComboModul->addItem("");
    QComboProv->clear();
    QComboProv->addItem("");
    QComboModul->setCurrentIndex(0);
    QDomElement element = docCommandeModule->documentElement();
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        //Prend le nom et vérifie l'initialisation du mode
        if (qde.tagName() == "Module")
        {
            QComboModul->addItem(qde.attribute("Nom:" + indLangue), qde.attribute("Cmd"));
        }
        //Prend le nom et vérifie l'initialisation du mode
        if (qde.tagName() == "Provenance")
        {
            QComboProv->addItem(qde.attribute("Nom:" + indLangue), qde.attribute("Cmd"));
        }
    }
    setProvModifiable(ModifProv, indexProv);

#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}
