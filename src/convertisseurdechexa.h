#ifndef CONVERTISSEURDECHEXA_H
#define CONVERTISSEURDECHEXA_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QBoxLayout>
#include <QLabel>

class ConvertisseurDecHexa : public QWidget
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
    QLabel *Label;

};

#endif // CONVERTISSEURDECHEXA_H
