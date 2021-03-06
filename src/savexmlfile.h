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
#include <QProgressBar>
#include <QListView>
#include "func_name.h"
#include "editeurprog.h"


#define PROGLIGNEITERATOR 3

//pour la fonction addToRAMTable(), mot clef lorsque l'adresse mémoire n'est pas spécifique
#define RAM_DEFAUT_VALUE 0

class SaveXmlFile : public QDockWidget
{
    Q_OBJECT
public:
    explicit SaveXmlFile(const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0);
    ~SaveXmlFile();
    //Fonction qui ajoute un Key pour une led donnée dans l'arbre des états
    bool addKey(int led, QString Module, int pause, Compilation::LedType Type, QString parametre = "");
    //Fonction qui ajoute une led dans l'arbre des états
    bool addLed(int id, QString Module, const QString &Description = QString::null);
    //Fonction qui li et compile les donnes d'eclairage
    bool addValidation(int idState, bool Validation, bool Synchronisation);
    //Fonction qui applique et réapplique les labels pour introduire leur traduction quand c'est nécessaire
    void retranslate(QString lang);
    //Fonction pour ajouter ou retourner une adresse RAM
    QString AddToRamTable(TableUsedRAM *TableRAM, QString Data, QString SpecialRef = RAM_DEFAUT_VALUE);

signals:
    //Signal Pour la création d'un nouveau mode
    void NouveauModeCreer();
    //Signal que l'utilisateur change de mode
    void ChangeActiveMode();
    //Le mode actif envois un signal de mise a jour
    void CompilationUpdated();
    //L'utilisateur lance la compilation
    void CompilationStart(bool);


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
    //Edition autorisée
    bool onEditionRequested();
    //Renvois le mode actif
    static char ActiveMode();

private slots:
    //Créeation d'une nouvelle instance de compilation
    void NewMode();
    //Supression d'une instance (encore à déterminer)
    void DeleteMode();
    //Changement de mode actif
    void ChangeModeActif(int Instance);
    //Le mode actif envois un signal de mise a jour
    void onCompilationUpdated();
    //Lancement de la compilation
    void onCompilationAsked();
    //Edition d'un fichier compilé
    void onEditionRequested(QModelIndex index);
    //Validation de la compilation
    void onValidationClicked();

private:
    QVBoxLayout *MainLayout, *LayoutBase;
    QHBoxLayout *LayoutCompilation;
    QPushButton *ButtonNewMode, *ButtonDelete, *ButtonCompiler, *ButtonValider;
    QLabel *LabelListeMode, *LabelFichiersCree;
    QWidget *Base, *WidgetScrollArea;
    QScrollArea *ScrollArea;
    QProgressBar *ProgressBarre;
    QListView *FichierCree;
    EditeurProg *Editeur;
    QStringListModel *ListeModel;
    CompilationPreAssemblage *CompilationAssemblage;
    bool Flag1erInstanceNotInitialised;
    //Mode compilation actif
    bool CompileMode = false;

    //Tableau avec les instances de compilations ouvertes
    static int ListeModeOuvert[5];
    static Compilation *ListeModeOuvertPoint[5];

    //Fonction Globale pour les const QString
    QString static Const_QString(int type);
    //Fonction pour changer l'affichage du dock en fonction si on est en édition ou en compilation
    void ChangeDockAffichage(bool mode);
    //Fonction de debugage
#ifdef DEBUG_COMANDSAVE
    void affichageqDebug(QString text);
#endif /* DEBUG_COMANDSAVE */
    //Fonction qui converti un nombre en Qstring de 4 carractères
    QString IntToQString(int Value);
    //Fichier commun 00
    void CompilationFichierCommun(DonneFichier *DataToFill, TableUsedRAM *TableRAM);
    //Controle des priorites
    bool ControlePriorite();
    //Compilation d'une demande d'appel de sous programme
    void CompilationAppelRegSousProg(QString variableRAM, QString fichier, int bouclage, DonneFichier *DataToFill, TableUsedRAM *TableRAM);
    //Compilation fichier commun controle présence CP
    void CompilationControlePrence(DonneFichier *DataToFill, TableUsedRAM *TableRAM);
};

#endif // SAVEXMLFILE_H
