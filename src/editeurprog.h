#ifndef EDITEURPROG_H
#define EDITEURPROG_H

#include <QWidget>
#include "lecommandemodules.h"
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QTextEdit>
#include <QFont>
#include <QLineEdit>

class EditeurProg : public QWidget
{
    Q_OBJECT

public:
    EditeurProg(QWidget * parent = 0);
    ~EditeurProg();
    //Fonction pour appliquer et réappliquer tous les labels en fonction des demande de traduction
    void retranslate(QString lang);

signals:

public slots:

private slots:

private:
    LECommandeModules *GenCommande;
    QPushButton *BPEnregistrer, *BPFermer;
    QHBoxLayout *LayoutPrinc, *NomFile;
    QVBoxLayout *LayoutMenu;
    QTextEdit *ZoneEdition;
    QLabel *LabelNomFile, *extFile;
    QLineEdit *LigneFile;

};

#endif // EDITEURPROG_H
