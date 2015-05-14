#include "newmodedialog.h"
#include "ui_newmodedialog.h"

NewModeDialog::NewModeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewModeDialog)
{
    ui->setupUi(this);

    connect(ui->Ouvrir, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->ParcourirDossier, SIGNAL(clicked()), this, SLOT(OnBrowse()));
    connect(ui->Nouveau, SIGNAL(clicked()), this, SLOT(OnNouveau()));
    connect(ui->NomMode, SIGNAL(editingFinished()), this, SLOT(AdaptId()));

    QLabel *label_img = new QLabel(this);
    QPixmap *Icone = new QPixmap(":Creche");
    label_img->setPixmap(*Icone);
    ui->IconeLayout->addWidget(label_img, Qt::AlignLeft);

    //Boolean qui indique si la class est en mode initialisation (false) ou édition (true)
    IniMode = false;

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

    ui->LayoutSelectionModule->setAlignment(Qt::AlignHCenter);

    QDomElement element = docCommandeModule->documentElement();
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        //Prend le nom et vérifie l'initialisation du mode
        if (qde.tagName() == "Module" && qde.hasAttribute("Checked"))
        {
            QCheckBox *Cbox = new QCheckBox(qde.attribute("Nom:" + indLangue));
            ui->LayoutSelectionModule->addWidget(Cbox);
            Cbox->setChecked(qde.attribute("Checked").toDouble());
            listemodules.insert(qde.attribute("Cmd"), Cbox);
        }
    }

    retranslate("fr");
}

NewModeDialog::~NewModeDialog()
{
    delete ui;
}

//Lorsque l'utilisateur clique sur Browse, Sélectionne un répertoire, puis la fonction crée un nom de fichier Avec le Nom du Mode
void NewModeDialog::OnBrowse()
{
    QString Chemin = QFileDialog::getExistingDirectory(this, tr("Selectionnez un dossier pour sauvegarder le fichier d'Effets lumineux de creche"));
    Chemin += "/";
    Chemin += ui->NomMode->text();
    Chemin += ".Xml";
    ui->CheminFichier->setText(Chemin);
}

void NewModeDialog::OnNouveau()
{
    if (IniMode){
        emit BoutonValider(ui->NomMode->text(), ui->DescriptionTexte->toPlainText(), ui->IdValue->value(), ui->PrioriteValue->value());
    }
    else {
        if (!ui->CheminFichier->text().endsWith('/') && !ui->CheminFichier->text().isEmpty())
        {
            QString CheminFichier = ui->CheminFichier->text();
            if (!CheminFichier.contains(".xml"))
                if (!CheminFichier.contains(".Xml"))
                    if (!CheminFichier.contains(".XML"))
                        CheminFichier.append(".Xml");
            emit BoutonNouveau(ui->NomMode->text(), ui->DescriptionTexte->toPlainText(), CheminFichier, ui->IdValue->value(), ui->PrioriteValue->value());
        }
        else
        {
            QMessageBox::warning(this, tr("Nom de fichier manquant"), tr("Le chemin de fichier ou le nom de fichier est manquant."), QMessageBox::Ok, QMessageBox::Ok);
        }
    }
}

void NewModeDialog::AdaptId()
{
    emit AdapterID();
}

void NewModeDialog::IDAdapter(int id)
{
    ui->IdValue->setValue(id);
    //on deconnecte en même temps le signal source pour éviter
    //de multiple tentative d'adapter l'id
    disconnect(ui->NomMode,0,0,0);
}

//Fonction qui applique et réapplique les labels pour introduire leur traduction quand c'est nécessaire
void NewModeDialog::retranslate(QString lang)
{
    ui->retranslateUi(this);
    //On récupère uniquement la langue et pas la country
    indLangue = lang.mid(0,2);
    QDomElement element = docCommandeModule->documentElement();
    for(QDomElement qde = element.firstChildElement(); !qde.isNull(); qde = qde.nextSiblingElement())
    {
        //Prend le nom et vérifie l'initialisation du mode
        if (qde.tagName() == "Module" && qde.hasAttribute("Checked"))
        {
            QCheckBox *Cbox = listemodules.value(qde.attribute("Cmd"));
            Cbox->setText(qde.attribute("Nom:" + indLangue));
        }
    }
}

//Renvois le nom du module pour l'indice indiqué, sauf s'il n'y en pas ou qu'il n'est pas coché
QStringList NewModeDialog::askeModules()
{
    QStringList liste;
    QHashIterator<QString, QCheckBox*> ListeItereateur(listemodules);
    while (ListeItereateur.hasNext()) {
        ListeItereateur.next();
        QCheckBox *Cbox = ListeItereateur.value();
        if (Cbox->isChecked())
           liste.append(ListeItereateur.key());
    }
    return liste;
}

//Faire passer en mode edition plus initialisation
void NewModeDialog::FinInitialisation()
{
    //On cache la modification du chemin d'acces, car inutile pour la suite
    ui->CheminFichier->hide();
    ui->LabelNomFichier->hide();
    ui->ParcourirDossier->hide();
    //On change la fonctionnalité des deux boutons, (nom et connexion)
    ui->Ouvrir->setText(tr("Cancel"));
    ui->Nouveau->setText(tr("Valider"));
    IniMode = true;
}

//Mise a jour des champs principaux
void NewModeDialog::UpdateAndShow(QString Nom, QString Description, int idMode, int priorite){
    ui->DescriptionTexte->setText(Description);
    ui->NomMode->setText(Nom);
    ui->PrioriteValue->setValue(priorite);
    ui->IdValue->setValue(idMode);
    //on termine par l'affichage de la fenetre
    show();
}

//Renvois la valeur de IniMode
bool NewModeDialog::InitialisationMode()
{
    return IniMode;
}
