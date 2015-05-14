#ifndef COMPILE_H
#define COMPILE_H

#include <QtXml>
#include <QMessageBox>
#include <QtGui>
#include <QWidget>
#include <QPixmap>
#include <QFileDialog>
#include "newmodedialog.h"
#include <QBoxLayout>
#include <QLabel>
#include "hled.h"
#include <QPainterPath>
#include <QPainter>
#include <QPlainTextEdit>
#include <QTextStream>
#include <QHash>
#include <QHashIterator>
#include "func_name.h"

#define TAG_MODE "Mode"
#define TAG_ID "id"
#define TAG_PRIORITY "Priorite"
#define TAG_DESCRIPTION "Description"
#define TAG_CONDITION "Condition"
#define TAG_SARTTIME "StartTime"
#define TAG_ENDTIME "EndTime"
#define TAG_MOIS "Mois"
#define TAG_JOUR "Jour"
#define TAG_JOURSEM "JourSem"
#define TAG_HEURE "Heure"
#define TAG_MIN "Min"
#define TAG_DATA "Data"
#define TAG_STATE "State"
#define TAG_PAUSE "Pause"
#define TAG_LED "Led"
#define TAG_NOM "Nom"
#define TAG_MODULE "Module"
#define TAG_PARAMETRE "Parametre"
#define ATTRIBUT_SYNC "Sync"
#define ATTRIBUT_VALID "Validation"
#define ATTRIBUT_MODULE "Module"
#define ATTRIBUT_TYPE "Type"
#define ATTRIBUT_ID "id"

//Enumeration pour le type de condition horaire souhaitée
typedef enum TypeCondHoraire{Periode = 0, Journalier, Hebdomadaire, Vide, View} TypeCondHoraire;

//Structure pour facilité l'échange des données condition Horaire
typedef struct CondHoraire{
    int DMois, DJour, DJourSem, DHeure, DMinute,
        EMois, EJour, EJourSem, EHeure, EMinute;
    TypeCondHoraire Type;
} CondHoraire;

//Structure avec les infos pour 1 fichier compilé
typedef struct DonneFichier{
    //Liste des instructions à la suite dans le fichier, pas de numéro de ligne, pas de commentaire
    QStringList ListeIstruction;
    //Instruction avec un commentaire associé, a insérer dans le fichier compilé
    QHash<QString, QString> Commentaire;
    //Description fichier, nom complet du mode
    QString ModeNom;
} DonneFichier;

//Index des adresses mémoire utilsé pour éviter la sur-écriture et éventuellement les doublons ("0001", "M02TL") => adresse 1, température alu, module 2
typedef QHash<QString, QString> TableUsedRAM;

//Structure avec toutes les donnée pour finaliser une compilation
typedef struct CompilationPreAssemblage{
    //Table Hash avec les noms de fichiers associé aux données de fichier compilés
    QHash <QString, DonneFichier*> DonneDesFichiers;
    //Index des adresses mémoire utilsé pour éviter la sur-écriture et éventuellement les doublons ("0001", "M02TL") => adresse 1, température alu, module 2
    TableUsedRAM Table;
    //Liste des modules utilisé
    QStringList ListeModules;
} CompilationPreAssemblage;

//Class personnalisée pour le traitement des fichier Xml généré, lu et enregistré
/*Chaque instance est prévue pour contenir les données d'un mode "Effet lumineux" entier.
 *Dans le constructeur on peut soit créer un nouvel arbre Xml que l'on complétera avec d'autres fonction
 *Soit ouvrir un fichier Xml avec un arbre déjà complété. Ce qui permet de modifier un arbre existant
 *La class contient des méthodes menbres et des méthodes statique qui sont chargées de la compilation proprement dite.
 *Les méthodes statiques font la compilation complète des données avant envois vers le module Arduino.
 *Les données sont codées puis envoyés sous le format QByteArray
 */

class Compilation : public QWidget
{
    Q_OBJECT

public:
    explicit Compilation(QWidget *parent = 0);
    ~Compilation();
    //Liste pour les les types de fonctionnement les
    enum LedType {TOR, Progressif, ProgressifDebut, ClignotantFranc, ClignotantProgr, Vacillement};
    //Fonction qui ajoute une condition horaire a un mode existant
    bool addCondition(CondHoraire *CondH, int indice);
    //Fonction qui ajoute un state dans l'arbre des états mais pas les entrées du state, et renvois l'id créé
    bool addkey(int led, QString Module, int pause, LedType Type, QString param = "");
    //Fonction qui ajoute un Progressif dans l'arbre des états pour un state donné, et renvois l'id + 1 si le progressif est bien créé
    bool addLed(int id, QString Module, const QString &Description = QString::null);
    //Fonction qui ajoute un ensemble TOR dans l'arbre des états pour un state donné, et renvois l'id + 1 si le ensemble TOR est bien créé
    bool addValidation(int idState, bool Validation, bool Synchronisation);
    //Fonction qui retourne la valeur de l'instance
    int InstanceValue();
    //Fonction qui propose un ID pour un nouveau mode(recherche un ID pas encore utilise
    static int NouvelId();
    //Les deux fonctions statics suivante permettent à l'aide d'une boucle de détruire toute les instances existance
    //donc de sauvegarder un fichier pour chaque instance existante.
    //Fonction qui retourne le nombre d'instance en cours d'execution
    static int NbInstance();
    //Fonction qui renvois un pointeur vers la dernière instance créé
    static Compilation *InstanceActive();
    //Fonction qui renvois un pointeur vers l'instance d'indice x
    static Compilation *ReturnInstance(int x);
    //Fonction qui allume et eteint les LED
    void TurneLedOn(bool On = true);
    //Fonction qui supprime les condition Horaire
    void SupprimeCondHoraire(int ValeurID);
    //Fonction qui applique et réapplique les labels pour introduire leur traduction quand c'est nécessaire
    void retranslate(QString lang);
    //Fonction qui compile les conditions horaires
    bool CompilationCH(DonneFichier *DataToFill, TableUsedRAM *TableRAM);
    //Fonction pour ajouter l'utilisation d'une adresse RAM
    QString AddToRamTable(TableUsedRAM *TableRAM, QString Data);
    //Fonction qui renvois la liste des modules qu'elle utilise
    QStringList ListeDesModules();
    //Retourne la priorité d'un mode
    int InstancePriorite();
    //Retourne L'indice d'un mode
    int InstanceIndice();
    //Retourne le nom d'un mode
    QString InstanceNom();
    //Retourne La description d'un mode
    QString InstanceDescription();
    //Renvois le mode actif
    static int ModeActif();

