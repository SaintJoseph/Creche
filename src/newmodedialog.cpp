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
void NewModeDialog::retranslate()
{
    ui->retranslateUi(this);
}
