#include "convertisseurdechexa.h"

ConvertisseurDecHexa::ConvertisseurDecHexa(QWidget *parent) :
    QWidget(parent)
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
    LineDecimal->setFixedWidth(50);
    LineHexa->setFixedWidth(50);
    LineHexa->setReadOnly(true);
    //Label
    Label = new QLabel("Deci. -> Hexa.");
    Label->setFont(font);
    Label->setFixedHeight(22);
    //Layout unique
    Layout = new QHBoxLayout;
    Layout->setMargin(1);
    Layout->setSpacing(2);
    Layout->addWidget(Label);
    Layout->addWidget(LineDecimal);
    Layout->addWidget(ButtonConvertir);
    Layout->addWidget(LineHexa);
    setLayout(Layout);

    connect(ButtonConvertir, SIGNAL(clicked()), SLOT(onButtonClicked()));
    connect(LineDecimal, SIGNAL(returnPressed()), SLOT(onButtonClicked()));
}

//Destructeur
ConvertisseurDecHexa::~ConvertisseurDecHexa() {
    delete ButtonConvertir;
    delete LineDecimal;
    delete LineHexa;
    delete Label;
    delete Layout;
}

//Convertisseur Hexa
void ConvertisseurDecHexa::onButtonClicked() {
    bool isInt;
    LineDecimal->text().toInt(&isInt);
    if(!isInt) {
        LineDecimal->clear();
    }
    else {
        LineHexa->clear();
        LineHexa->setText(QString::number(LineDecimal->text().toInt(),16));
        emit HexaVal(LineHexa->text());
    }
}
