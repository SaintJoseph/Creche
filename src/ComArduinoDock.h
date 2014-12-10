#ifndef COMARDUINODOCK_H
#define COMARDUINODOCK_H

#include <QDockWidget>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QTime>
#include <QDate>
#include <QTimer>
#include <QDateTime>
#include <QByteArray>
#include <QDebug>
#include <QLayout>
#include <QSize>
#include <QTabWidget>
#include <QLabel>
#include <QBoxLayout>
#include "qextserialenumerator.h"
#include "qextserialport.h"
#include <QSignalMapper>
#include "hled.h"
#include "Compile.h"
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include "func_name.h"
#include "lecommandemodules.h"

//Message pour l'ouverture ou la fermeture du port serie
#define OPEN_PORT 1
#define CLOSE_PORT 2

class ComArduino : public QDockWidget
{
        Q_OBJECT

 public:
    explicit ComArduino(const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0);
    ~ComArduino();
    void onOpenCloseButtonClicked(char action);
    //Fonction pour appliquer et réappliquer tous les labels en fonction des demande de traduction
    void retranslate(QString lang);

signals:
    void genereOpenCloseEvent();

protected:

private slots:
    void onPortNameChanged(const QString &name);
    void onBaudRateChanged(int idx);
    void onParityChanged(int idx);
    void onDataBitsChanged(int idx);
    void onStopBitsChanged(int idx);
    void onQueryModeChanged(int idx);
    void onTimeoutChanged(int val);
    void onOpenCloseButtonClicked();
    void onSendButtonClicked(QByteArray commandeToSend);
    void onReadyRead();
    void onPortAddedOrRemoved();
    void onTimeUpdate();

private:
    QTimer *timer, *ClockUpdate;
    QVBoxLayout *BoxTest;
    QextSerialPort *port;
    QextSerialEnumerator *enumerator;
    QDateTime *dateEtHeure;
    QVBoxLayout *DateTimeConnect, *MenuGauche, *EspaceTerminal, *LayoutParamG, *LayoutParamD;
    QHBoxLayout *LayoutPrincH, *LayoutCommandeMan, *EspaceDockLiaison, *EspaceParam;
    QLabel *LabelDateEtHeure, *label_img, *LabelBaudeRate, *LabelParity, *LabelDataBit, *LabelStopBit,\
    *LabelQueryMode, *LabelTimeOut, *LabelPort;
    QTabWidget *TabDock;
    QWidget *LiaisonArduino, *Parametre, *Base, *TerminalZone;
    HLed *LedVerte;
    QPlainTextEdit *TerminalUSBSerial;
    QPushButton *ButtonSend;
    QLineEdit *InstructionManuel;
    QComboBox *BaudeRate, *ParityBox, *DataBitBox, *StopBitBox, *QueryModeBox, *PortBox;
    QSpinBox *TimeOutBox;
    QDomDocument *docCommandeModule;
    QString CommandeRecu;
    LECommandeModules *WidgetEnvoisRecep;
    bool CommandeEnCours;

    QString Const_QString(int type);
    QString Interpretation(QString Message);
    //Converti les valeurs hexa contenue dans Value en décimal
    QString ConvertHexaDec(QString Format, QString Value);

};

#endif // COMARDUINODOCK_H
