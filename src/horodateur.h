#ifndef HORODATEUR_H
#define HORODATEUR_H

#include "Compile.h"
#include "savexmlfile.h"
#include <QDockWidget>
#include <QWidget>
#include <QMessageBox>
#include <QPixmap>
#include <QLayout>
#include <QBoxLayout>
#include <QSize>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QSpinBox>
#include <QScrollArea>
#include <QTimeEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QDebug>
#include <QDateTime>
#include <QPlainTextEdit>
#include "func_name.h"

class Horodateur : public QDockWidget
{
    Q_OBJECT
public:
    explicit Horodateur(const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0);
    ~Horodateur();
    //Fonction pour remettre a zero les conditions horaires
    void ResetAffichage();
    //Fonction pour appliquer et réappliquer les labels suite a une demande de traduction
    void retranslate();

    //Enumeration pour le type de condition horaire souhaitée
    enum Type {Periode, Journalier, Hebdomadaire, Vide, View};


signals:
    //Envois des conditions horaires
    void SendCondHoraire(CondHoraire *);
    //Demande pour supprimer condition horaire
    void DemandeSuppCondHoraire();

public slots:

private slots:
    //Slot pour choisir un type de condition horaire
    void ChangeType(int index);
    //Slot qui prepare les condition horaire pour la compilation
    void PrepareToSend();
    //Slot pour le boutton delete
    void BouttonDeleteClick();

protected:

private:

    QVBoxLayout *MainLayout, *LayoutDateEdite, *LayoutJourSelect, *LayoutBase, *LayoutTimeEdit;
    QHBoxLayout *LayoutStartDate, *LayoutEndDate;
    QLabel *LabelStartDate, *LabelEndDate, *LabelJourSelect, *LabelTimeStart, *LabelTimeEnd;
    QPushButton *ButtonStartDate, *ButtonEndDate, *ButtonValider, *ButtonDelete;
    QScrollArea *ScrollArea;
    QWidget *Base, *DateEdite, *SelectJour, *TimeEdit, *WidgetScrollArea;
    QComboBox *ComboBoxSelect, *JourSelect;
    QDateEdit *DateEditeStart, *DateEditeEnd;
    QTimeEdit *TimeEditStart, *TimeEditEnd;
    QPlainTextEdit *TextView;

};

#endif // HORODATEUR_H
