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
    GenCommande->setMaximumWidth(500);
    GenCommande->setMaximumWidth(200);
    //Zone éditable
    ZoneEdition = new QTextEdit;
    ZoneEdition->setFont(font);
    ZoneEdition->setMaximumWidth(80%);
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
