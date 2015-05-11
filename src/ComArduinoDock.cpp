#include "ComArduinoDock.h"
#include <QTranslator>
#include <QtCore>
#include "qextserialport.h"
#include "qextserialenumerator.h"
#include <QMessageBox>
#include <QColor>

ComArduino::ComArduino(const QString & title, QWidget *parent , Qt::WindowFlags flags) :
    QDockWidget(title, parent, flags)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Initialisations des widgets composant le dock
    //Définition d'une police pour le dock
    QFont font("Verdana", 9);
    setFont(font);
    //Tab du Dock
    TabDock = new QTabWidget;
    TabDock->setMaximumWidth(500);
    TabDock->setMinimumWidth(300);
    TabDock->setContentsMargins(1,1,1,1);
    TabDock->setStyleSheet("QTabBar::tab{height:20px}");
    TabDock->setFont(font);
    //Widget de base pour accueillir le layout Principale
    Base = new QWidget;
    //Widget qui contient la partie liaison Arduino
    LiaisonArduino = new QWidget;
    //Widget qui permet l'échange de donnée avec Arduino
    Parametre = new QWidget;
    //Widget qui accueil le layout avec la zone pour la console
    TerminalZone = new QWidget;
    //Label pour afficher la date et l'heure du PC
    LabelDateEtHeure = new QLabel;
    LabelDateEtHeure->setText(tr("Date<br>Heure"));
    LabelDateEtHeure->setFont(font);
    //Label Baude Rate
    LabelBaudeRate = new QLabel(tr("Baude Rate"));
    LabelBaudeRate->setFont(font);
    //Label Parity
    LabelParity = new QLabel(tr("Parity"));
    LabelParity->setFont(font);
    //Label DataBit
    LabelDataBit = new QLabel(tr("Data Bit"));
    LabelDataBit->setFont(font);
    //Label Stop Bit
    LabelStopBit = new QLabel(tr("Stop Bit"));
    LabelStopBit->setFont(font);
    //Label Query mode
    LabelQueryMode = new QLabel(tr("Query Mode"));
    LabelQueryMode->setFont(font);
    //Label ports
    LabelPort = new QLabel(tr("Liste des ports"));
    LabelPort->setFont(font);
    //Label time out
    LabelTimeOut = new QLabel(tr("Time Out"));
    LabelTimeOut->setFont(font);
    //Insertion de l'icone déco dans le layout IconeElectronique via un label
    label_img = new QLabel;
    QPixmap Icone(":Arduino");
    label_img->setPixmap(Icone);
    label_img->setMaximumWidth(120);
    //Layout Horizontale pour équilibrer le dock
    LayoutPrincH = new QHBoxLayout;
    LayoutPrincH->setContentsMargins(1,1,1,1);
    LayoutPrincH->setSpacing(1);
    //Layout du 1er onglet
    EspaceDockLiaison = new QHBoxLayout;
    EspaceDockLiaison->setContentsMargins(1,1,1,1);
    EspaceDockLiaison->setSpacing(1);
    //Layout Horizontale pour l'affichage de la date, l'heure
    DateTimeConnect = new QVBoxLayout;
    DateTimeConnect->setAlignment(Qt::AlignVCenter);
    DateTimeConnect->setSpacing(1);
    DateTimeConnect->setContentsMargins(1,1,1,1);
    //Layout pour placer le terminal et la zone de commande manuelle
    EspaceTerminal = new QVBoxLayout;
    EspaceTerminal->setContentsMargins(1,1,1,1);
    EspaceTerminal->setSpacing(1);
    //Layout H de commande manuel de l'arduino
    LayoutCommandeMan = new QHBoxLayout;
    LayoutCommandeMan->setSpacing(1);
    //Layout pour accueillir les paramètre de connection Partie droite
    LayoutParamD = new QVBoxLayout;
    LayoutParamD->setContentsMargins(1,1,1,1);
    LayoutParamD->setSpacing(1);
    //Layout pour accueillir les paramètre de connextion Partie Gauche
    LayoutParamG = new QVBoxLayout;
    LayoutParamG->setContentsMargins(1,1,1,1);
    LayoutParamG->setSpacing(1);
    //Layout horizontale pour l'espace paramètre
    EspaceParam = new QHBoxLayout;
    EspaceParam->setContentsMargins(1,1,1,1);
    EspaceParam->setSpacing(1);
    //Led Vert pour la connection
    LedVerte = new HLed;
    LedVerte->turnOff();
    LedVerte->setFixedHeight(22);
    LedVerte->setFixedWidth(22);
    //Terminal de réception USBSériale
    TerminalUSBSerial = new QPlainTextEdit;
    TerminalUSBSerial->insertPlainText(Const_QString(0));
    TerminalUSBSerial->setReadOnly(true);
    TerminalUSBSerial->setFont(font);
    //Boutton pour envoyer a l'arduino une instruction
    ButtonSend = new QPushButton(tr("Envoyer"));
    ButtonSend->setMinimumWidth(50);
    ButtonSend->setFixedHeight(22);
    ButtonSend->setFont(font);
    //Ligne d'édition pour introduire une commande manuelle
    InstructionManuel = new QLineEdit;
    InstructionManuel->setFixedHeight(22);
    InstructionManuel->setFont(font);
    //ComboBox Baude Rate
    BaudeRate = new QComboBox;
    BaudeRate->setFont(font);
    BaudeRate->addItem("1200", BAUD1200);
    BaudeRate->addItem("2400", BAUD2400);
    BaudeRate->addItem("4800", BAUD4800);
    BaudeRate->addItem("9600", BAUD9600);
    BaudeRate->addItem("19200", BAUD19200);
    BaudeRate->setCurrentIndex(3);
    //ComboBox Parity Box
    ParityBox = new QComboBox;
    ParityBox->setFont(font);
    ParityBox->addItem("NONE", PAR_NONE);
    ParityBox->addItem("ODD", PAR_ODD);
    ParityBox->addItem("EVEN", PAR_EVEN);
    //ComboBox Databits Box
    DataBitBox = new QComboBox;
    DataBitBox->addItem("5", DATA_5);
    DataBitBox->addItem("6", DATA_6);
    DataBitBox->addItem("7", DATA_7);
    DataBitBox->addItem("8", DATA_8);
    DataBitBox->setCurrentIndex(3);
    //ComboBox Stop Bit Box
    StopBitBox = new QComboBox;
    StopBitBox->setFont(font);
    StopBitBox->addItem("1", STOP_1);
    StopBitBox->addItem("2", STOP_2);
    //ComboBox Query Mode Box
    QueryModeBox = new QComboBox;
    QueryModeBox->setFont(font);
    QueryModeBox->addItem(tr("Polling","Méthode utilisée"), QextSerialPort::Polling);
    QueryModeBox->addItem(tr("EventDriven","Méthode Utilisée"), QextSerialPort::EventDriven);
    //Spin Box time Out Box
    TimeOutBox = new QSpinBox;
    TimeOutBox->setFont(font);
    TimeOutBox->setSuffix("ms");
    TimeOutBox->setMaximum(1000);
    TimeOutBox->setMinimum(0);
    TimeOutBox->setValue(10);
    TimeOutBox->setSingleStep(10);
    //ComboBox Port Box list
    PortBox = new QComboBox;
    PortBox->setFont(font);
    //Widget avec l'éditeur de commande
    WidgetEnvoisRecep = new LECommandeModules;
    WidgetEnvoisRecep->setProvModifiable(false,1);

    //Dessin du dock en attribuant les références de parents enfants
             //Ajout du Label avec la date et l'heure
             DateTimeConnect->addWidget(LabelDateEtHeure);
             //Ajout de l'icone électronique
             DateTimeConnect->addWidget(label_img,0,Qt::AlignCenter);
             //Ajout du comboBox de sélection du port et précédé de son label
             DateTimeConnect->addWidget(LabelPort);
             DateTimeConnect->addWidget(PortBox);
          //Ajout de la zone Date et Heure
          EspaceDockLiaison->addLayout(DateTimeConnect);
          //Ajout de la zone envois recep Dans l'espace DockLiaison
          EspaceDockLiaison->addWidget(WidgetEnvoisRecep);
       //Application du layout au widget liaison Arduino
       LiaisonArduino->setLayout(EspaceDockLiaison);
    //Création d'un ongle avec le widget Liaison Arduino
    TabDock->addTab(LiaisonArduino, tr("Envoi/Reception"));
    TabDock->setTabToolTip(0,tr("Envois et réception de donnée avec le module Arduino"));
             //Ajout du comboBox Baude Rate Au menu param gauche et précédé de son label
             LayoutParamG->addWidget(LabelBaudeRate);
             LayoutParamG->addWidget(BaudeRate);
             //Ajout du comboBox Parity Box au menu param gauche et précédé de son label
             LayoutParamG->addWidget(LabelParity);
             LayoutParamG->addWidget(ParityBox);
             //Ajout du comboBox StopBit au menu param gauche et précédé de son label
             LayoutParamG->addWidget(LabelStopBit);
             LayoutParamG->addWidget(StopBitBox);
             //Ajout du comboBox DataBit Box au menu param droite et précédé de son label
             LayoutParamD->addWidget(LabelDataBit);
             LayoutParamD->addWidget(DataBitBox);
             //Ajout du comboBox QueryMode au menu param droite et précédé de son label
             LayoutParamD->addWidget(LabelQueryMode);
             LayoutParamD->addWidget(QueryModeBox);
             //Ajout du spinBox TimeOut au menu param droite et précédé de son label
             LayoutParamD->addWidget(LabelTimeOut);
             LayoutParamD->addWidget(TimeOutBox);
          //Application du layout param gauche au layout param et précédé de son label
          EspaceParam->addLayout(LayoutParamG);
          //Application du layout param droite au layout param et précédé de son label
          EspaceParam->addLayout(LayoutParamD);
       //Application du layout param au widget param
       Parametre->setLayout(EspaceParam);
    //Création d'un ongle avec le widget Envois Recept
    TabDock->addTab(Parametre, tr("Paramètres"));
    TabDock->setTabToolTip(1,tr("Outil pour s'assurer que le PC communique bien avec l'Arduino"));
    //Ajout dans le Layout Principale du tab
    LayoutPrincH->addWidget(TabDock);
          //Ajout de la Led verte de connection
          LayoutCommandeMan->addWidget(LedVerte);
          //Ajout de la ligne d'instruction dans le layout H
          LayoutCommandeMan->addWidget(InstructionManuel);
          //Ajout du boutton d'envois a coté de la ligne d'instruction
          LayoutCommandeMan->addWidget(ButtonSend);
       //Ajout du plainTextEdit dans le layout de l'espace Terminal
       EspaceTerminal->addWidget(TerminalUSBSerial);
       //Ajout de la zone de commande manuel dans l'espace terminal
       EspaceTerminal->addLayout(LayoutCommandeMan);
       //Application du layout de l'EspaceTerminal dans le widget terminal
       TerminalZone->setLayout(EspaceTerminal);
    //Ajout dans le layout principale
    LayoutPrincH->addWidget(TerminalZone);
    //Application du layout Principale dans le widget de base
    Base->setLayout(LayoutPrincH);
    //Application du layout Principale au dock
    setWidget(Base);
    //Zone autorisée pour le dock
    setAllowedAreas(Qt::BottomDockWidgetArea);

    timer = new QTimer(this);
    timer->setInterval(40);

    //Timer qui déclanche un signal pour rafraichir l'heure 1 fois par seconde
    ClockUpdate = new QTimer(this);
    ClockUpdate->setInterval(1000);
    ClockUpdate->start();
    connect(ClockUpdate, SIGNAL(timeout()), this, SLOT(onTimeUpdate()));

    //Variable pour l'affichage de la date et l'heure
    dateEtHeure = new QDateTime();

    //Création du port usbsérie
    PortSettings settings = {BAUD9600, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10};
    port = new QextSerialPort(PortBox->currentText(), settings, QextSerialPort::Polling);

    //Liste des port usbsérie disponible
    enumerator = new QextSerialEnumerator(this);
    enumerator->setUpNotifications();

    //Aplication d'un style au dock
    setStyleSheet("QDockWidget::title {text-align: centre; background: rgb(0,189,138); color: rgb(0,0,138); border-style: inset; border-width: 2px; border-radius: 2px; border-color: rgb(0,0,138); font: 8pt; padding: 0px; height: 20px} QDockWidget::float-button { border: 1px solid; background: rgb(0,189,138); padding: 0px} QDockWidget::float-button:hover { background: rgb(0,0,138) } QDockWidget {border-style: outset; border-width: 2px; border-radius: 4px; border-color: rgb(0,0,138)}");
    setFeatures(QDockWidget::NoDockWidgetFeatures);
    setFeatures(QDockWidget::DockWidgetMovable);
    setFeatures(QDockWidget::DockWidgetFloatable);

    //Connection des actionneurs divers au différentes actions
    //Paramettrage du port serie
    connect(BaudeRate, SIGNAL(currentIndexChanged(int)), SLOT(onBaudRateChanged(int)));
    connect(ParityBox, SIGNAL(currentIndexChanged(int)),SLOT(onParityChanged(int)));
    connect(DataBitBox, SIGNAL(currentIndexChanged(int)), SLOT(onDataBitsChanged(int)));
    connect(StopBitBox, SIGNAL(currentIndexChanged(int)), SLOT(onStopBitsChanged(int)));
    connect(QueryModeBox, SIGNAL(currentIndexChanged(int)), SLOT(onQueryModeChanged(int)));
    connect(TimeOutBox, SIGNAL(valueChanged(int)), SLOT(onTimeoutChanged(int)));
    connect(PortBox, SIGNAL(currentIndexChanged(QString)), SLOT(onPortNameChanged(QString)));
    //Envoi de la commande introduite dans le champs par l'utilisateur
    connect(ButtonSend, SIGNAL(clicked()), SLOT(onSendButtonClicked()));
    //Réception de donnée sur le port série
    connect(timer, SIGNAL(timeout()), SLOT(onReadyRead()));
    connect(port, SIGNAL(readyRead()), SLOT(onReadyRead()));
    //Signal pour lancer l'ouverture ou la fermeture du port série
    //Il est possible de mettre un bouton qui agit sur le meme slot.
    //Inutile ici, car le signal est générer automatiquement au branchement du cable USB
    connect(this, SIGNAL(genereOpenCloseEvent()), SLOT(onOpenCloseButtonClicked()));
    //Signal return pressed des zones d'édition
    connect(InstructionManuel, SIGNAL(returnPressed()), SLOT(onSendButtonClicked()));
    //Signal provenant du générateur de commande
    connect(WidgetEnvoisRecep, SIGNAL(Commande(QByteArray)), SLOT(onSendButtonClicked(QByteArray)));

