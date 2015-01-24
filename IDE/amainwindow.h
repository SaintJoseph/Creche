#ifndef AMAINWINDOW_H
#define AMAINWINDOW_H

#include <QMainWindow>
#include "src/ComArduinoDock.h"
#include "src/savexmlfile.h"
#include "src/horodateur.h"

namespace Ui {
class AMainWindow;
}

class AMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AMainWindow(QWidget *parent = 0);
    ~AMainWindow();
    //Slot qui fait toute les actions de mise a jours suite à un changement de mode demandé par l'utilisateur
    void OnModeChange();
    //Fonction qui lance le changement de langue
    void setLanguage(QAction *action);

public slots:

private slots:
    //Slot pour affiche ou cacher le dock Horodateur
    void AfficherDockHorodateur();
    //Slot pour afficher ou cacher le dock Communication
    void AfficherDockComm();
    //Slot lors du lancement ou fin de la compilation
    void CompilationStarted(bool Start);
    //Slot lors de la connection d'un module Arduino
    void onArduinoConnected();

signals:

private:
    Ui::AMainWindow *ui;
    ComArduino *DockArduino;
    SaveXmlFile *DockSave;
    Horodateur *DockHorodateur;

    //Fonction qui met a jours la liste des langues (QAction)
    void fillLanguages();
    //Surchage de ChangeEvent(QEvent *event)
    void changeEvent(QEvent* event);
    // store translated widgets as members
    QMenu *Menulanguages, *MenuVisbilite;
    QAction *ActionHorodateur, *ActionComm;
    //Fonction qui applique et réapplique les labels pour introduire leur traduction quand c'est nécessaire
    void retranslate();
};

#endif // AMAINWINDOW_H
