#include "editeurprog.h"

EditeurProg::EditeurProg(QWidget *parent) :
    QWidget(parent)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Initialisations des widgets composant le dock
    //Définition d'une police pour le dock
    QFont font("Verdana", 9);
    setFont(font);
    //Bouton Enregistrer
    BPEnregistrer = new QPushButton;
    BPEnregistrer->setFont(font);
    BPEnregistrer->setFixedHeight(22);
    //Bouton Annuler
    BPFermer = new QPushButton;
    BPFermer->setFont(font);
    BPFermer->setFixedHeight(22);
    //Layout principale
    LayoutPrinc = new QHBoxLayout;
    LayoutPrinc->setSpacing(1);
    LayoutPrinc->setContentsMargins(1,1,1,1);
    //Layout de menu
    LayoutMenu = new QVBoxLayout;
    LayoutMenu->setSpacing(1);
    LayoutMenu->setContentsMargins(0,0,0,0);
    //Genérateur de commande
    GenCommande = new LECommandeModules;
    GenCommande->setProvModifiable(true, 0);
    //Zone éditable
    ZoneEdition = new QTextEdit;
    ZoneEdition->setFont(font);
    //Label Nom du fichier
    LabelNomFile = new QLabel;
    LabelNomFile->setFont(font);
    LabelNomFile->setFixedHeight(22);
    LabelNomFile->setAlignment(Qt::AlignRight);
    //Label extension
    extFile = new QLabel(".cre");
    extFile->setFont(font);
    extFile->setFixedHeight(22);
    extFile->setAlignment(Qt::AlignLeft);
    //Ligne edite avec nom fichier
    LigneFile = new QLineEdit;
    LigneFile->setFont(font);
    LigneFile->setFixedHeight(22);
    LigneFile->setInputMask("NN");
    LigneFile->setFixedWidth(35);
    //Layout pour éditer le nom de fichier
    NomFile = new QHBoxLayout;
    NomFile->setContentsMargins(1,1,1,1);
    NomFile->setSpacing(0);
    NomFile->setAlignment(Qt::AlignCenter);

    NomFile->addWidget(LabelNomFile);
    NomFile->addWidget(LigneFile);
    NomFile->addWidget(extFile);
    LayoutMenu->addLayout(NomFile);
    LayoutMenu->addWidget(GenCommande);
    LayoutMenu->addWidget(BPEnregistrer);
    LayoutMenu->addWidget(BPFermer);
    LayoutPrinc->addLayout(LayoutMenu);
    LayoutPrinc->addWidget(ZoneEdition);
    setLayout(LayoutPrinc);

    retranslate("fr");

    connect(GenCommande, SIGNAL(Commande(QByteArray)),SLOT(onTextCome(QByteArray)));
    connect(BPEnregistrer, SIGNAL(clicked()), SLOT(onSaveButton()));
    //A chaque fois que le texte est modifier on adapte la mise en forme
    connect(ZoneEdition, SIGNAL(textChanged()), SLOT(onTextChange()));

#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

EditeurProg::~EditeurProg()
{
    delete GenCommande;
    delete BPEnregistrer;
    delete BPFermer;
    delete ZoneEdition;
    delete LabelNomFile;
    delete extFile;
    delete LigneFile;
    delete NomFile;
    delete LayoutMenu;
    delete LayoutMenu;
}

//Fonction pour appliquer et réappliquer tous les labels en fonction des demande de traduction
void EditeurProg::retranslate(QString lang)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    BPEnregistrer->setText(tr("Enregistrer"));
    BPFermer->setText(tr("Fermer"));
    LabelNomFile->setText("Nom du fichier : Exe_");
    GenCommande->retranslate(lang);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Ajout des commandes depuis le générateur
void EditeurProg::onTextCome(QByteArray message)
{
    ZoneEdition->moveCursor(QTextCursor::End);
    if (message.contains("<"))
        message.replace("<", "&#60;");
    if (message.contains(">"))
        message.replace(">", "&#62;");
    message.append("\n");
    ZoneEdition->insertHtml(message);
}

//Traitement du fichier et sauvegarde de celui-ci
void EditeurProg::onSaveButton()
{
    //On parcour le fichier ligne par ligne pour les traiters et les compléter dans l'ordre
    QStringList LigneParLigne = ZoneEdition->toPlainText().split("\n");
    QString TextFinal = "#Fichier Exe_" + LigneFile->text() + ".cre\n#Créé le " + QDate::currentDate().toString() + " T " + QTime::currentTime().toString() + "\n";
    int CompteurLigne = 1;
    foreach (QString Ligne, LigneParLigne) {
       if (!Ligne.isEmpty()) {
           if (Ligne.at(0) == QChar('#')) {
               TextFinal.append(Ligne + QString("\n\0"));
           }
           else if (Ligne.contains("#")) {
               TextFinal.append(Ligne.mid(Ligne.indexOf('#')) + QString("\n"));
           }
           else if (Ligne.contains("<") && Ligne.contains(">")) {
               QString NumLigne = QString::number(CompteurLigne, 16).toUpper();
               int taille = NumLigne.size();
               while (taille < 3) {
                   taille = NumLigne.size();
                   NumLigne.prepend(QString("0"));
               }
               TextFinal.append(NumLigne + QString(" ") + Ligne.mid(Ligne.indexOf('<')).remove(QChar(' '), Qt::CaseSensitive) + QString("\n"));
               CompteurLigne += 2;
           }
           else {
               TextFinal.append(QString("#") + Ligne + QString("\n\0"));
           }
       }
    }
    QString Nomfichier =  QFileDialog::getSaveFileName(this, tr("Enregistrer un fichier 'Effet lumineux Creche' "), "./Exe_" + LigneFile->text() + ".cre", tr("cre Files (*.cre);;Text Files (*.txt, *.xml, *.Xml, *.XML);;All Files (*.*)"));
    if (Nomfichier != "")
    {
        QFile file(Nomfichier);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << TextFinal;
        file.close();
    }
}

//Fonction qui formate le text affiché
void EditeurProg::onTextChange()
{
    //On déconnecte le changement du text, on apporte le nouveau formatage
    //Puis on reconnecte le changement de text
    ZoneEdition->disconnect();
    QTextCursor tcmov = ZoneEdition->textCursor(), tcligne = ZoneEdition->textCursor();
    tcmov.select(QTextCursor::WordUnderCursor);
    tcligne.select(QTextCursor::LineUnderCursor);
    QString word = tcmov.selectedText(), ligne = tcligne.selectedText();
    if (ligne.contains("#")) {
        ligne.prepend("<font color=blue>");
        ligne.append("</font>");
        tcligne.removeSelectedText();
        tcligne.insertHtml(ligne);
    }
    else {
        word.append("</font></i>");
        word.prepend("<i><font color=red>");
        tcmov.removeSelectedText();
        tcmov.insertHtml(word);
    }
    connect(ZoneEdition, SIGNAL(textChanged()), SLOT(onTextChange()));
}