#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

ComArduino::~ComArduino()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    delete WidgetEnvoisRecep;
    delete port;
    delete LedVerte;
    delete LabelDateEtHeure;
    delete LabelBaudeRate;
    delete LabelParity;
    delete LabelDataBit;
    delete LabelStopBit;
    delete LabelQueryMode;
    delete LabelPort;
    delete LabelTimeOut;
    delete label_img;
    delete TerminalUSBSerial;
    delete ButtonSend;
    delete InstructionManuel;
    delete BaudeRate;
    delete ParityBox;
    delete DataBitBox;
    delete StopBitBox;
    delete QueryModeBox;
    delete TimeOutBox;
    delete PortBox;
    delete LayoutParamD;
    delete LayoutParamG;
    delete EspaceParam;
    delete Parametre;
    delete LayoutCommandeMan;
    delete DateTimeConnect;
    delete EspaceDockLiaison;
    delete LiaisonArduino;
    delete TabDock;
    delete EspaceTerminal;
    delete TerminalZone;
    delete LayoutPrincH;
    delete Base;

#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction de mise à jour de l'heure
void ComArduino::onTimeUpdate()
{
    QFont font("Verdana", 9);
    dateEtHeure->setDate(QDate::currentDate());
    dateEtHeure->setTime(QTime::currentTime());
    LabelDateEtHeure->setText(dateEtHeure->toString(tr("ddd dd MMM yyyy<br>hh:mm:ss ap","Date et Heure en mode 12h (am/pm), Exemple: Sam. 24 Aout 2013 (a la ligne) 10:25:14 am")));
    LabelDateEtHeure->setFont(font);
    ClockUpdate->start();
    //On  en profite pour mettre la liste des ports a jour. Permet de réaliser dans la seconde
    //une connextion avec un module qui vient d'etre branché. -> Suppression du bouton Open/Close
    onPortAddedOrRemoved();
    if (!port->isOpen() && PortBox->count() > 0) onOpenCloseButtonClicked(OPEN_PORT);
    if (port->isOpen() && PortBox->count() == 0) onOpenCloseButtonClicked(CLOSE_PORT);
}

