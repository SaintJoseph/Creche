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
#include <QTextStream>
#include "func_name.h"

#define TAG_MODE "Mode"
#define ATTRIBUT_ID "id"
#define ATTRIBUT_PRIORITY "Priorite"
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
#define ATTRIBUT_SYNC "Sync"
#define ATTRIBUT_VALID "Validation"
#define ATTRIBUT_MODULE "Module"

//Structure pour facilité l'échange des données condition Horaire
struct CondHoraire{
    int DMois, DJour, DJourSem, DHeure, DMinute,
        EMois, EJour, EJourSem, EHeure, EMinute;
};

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
    //Liste pour les couleurs de led Progressives
    enum LedColor {Blanc1Progressif = 1, Blanc2Progressif, Blanc3Progressif, RougeProgressif, OrangeProgressif, BleuProgressif};
    //Fonction qui ajoute une condition horaire a un mode existant
    bool addCondition(CondHoraire *CondH);
    //Fonction qui ajoute un state dans l'arbre des états mais pas les entrées du state, et renvois l'id créé
    bool addState(int id = 0, int pause = 0);
    //Fonction qui ajoute un Progressif dans l'arbre des états pour un state donné, et renvois l'id + 1 si le progressif est bien créé
    bool addProgressif(LedColor id, int idState, int level, const QString &Nom, const QString &Description = QString::null);
    //Fonction qui ajoute un ensemble TOR dans l'arbre des états pour un state donné, et renvois l'id + 1 si le ensemble TOR est bien créé
    bool addTOR(int id,int idState, bool tor[16], const QString &Nom, const QString &Description);
    //Fonction qui ajoute une validation de synchronisation a un state donné (Maximum 4 validations / mode et 1 synchronisation / mode)
    bool addValidation(int idState, bool Validation, bool Synchronisation);
    //Fonction qui retourne la valeur de l'instance
    int InstanceValue();
    //Fonction qui li et compile toutes les conditions de toutes les instances existante du programme
    //En sortie on peu directement l'écrire sur le port de l'Arduino (après synchronisation)
    static QByteArray LireToutesConditions();
    //Fonction qui li et compile toutes les conditions de toutes les instances existante du programme
    //En sortie on peu directement l'écrire sur le port de l'Arduino (après synchronisation)
    static QByteArray LireToutData();
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
    //Fonction qui retourneles condition horaire de l'instance
    QString ReturnCondHoraire();
    //Fonction qui supprime les condition Horaire
    void SupprimeCondHoraire();
    //Fonction qui applique et réapplique les labels pour introduire leur traduction quand c'est nécessaire
    void retranslate(QString lang);

signals:
    void IDproposer(int);
    //Signal emit lors d'un click sur le widget
    void MouseClickEvent(int);
    //Signal pour une instance qui demande a être supprimée
    void DeleteMe(Compilation *);

public slots:
    void initialisationNouveauDom(QString Nom, QString Description, QString FichierNom, int idMode, int priorite);
    void initialisationFichierDom();
    void ProposeID();
    void showNewModeDialog();
    void DemndeLectureCH(CondHoraire *CondH);

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
    static char nbInstance;
    char Instance;

    //Traitement des chaines de carractère
    QString Const_QString(int type);
    //Controle de l'état de montage d'un mode (l'état est enregistrer avec la couleur des LED)
    void ControleCompilation();
    //Fonction pour ajouter un élément xml
    QDomElement addElement( QDomDocument *doc, QDomNode *node, const QString &tag, const QString &value);
    QDomElement addElement( QDomDocument *doc, QDomNode node, const QString &tag, const QString &value);
    //Fonction qui enlève un élément xml
    void delElement(QDomDocument *doc, const QString &tag, const QString &attribu = QString::null, int val = 0);
    //Fonction qui retourne l'arbre Xml sous forme de qstring
    QString DomDocument();
    //Fonction utiliser par la compilation des data, lecture des level et des rampes
    bool compileProgressif(QByteArray *byteArray, QDomElement *element, int *position);
    //Fonction qui li et compile les donnes d'eclairage
    QByteArray LireData();
    //Fonction qui lit et compile toute les conditions d'un mode pour envois vers Arduino
    QByteArray LireConditions();
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
