#include "amainwindow.h"
#include "ui_amainwindow.h"

AMainWindow::AMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AMainWindow)
{
    ui->setupUi(this);
}

AMainWindow::~AMainWindow()
{
    delete ui;
}
