#ifndef EDITEURPROG_H
#define EDITEURPROG_H

#include <QWidget>
#include "lecommandemodules.h"
#include "Compile.h"
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QTextEdit>
#include <QFont>
#include <QLineEdit>
#include <QMessageBox>
#include <QTime>
#include <QFileDialog>
#include <QTextCursor>
#include <QRegExp>

class EditeurProg : public QWidget
{
    Q_OBJECT

public:
    EditeurProg(DonneFichier *Fichier = NULL, QWidget * parent = 0);
    ~EditeurProg();
    //Fonction pour appliquer et réappliquer tous les labels en fonction des demande de traduction
    void retranslate(QString lang);
    //Fonction de chargement d'un nouveau fichier compilé
    void setDonneeFichier(DonneFichier *Fichier);

signals:

public slots:

private slots:
    void onTextCome(QByteArray message);
    void onSaveButton();
    //A chaque modification de la zone d'édition on reformate l'affichage
    void onTextChange();

private:
    LECommandeModules *GenCommande;
    QPushButton *BPEnregistrer, *BPFermer;
    QHBoxLayout *LayoutPrinc, *NomFile;
    QVBoxLayout *LayoutMenu;
    QTextEdit *ZoneEdition;
    QLabel *LabelNomFile, *extFile;
    QLineEdit *LigneFile;
    DonneFichier *FichierDonne;

};

#endif // EDITEURPROG_H
