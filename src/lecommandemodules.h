#ifndef LECOMMANDEMODULES_H
#define LECOMMANDEMODULES_H

#include <QWidget>
#include <QtXml>
#include <QComboBox>
#include <QByteArray>
#include <QDebug>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include "func_name.h"
#include "convertisseurdechexa.h"

class LECommandeModules : public QWidget
{
    Q_OBJECT
public:
    explicit LECommandeModules(QWidget *parent = 0);
    ~LECommandeModules();
    //Fonction pour appliquer et réappliquer tous les labels en fonction des demande de traduction
    void retranslate(QString lang);
    //Fonction qui renvois une phrase qui traduit le message donné en paramètre a l'aide des données xml
    QString Interpretation(QString Message);

signals:
    void Commande(QString);

private slots:
    void onModuleChanged(int index);
    void onTypeCmdChanged(int index);
    void onCmdChanged(int index);
    void onHexaCame(QString Val);
    void onValidMesg();

public slots:
    void setProvModifiable(bool Modifiable, int index = 0);

private:
    QVBoxLayout *LayoutEnvoisRecep, *LayoutDesti, *LayoutProv;
    QHBoxLayout *LayoutModules;
    QLabel *LabelModule, *LabelDesti, *LabelProv, *LabelTypeCmd, *LabelCmd, *LabelInstruction, *LabelAideParametre;
    QWidget *LiaisonArduino;
    QLineEdit *LineEditInstruction;
    QComboBox *QComboModul, *QComboTypeCmd, *QComboCmd, *QComboProv;
    QDomDocument *docCommandeModule;
    QPushButton *BouttonValider;
    ConvertisseurDecHexa *Convert;
    QString CommandeRecu;
    bool ModifProv;
    QString indLangue;
    int indexProv;

    QString Const_QString(int type);
    //Converti les valeurs hexa contenue dans Value en décimal
    QString ConvertHexaDec(QString Format, QString Value);

};

#endif // LECOMMANDEMODULES_H