void ComArduino::onPortNameChanged(const QString & )
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    if (port->isOpen()) {
        emit genereOpenCloseEvent();
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

void ComArduino::onBaudRateChanged(int idx)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    port->setBaudRate((BaudRateType)BaudeRate->itemData(idx).toInt());
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

void ComArduino::onParityChanged(int idx)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    port->setParity((ParityType)ParityBox->itemData(idx).toInt());
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

void ComArduino::onDataBitsChanged(int idx)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    port->setDataBits((DataBitsType)DataBitBox->itemData(idx).toInt());
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

void ComArduino::onStopBitsChanged(int idx)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    port->setStopBits((StopBitsType)StopBitBox->itemData(idx).toInt());
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}


void ComArduino::onQueryModeChanged(int idx)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    port->setQueryMode((QextSerialPort::QueryMode)QueryModeBox->itemData(idx).toInt());
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

void ComArduino::onTimeoutChanged(int val)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    port->setTimeout(val);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

void ComArduino::onOpenCloseButtonClicked()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    onOpenCloseButtonClicked(0);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

void ComArduino::onOpenCloseButtonClicked(char action)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    if (PortBox->currentText().contains("ttyACM"))
    {
        if (!port->isOpen())
        {
            if (action == OPEN_PORT || action == 0)
            {
                port->setPortName(PortBox->currentText());
                port->open(QIODevice::ReadWrite);
                emit ConnectEvent();
            }
        }
        else
        {
            if (action == CLOSE_PORT || action == 0)
            {
                port->close();
            }
        }
    }
    else
    {
        port->close();
    }
    //If using polling mode, we need a QTimer
    if (port->isOpen() && port->queryMode() == QextSerialPort::Polling)
        timer->start();
    else
        timer->stop();
    //update led's status
    LedVerte->turnOn(port->isOpen());

