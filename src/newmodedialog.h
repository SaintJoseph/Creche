#ifndef NEWMODEDIALOG_H
#define NEWMODEDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QPixmap>
#include <QLabel>
#include "func_name.h"

namespace Ui {
class NewModeDialog;
}

class NewModeDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewModeDialog(QWidget *parent = 0);
    ~NewModeDialog();
    Ui::NewModeDialog *ui;
    //Fonction qui applique et réapplique les labels pour introduire leur traduction quand c'est nécessaire
    void retranslate();

private Q_SLOTS:
    void OnBrowse();
    void OnNouveau();
    void AdaptId();
    void IDAdapter(int id);

signals:
    void BoutonNouveau(QString, QString, QString, int, int);
    void AdapterID();

private:
    QString Chemin;
};

#endif // NEWMODEDIALOG_H
