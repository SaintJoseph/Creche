#ifndef CONVERTISSEURDECHEXA_H
#define CONVERTISSEURDECHEXA_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QBoxLayout>

class ConvertisseurDecHexa : public QDialog
{
    Q_OBJECT
public:
    explicit ConvertisseurDecHexa(QWidget *parent = 0);
    ~ConvertisseurDecHexa();

signals:
    void HexaVal(QString);

private slots:
    void onButtonClicked();

private:
    QPushButton *ButtonConvertir;
    QLineEdit *LineHexa, *LineDecimal;
    QHBoxLayout *Layout;

};

#endif // CONVERTISSEURDECHEXA_H