#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui envois des données sur le port série
void ComArduino::onSendButtonClicked(QByteArray commandeToSend)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //1er cas on envois le message introduit par l'utilisateur
    if (port->isOpen()){
        if (commandeToSend == "")
        {
            //Si il n'y a pas d'instruction manuel et pas de commandeToSend alors on regarde les messages système
            port->write(InstructionManuel->text().toLatin1());
            TerminalUSBSerial->moveCursor(QTextCursor::End);
            TerminalUSBSerial->insertPlainText(InstructionManuel->text().toLatin1());
            TerminalUSBSerial->moveCursor(QTextCursor::End);
            TerminalUSBSerial->insertPlainText("\n");
            TerminalUSBSerial->moveCursor(QTextCursor::End);
            TerminalUSBSerial->insertPlainText(WidgetEnvoisRecep->Interpretation(InstructionManuel->text()));
            InstructionManuel->clear();

        }
        //2ieme cas on envois une commande venant d'une autre fonction
        else {
            port->write(commandeToSend.data());
            TerminalUSBSerial->moveCursor(QTextCursor::End);
            TerminalUSBSerial->insertPlainText(commandeToSend);
            TerminalUSBSerial->moveCursor(QTextCursor::End);
            TerminalUSBSerial->insertPlainText("\n");
            TerminalUSBSerial->moveCursor(QTextCursor::End);
            TerminalUSBSerial->insertPlainText(WidgetEnvoisRecep->Interpretation(QString(commandeToSend)));
        }
    }
    port->flush();
    InstructionManuel->clear();
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui lit tout ce qui se présente sur le port série, lorsque la connection est ouverte
//Quand un signal de synchronisation se présente, il est traité et on affiche un texte adapté
void ComArduino::onReadyRead()
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    if (port->bytesAvailable()) {
        TerminalUSBSerial->moveCursor(QTextCursor::End);
        QString texteRecu = QString::fromLatin1(port->readAll());
        TerminalUSBSerial->insertPlainText(texteRecu);
        TerminalUSBSerial->moveCursor(QTextCursor::End);
        //Insérer ici le traitement des commandes reçue du système
        //On affiche la commande en brute, puis elle est interprétée
        //et on affiche une phare traduisant le message
        for (int i = 0; i < texteRecu.length(); i++) {
            if (CommandeEnCours)
                CommandeRecu.append(texteRecu.at(i));
            if (CommandeEnCours && texteRecu.at(i) == '>') {
                CommandeEnCours = false;
                TerminalUSBSerial->insertPlainText(WidgetEnvoisRecep->Interpretation(CommandeRecu));
            }
            if (!CommandeEnCours && texteRecu.at(i) == '<') {
                CommandeEnCours = true;
                CommandeRecu = "<";
            }
        }
    }
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Fonction qui met a jour la liste des ports disponibles
void ComArduino::onPortAddedOrRemoved()
{
    QString current = PortBox->currentText();
    //Création d'une lite vide que l'on va remplir avec tout les modules arduino connecté
    QStringList PortArduino;
    PortBox->blockSignals(true);
    PortBox->clear();
    //On fait le tri, pour ne garder que les ports avec modules Arduino
    foreach (QextPortInfo info, QextSerialEnumerator::getPorts()) {
        QString portl = info.portName;
        if (portl.contains("ttyACM"))
        {
            if (!PortArduino.contains(info.physName))
            {
                //Si c'est bien un Arduino "ttyACM.", on l'ajoute a la liste des modules Arduino connecté
                 PortArduino.append(info.physName);
            }
        }
    }
    PortBox->addItems(PortArduino);
    if (PortBox->findText(current) != -1)
    {
        PortBox->setCurrentIndex(PortBox->findText(current));
    }
    if (PortBox->currentText() != current && !port->isOpen())
    {
        TerminalUSBSerial->clear();
        TerminalUSBSerial->insertPlainText(Const_QString(0));

    }
    PortBox->blockSignals(false);
}

