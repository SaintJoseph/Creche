#include "convertisseurdechexa.h"

ConvertisseurDecHexa::ConvertisseurDecHexa(QWidget *parent) :
    QDialog(parent)
{
    //Initialisations des widgets composant le dock
    //Définition d'une police pour le dock
    QFont font("Verdana", 9);
    setFont(font);
    //Boutton convertion
    ButtonConvertir = new QPushButton("->");
    ButtonConvertir->setFont(font);
    ButtonConvertir->setFixedHeight(22);
    ButtonConvertir->setFixedWidth(22);
    //line Edit
    LineDecimal = new QLineEdit;
    LineHexa = new QLineEdit;
    LineDecimal->setFont(font);
    LineHexa->setFont(font);
    LineDecimal->setFixedHeight(22);
    LineHexa->setFixedHeight(22);
    LineHexa->setReadOnly(true);
    //Layout unique
    Layout = new QHBoxLayout;
    Layout->setMargin(1);
    Layout->setSpacing(2);
    Layout->addWidget(LineDecimal);
    Layout->addWidget(ButtonConvertir);
    Layout->addWidget(LineHexa);
    setLayout(Layout);
    setWindowTitle(tr("Convertisseur Décimal -> Hexadécimal"));
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint |Qt::WindowTitleHint);

    connect(ButtonConvertir, SIGNAL(clicked()), SLOT(onButtonClicked()));
}

//Destructeur
ConvertisseurDecHexa::~ConvertisseurDecHexa() {
    delete ButtonConvertir;
    delete LineDecimal;
    delete LineHexa;
    delete Layout;
}

//Convertisseur Hexa
void ConvertisseurDecHexa::onButtonClicked() {
    bool isInt;
    int leChiffre = LineDecimal->text().toInt(&isInt);
    if(!isInt) {
        LineDecimal->clear();
    }
    else {
        LineHexa->clear();
        LineHexa->setText(QString::number(LineDecimal->text().toInt(),16));
        emit HexaVal(LineHexa->text());
    }
}
