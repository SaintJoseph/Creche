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
    QMenu* languages;
    //Fonction qui applique et réapplique les labels pour introduire leur traduction quand c'est nécessaire
    void retranslate();
};

#endif // AMAINWINDOW_H
