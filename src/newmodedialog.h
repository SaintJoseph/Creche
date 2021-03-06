#ifndef NEWMODEDIALOG_H
#define NEWMODEDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QPixmap>
#include <QLabel>
#include <QtXml>
#include "func_name.h"
#include <QCheckBox>
#include <QHash>
#include <QHashIterator>

namespace Ui {
class NewModeDialog;
}

typedef QHash<QString, QCheckBox*> ListeModule;

class NewModeDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewModeDialog(QWidget *parent = 0);
    ~NewModeDialog();
    Ui::NewModeDialog *ui;
    //Fonction qui applique et réapplique les labels pour introduire leur traduction quand c'est nécessaire
    void retranslate(QString lang);
    //Renvois le nom du module pour l'indice indiqué, sauf s'il n'y en pas ou qu'il n'est pas coché
    QStringList askeModules();
    //Renvois La valeur de iniMode
    bool InitialisationMode();

public slots:
    //Faire passer en mode edition plus initialisation
    void FinInitialisation();
    //Mise a jour des champs principaux
    void UpdateAndShow(QString Nom, QString Description, int idMode, int priorite, QStringList Modules);

private Q_SLOTS:
    void OnBrowse();
    void OnNouveau();
    void AdaptId();
    void IDAdapter(int id);

signals:
    void BoutonNouveau(QString, QString, QString, int, int);
    void AdapterID();
    void BoutonValider(QString, QString, int, int);

private:
    QString Chemin;
    QString indLangue;
    QDomDocument *docCommandeModule;
    ListeModule listemodules;
    bool IniMode;

};

#endif // NEWMODEDIALOG_H
