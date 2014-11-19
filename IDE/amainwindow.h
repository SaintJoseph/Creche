#ifndef AMAINWINDOW_H
#define AMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class AMainWindow;
}

class AMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AMainWindow(QWidget *parent = 0);
    ~AMainWindow();

private:
    Ui::AMainWindow *ui;
};

#endif // AMAINWINDOW_H
