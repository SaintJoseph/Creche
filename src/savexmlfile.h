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
    //Fonction qui ajoute un state dans l'arbre des états mais pas les entrées du state, et renvois l'id créé
    bool addState(int id, int pause = 0);
    //Fonction qui ajoute un Progressif dans l'arbre des états pour un state donné, et renvois l'id + 1 si le progressif est bien créé
    bool addProgressif(Compilation::LedColor id, int idState, int level, const QString &Nom, const QString &Description = QString::null);
    //Fonction qui ajoute un ensemble TOR dans l'arbre des états pour un state donné, et renvois l'id + 1 si le ensemble TOR est bien créé
    bool addTOR(int idState, bool tor[16], const QString &Nom, const QString &Description);
    //Fonction qui li et compile les donnes d'eclairage
    bool addValidation(int idState, bool Validation, bool Synchronisation);
    //Fonction qui retourne les condition horaire de linstance active
    static QString ReturneCondHoraire();
    //Fonction qui applique et réapplique les labels pour introduire leur traduction quand c'est nécessaire
    void retranslate(QString lang);

signals:
    //Signal Pour la création d'un nouveau mode
    void NouveauModeCreer();
    //Signal que l'utilisateur change de mode
    void ChangeActiveMode();

public slots:
    void SupprimerUneInstance(Compilation *Instance = NULL);
    //Fonction qui ajoute une condition horaire a un mode existant
    void addCondition(CondHoraire *CondH);
    //Fonction pour supprimer les conditions horaire
    void SupprimerCondHoraire();

private slots:
    //Créeation d'une nouvelle instance de compilation
    void NewMode();
    //Supression d'une instance (encore à déterminer)
    void DeleteMode();
    //Changement de mode actif
    void ChangeModeActif(int Instance);

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
