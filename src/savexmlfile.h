#ifndef SAVEXMLFILE_H
#define SAVEXMLFILE_H

#include <QDockWidget>
#include <QtXml>
#include <QMessageBox>
#include <QtGui>
#include <QWidget>
#include <QPixmap>
#include <QLayout>
#include <QBoxLayout>
#include <QSize>
#include "newmodedialog.h"
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include "Compile.h"
#include <QScrollArea>
#include "func_name.h"

class SaveXmlFile : public QDockWidget
{
    Q_OBJECT
public:
    explicit SaveXmlFile(const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0);
    ~SaveXmlFile();
    //Renvois le mode actif
    char ActiveMode();
    //Fonction qui ajoute un Key pour une led donnée dans l'arbre des états
    bool addKey(int led, QString Module, int pause, Compilation::LedType Type, QString parametre = "");
    //Fonction qui ajoute une led dans l'arbre des états
    bool addLed(int id, QString Module, const QString &Description = QString::null);
    //Fonction qui li et compile les donnes d'eclairage
    bool addValidation(int idState, bool Validation, bool Synchronisation);
    //Fonction qui applique et réapplique les labels pour introduire leur traduction quand c'est nécessaire
    void retranslate(QString lang);

signals:
    //Signal Pour la création d'un nouveau mode
    void NouveauModeCreer();
    //Signal que l'utilisateur change de mode
    void ChangeActiveMode();
    //Le mode actif envois un signal de mise a jour
    void CompilationUpdated();


public slots:
    void SupprimerUneInstance(Compilation *Instance = NULL);
    //Fonction qui ajoute une condition horaire a un mode existant
    void addCondition(CondHoraire *CondH, int indice);
    //Fonction pour supprimer les conditions horaire
    void SupprimerCondHoraire(int ValeurID);
    //Retourne un pointeur vers le mode actif
    Compilation* InstanceActive();
    //Sur demande des condition Horaire
    void onDemandeCHToPlainText(QPlainTextEdit *ZoneDeTexte);
    void onDemandeCHToPlainText(CondHoraire *CondH, int indice);

private slots:
    //Créeation d'une nouvelle instance de compilation
    void NewMode();
    //Supression d'une instance (encore à déterminer)
    void DeleteMode();
    //Changement de mode actif
    void ChangeModeActif(int Instance);
    //Le mode actif envois un signal de mise a jour
    void onCompilationUpdated();

private:
    QVBoxLayout *MainLayout, *LayoutBase;
    QPushButton *ButtonNewMode, *ButtonDelete;
    QLabel *LabelListeMode;
    QWidget *Base, *WidgetScrollArea;
    QScrollArea *ScrollArea;
    bool Flag1erInstanceNotInitialised;

    //Tableau avec les instances de compilations ouvertes
    static int ListeModeOuvert[5];
    static Compilation *ListeModeOuvertPoint[5];
    //Indique le mode actif
    static int ActiveModeNum;

    //Fonction Globale pour les const QString
    QString static Const_QString(int type);

    //Fonction de debugage
#ifdef DEBUG_COMANDSAVE
    void affichageqDebug(QString text);
#endif /* DEBUG_COMANDSAVE */
    
};

#endif // SAVEXMLFILE_H