    //Sauvegarde automatique active
    bool saveAuto = true;

signals:
    void IDproposer(int);
    //Signal emit lors d'un click sur le widget
    void MouseClickEvent(int);
    //Signal pour une instance qui demande a être supprimée
    void DeleteMe(Compilation *);
    //Mise a jour du mode
    void CompilationUpdated();
    //Demande l'affichage de NewModeDialogue une nouvelle fois
    void AfficheNewMode(QString Nom, QString Description, int idMode, int priorite);

public slots:
    void initialisationNouveauDom(QString Nom, QString Description, QString FichierNom, int idMode, int priorite);
    void initialisationFichierDom();
    //Fonction qui met à jour les paramètres principaux d'un mode lumineux
    void UpdateDom(QString Nom, QString Description, int idMode, int priorite);
    void ProposeID();
    void showNewModeDialog();
    //Retourne les condition Horaire sous forme de structure ou de texte
    void DemndeLectureCH(CondHoraire *CondH, int indice);
    void DemndeLectureCH(QPlainTextEdit *CondH);
    bool onControleCompilation();
    //Fonction pour modifier le mode actif
    static void setModeActif(int ModeActive);

protected:
    //re-implement processing of moue events
    void mouseDoubleClickEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *);

private:
    //Objets Qt, privé
    QDomDocument *doc;
    QMessageBox *enregistrer;
    QString Nomfichier;
    NewModeDialog *NewMode;
    HLed *LedIni, *LedCond, *LedState;
    QVBoxLayout *layoutAappliquer;
    QHBoxLayout *ZoneTitre, *ZoneInfo;
    QLabel *LabelNom, *LabelId;

    //Liste des instances pour une compilation de tous les modes à la fois
    static Compilation *ListeInstance[10];
    static int nbInstance;
    int Instance;
    static int ActiveModeNum;

    //Traitement des chaines de carractère
    QString Const_QString(int type);
    //Controle de l'état de montage d'un mode (l'état est enregistrer avec la couleur des LED)
    bool ControleCompilation();
    bool ControleInstance;
    //Fonction pour ajouter un élément xml
    QDomElement addElement( QDomDocument *doc, QDomNode *node, const QString &tag, const QString &value);
    QDomElement addElement( QDomDocument *doc, QDomNode node, const QString &tag, const QString &value);
    //Fonction qui enlève un élément xml
    void delElement(QDomDocument *doc, const QString &tag, const QString &attribu = QString::null, int val = 0);
    //Fonction qui retourne l'arbre Xml sous forme de qstring
    QString DomDocument();
    //Fonction qui retourne le nombre de jours d'un mois donné
    int nbjourmois(int mois);
    //Fonction qui converti un nombre en Qstring de 4 carractères
    QString IntToQString(int Value);
    //Fonction qui compile une condition sur la date
    void CompilationDate(CondHoraire *CondH, DonneFichier *DataToFill, TableUsedRAM *TableRAM);
    //Fonction qui compile une condition sur l'heure
    void CompilationHeure(CondHoraire *CondH, DonneFichier *DataToFill, TableUsedRAM *TableRAM);
    //Fonction qui compile une condition sur les jours de la semaine
    void CompilationSem(CondHoraire *CondH, DonneFichier *DataToFill, TableUsedRAM *TableRAM);

};

#endif // COMPILE_H

/*

<?xml version='1.0' encoding='UTF-8'?>
<!DOCTYPE Light_Effect_Crib>
<!--Fichier avec le détails des effets lumineux programmé pour la creche de l'Eglise Saint Joseph-->
<Mode id="" Priorite="">
 <Nom></Nom>
 <Description></Description>
 <Module></Module>
 <Data>
  <Led id="" Module="">
   <Description></Description>
   <State id="">
    <Parametre></Parametre>
    <Pause></Pause>
   </State>
  </led>
  <Validation>
   <Description></Description>
   <Pause></Pause>
  </Validation>
 </Data>
 <Condition id="">
  <StartTime>
   <Mois></Mois>
   <Jour></Jour>
   <JourSem></JourSem>
   <Heure></Heure>
   <Min></Min>
  </StartTime>
  <EndTime>
   <Mois></Mois>
   <Jour></Jour>
   <JourSem></JourSem>
   <Heure></Heure>
   <Min></Min>
  </EndTime>
 </Condition>
</Mode>
*/