//Fonction Globale pour les const QString
QString ComArduino::Const_QString(int type)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    //Variable globale pour les const QString
    static const char* TitreEtConstante[] = {
       QT_TR_NOOP("Lecture de l'Arduino...")
    };
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    return tr(TitreEtConstante[type]);
}

//Fonction pour appliquer et réappliquer tous les labels en fonction des demande de traduction
void ComArduino::retranslate(QString lang)
{
#ifdef DEBUG_COMANDSAVE
    std::cout << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
    QueryModeBox->setItemText(0,tr("Polling","Méthode utilisée"));
    QueryModeBox->setItemText(1,tr("EventDriven","Méthode Utilisée"));
    ButtonSend->setText(tr("Envoyer"));
    LabelBaudeRate->setText(tr("Baude Rate"));
    LabelParity->setText(tr("Parity"));
    LabelDataBit->setText(tr("Data Bit"));
    LabelStopBit->setText(tr("Stop Bit"));
    LabelQueryMode->setText(tr("Query Mode"));
    LabelPort->setText(tr("Liste des ports"));
    LabelTimeOut->setText(tr("Time Out"));
    TabDock->setTabText(0, tr("Envoi/Reception"));
    TabDock->setTabToolTip(0, tr("Envois et réception de donnée avec le module Arduino"));
    TabDock->setTabText(1, tr("Paramètres"));
    TabDock->setTabToolTip(1, tr("Outil pour s'assurer que le PC communique bien avec l'Arduino"));
    WidgetEnvoisRecep->retranslate(lang);
#ifdef DEBUG_COMANDSAVE
    std::cout << "/" << func_name << std::endl;
#endif /* DEBUG_COMANDSAVE */
}

//Retourne si le dock est connecté à un module arduino
bool ComArduino::isConnected()
{
    return port->isOpen();
}
