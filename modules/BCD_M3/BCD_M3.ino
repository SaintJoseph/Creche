/*
 *                                        XX
 *                                        XX
 *                                  XXXXXXXXXXXXXX
 *                                  XXXXXXXXXXXXXX
 *                                        XX
 *                                        XX
 *                                        XX
 *                                        XX
 *                                        XX
 *
 ********************************************************************************
 **********PROGRAMME BCD POUR L'ECLAIRAGE DE LA CRECHE DE SAINT JOSEPH,**********
 ********************************************************************************
 *                                SUR ARDUINO UNO,                              *
 *       A UTILISER UNIQUEMENT AVEC LA PROTOSHIELD ADAPTE A CE PROGRAMME        *
 *                                                                              *
 *                      Cette carte est composée:                               *
 *          -un afficheur 15 segments (température et puissance instantanée)    *
 *          -2 Boutons poussoir                                                 *
 *          -3 pin d'pour l'alimentation venant de la méga2560 et fournit 3,3V  *
 *          -WatchDog Harware avec un 555, avec une tempos de 9s                *
 *                                                                              *
 ********************************************************************************
 *                                                                              *
 *          FONCTION:                                                           *
 *Module qui affiche en alternance la témpérature du profilé alu                *
 *et en alternance avec la puissance instanténée de l'alimentation.             *
 *Il fournit également le 3,3V nécéssaire au fonctionnement du NRf24l01+        *
 *de la carte méga2560.                                                         *
 *                                                                              *
 ********************************************************************************
 *   Structure des messages recu par le port serie                              *
 **Message spéciaux <Xav> avec a = R:Reboot a = A:Tout, a = 1-6:led couleur     *
 *  v = 1:Allumé v = 0:eteint                                                   *
 **Demande lecture <La> avec                                                    *
 **Ce module est le numero 3                                                    *
 ********************************************************************************
 *              copyright      REALISE PAR E.CROONEN,                           *
 ********************************************************************************
 
 
 
//*******************************************************************************
//*******************************************************************************
//                                ATTENTION                                     *
//les messages radios et PC changent (identifiant; valeur; fin de com)          *
//*******************************************************************************
//*******************************************************************************
 */

//PROGMEM const String devise = "Mon Dieu faite que je serve uniquement a votre plus grande gloire";

//---------------------------INCLUDE--------------------------------

#include <Creche.h>

//---------------------------VARIABLES--------------------------------

//Varibles pour l'afficheur 15segments
//Variable avec les pins correspondant a chaque segments
const byte pinled[15] = {
  2,11,12,10,13,4,3,5,8,7,9,6,A1,A0,A3};
//1er centaine 2 a 8 dizaine, de 9 a 15 unite
bool etat[15] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int count = 0; // variable de test
long alternance = 0; // Dernière action
#define change 1000 // Temps d'affichage de chaque chiffre

//Variables a afficher
byte temp = 20; // température a afficher
byte puis = 0; // puissance absorbée

//Variables pour les échanges de données structurées par sériale
String rxCmd ; // Commande reçue (sans délimiteurs)
String txCmd ; // Commande à transmettre
String fCmd; //Message reçu par les cannaux de communication

//Variable qui indique si le système est allumé ou éteint
boolean StateAlim1240;
boolean LastStateAlim1240;

//repère pour l'affichage en alternance
byte stateaff = 0;
//repère pour l'affichage spéciale en cas de stop
byte indiceLed = 0;
const bool etatdu[10][7] = { //Table de convertion BCD
  {
    1,1,1,0,1,1,1  }
  ,
  {
    0,0,1,0,0,0,1  }
  ,
  {
    0,1,1,1,1,1,0  }
  ,
  {
    0,1,1,1,0,1,1  }
  ,
  {
    1,0,1,1,0,0,1  }
  ,
  {
    1,1,0,1,0,1,1  }
  ,
  {
    1,1,0,1,1,1,1  }
  ,
  {
    0,1,1,0,0,0,1  }
  ,
  {
    1,1,1,1,1,1,1  }
  ,
  {
    1,1,1,1,0,1,1  }
};

BOUTON Bouton1(A2), Bouton2(A5);

//---------------------------------FONCTIONS------------------------------------

// traitement du car recu
void TreatChar(char car, Mode canal)
{
  switch (car)
  {
  case '<' :
    fCmd = "";
    break ;
  case '>' :
    AdressageMessage();
    break ;
  default:
    fCmd = fCmd + car ;
  }
}

//Traitement d'un message valid reçu
void AdressageMessage()
{
    txCmd = "";
    //On fait une analyse du destinataire et de l'envoyeur pour faire suivre le message vers la bonne adresse
    byte desti, envoyeur;
    //On relève les addresse des destinataires et envoyeur
    //Dans le cas All la valeur destinataire doit etre inscrite mais est inutile
    StringToByte(desti,fCmd, 1);
    StringToByte(envoyeur,fCmd, 4);
    switch (fCmd[0])
    {
      case 'M' : case 'm' :
        if (desti == 3) {
           txCmd = fCmd[3] + ByteToString(envoyeur) + "M03";
           //Les 6 premier carractère du message concerne l'adressage, le module prend cette information séparement pour un éventuel retour de message
           TreatCommand(fCmd.substring(6)) ;
        }
        break ;
      case 'A' : case 'a' :
           txCmd = fCmd[3] + ByteToString(envoyeur) + "M03";
           //Les 6 premier carractère du message concer l'adressage, le module prend cette information séparement pour un éventuel retour de message
           TreatCommand(fCmd.substring(6));
        break ;
      default:
        //Le message ne correspond a rien
        txCmd = "M00M03ErrMessageM3";
        Send();
        break;
    }
}

// Transmission de la commande. le message est dirigé vers le bon récepteur
void Send() {
    //On fait une analyse du destinataire et de l'envoyeur pour faire suivre le message vers la bonne adresse
    byte desti, envoyeur;
    //On relève les addresse des destinataires et envoyeur
    //Dans le cas All la valeur destinataire doit etre inscrite mais est utilité
    StringToByte(desti,txCmd, 1);
    StringToByte(envoyeur,txCmd, 4);
    String cmd = '<' + txCmd + '>';
    Serial.println(cmd);
}

void CmdError(String type)
// Doit être appelé quand il y a une erreur dans rxCmd
{
  txCmd = txCmd + "Z ERR " + type;
  Send();
  txCmd = "";
}

//Traitement des commandes propre pour ce module
void TreatCommand(String cmd) {
  rxCmd = cmd;
  byte desti;
  switch (rxCmd[0])
  {
    case 'X' : case 'x' : 
      //Message reçu : <(X opération)...>
      DonneSpeciaux() ;
      break ;
    case 'L' : case 'l' : 
      //Message reçu : <(Lecture)...>
      LectureVariable() ;
      break ;
    case 'E' : case 'e' : 
      //Message reçu : <(Ecriture)...>
      EcritureVariable() ;
      break ;
    case 'D' : case 'd' : 
      //Message reçu : <(Demarrage)...>
      //Un module x vient de redémrrer peut-etre quil faut lui envoyer une commande de mise a jour
      StringToByte(desti,rxCmd,1);
      break ;
    case 'P' : case 'p' : 
      //Message reçu : <(Présent)...>
      //Un module x vient de répondre présent suite a une requete
      StringToByte(desti,rxCmd,1);
      break ;
    default:   
      CmdError("type d'operation (X.L.E.D.P)");
      break ;      
  }
}

//Ecritures des variables
void EcritureVariable() {
  byte TempByte;
  switch (rxCmd[1])
  {
    case 'P' : case 'p' : 
      //Message reçu : <(Puissance)...>
      StringToByte(TempByte,rxCmd,2);
      puis = TempByte;
      break ;
    case 'T' : case 't' : 
      //Message reçu : <(Température)...>
      StringToByte(TempByte,rxCmd,2);
      temp = TempByte;
      break ;
    case 'A' : case 'a' :
      //Message reçu : <(Alternance)...>
      if (rxCmd[2] == '1') {
         Bouton1.state = 1;
      }
      else {
         Bouton1.state = 0;
      }
      break ;
    default:   
      CmdError("ecriture variable (P.T.A)");
      break ;      
  }
}

//Fonction qui réagit a des commandes spéciales
void DonneSpeciaux() {
  byte led;
  switch (rxCmd[1])
  {
    case 'R' : case 'r' :
       //Message reçu: <(X opération)(Reboot)>
       //Format ""
       while ( rxCmd[1] < 't');
       break;
    case 'A' : case 'a' :
       //Message reçu: <(X opération)(All)(...)>
       //Format "B"
       switch (rxCmd[2]) {
          case '1':
             //Message reçu: <(X opération)(All)(on)>
             for (int i = 0; i < 15; i++) {
                digitalWrite(pinled[i], LOW);
             }
             break;
          case '0':
             //Message reçu: <(X opération)(All)(off)>
             for (int i = 0; i < 15; i++) {
                digitalWrite(pinled[i], HIGH);
             }
             break;
          default:
             CmdError("All preciser OnOff (1.0)");
       }
       break ;
    case 'B' : case 'b' :
       //Message reçu: <(X opération)(Boutton)>
       //Format "B"
       switch (rxCmd[2]) {
          case '1':
             //Message reçu: <(X opération)(Boutton)(on)>
             StateAlim1240 = true;
             Bouton2.state = true;
             break;
          case '0':
             //Message reçu: <(X opération)(Boutton)(off)>
             StateAlim1240 = false;
             Bouton2.state = false;
             break;
          default:
             CmdError("boutton OnOff (1.0)");
       }
       break;
    case 'P' : case 'p' :
       //Message reçu: <(X opération)(Présence)>
       //Message envoyé: <(Présent)>
       //Format "PHH"
       txCmd = txCmd + "P03";
       Send();
       break;
    default:   
      CmdError("Type d action speciale (R.B.A.P)");           
  } 
}

//Fonction qui renvoit la valeur des variables demandée
void LectureVariable() {
  switch (rxCmd[1])
  {
    case 'T' : case 't' :
       //Message reçu: <(Lecture)(Température)>
       //Message envoyé: <(Lecture)(Température)(température..)>
       //Format reçu: "HHHH"
       //Format envoyé : "ERMHHAA\AHHHHVHHHH"
       txCmd = "M01M03ERM03LTA" + rxCmd.substring(2,6) + "V00" + ByteToString(temp);
       Send();
       break ;
    case 'P' : case 'p' :
       //Message reçu: <(Lecture)(Puissance)>
       //Message envoyé: <(Lecture)(Puissance)(puissance..)>
       //Format reçu: "HHHH"
       //Format envoyé : "ERMHHAA\AHHHHVHHHH"
       txCmd = "M01M03ERM03LPA" + rxCmd.substring(2,6) + "V00" + ByteToString(puis);
       Send();
       break ;
    case 'M' : case 'm' :
       //Message reçu: <(Lecture)(Puissance)>
       //Message envoyé: <(Lecture)(Puissance)(puissance..)>
       //Format reçu: "HHHH"
       //Format envoyé : "ERMHHAA\AHHHHVHHHH"
       txCmd = "M01M03ERM03LMA" + rxCmd.substring(2,6) + "V" + WordToString(freeMemory());
       Send();
       break ;
    case 'N' : case 'n' :
       //Message reçu: <(Lecture)(doNnées)>
       //Message envoyé: <(Lecture)(doNnées)(données..)>
       //Format reçu: "HHHH"
       //Format envoyé : "ERMHHAA\AHHHHVHHHH"
       txCmd = "M01M03ERM03N";
       if (Bouton1.state)
          txCmd = txCmd + "A";
       else
          txCmd = txCmd + "F";
       txCmd = txCmd + "A" + rxCmd.substring(2,6) + "V" + WordToString(change);
       Send();
       break ;
    case 'O' : case 'o' :
       //Message reçu: <(Lecture)(On/Off)>
       //Message envoyé: <(Lecture)(On/Off)(StateAlim1240..)>
       //Format reçu: "HHHH"
       //Format envoyé : "ERMHHAA\AHHHHVHHHH"
       txCmd = "M01M03ERM03LOA" + rxCmd.substring(2,6) + "V000";
       if (StateAlim1240) txCmd = txCmd + "1";
       else txCmd = txCmd + "0";
       Send();
       break;
    default:
      CmdError("Type de lecture (M.P.T.D.O)");
  } 
}

//Fonction qui convertit et affiche une valeur sur l'afficheur 15 segments
void convertbcd(byte *valeur){
  int dizaine;
  int unite;
  if (*valeur > 9){
    if (*valeur > 99){
      etat[0] = 1;
      //       Serial.print("C = 1, ");
      dizaine = (*valeur / 10) % 10;
    }
    else {
      etat[0] = 0;
      dizaine = *valeur / 10;
    }
    for (byte a = 0; a < 7; a++){
      etat[a + 1] = etatdu[dizaine][a];
    }
  }
  else {
    for (byte a = 0; a < 7; a++){
      etat[a + 1] = 0;
    }
  }
  unite = *valeur % 10;
  for (byte a = 0; a < 7; a++){
    etat[a + 8] = etatdu[unite][a];
  }
  for(int a = 0; a < 15; a++){
    digitalWrite(pinled[a], 1 - etat[a]);
  }
}

void setup(){

  //Initialise les Serial
  Serial.begin(9600) ;

  //Commande demmarrage du module (attenttion réécrire le numéro du module après type démarage)
  txCmd = "A00M03D03";
  Send();

  for (int a = 0; a < 15; a++){
    pinMode(pinled[a],OUTPUT);
    digitalWrite(pinled[a],HIGH);
  }

  //Initialisation de la librairie Creche
  CR.configure(A4);
  CR.check(true);

  //Etat du bouton1 doit etre haut pour commencer
  Bouton2.state = 1;
  StateAlim1240 = 1;
  Bouton1.state = 1;  
}

void loop(){
  // controle par la librairie Creche
  CR.check(true);
  Bouton1.check(&CR.currentMillis);
  Bouton2.check(&CR.currentMillis);

if (StateAlim1240) {
  //affiche en alternance la température et la puissance
  if (CR.currentMillis - alternance > change && Bouton1.state){
    alternance = CR.currentMillis;
    if (stateaff == 1) convertbcd(&puis);
    if (stateaff++ == 0) convertbcd(&temp);
    if (stateaff == 2) stateaff = 0;
  }
  //affiche soit la température soit la puissance avec rafraichissement
  if (CR.currentMillis - alternance > change && !Bouton1.state){
    alternance = CR.currentMillis;
    if (stateaff == 1) convertbcd(&puis);
    if (stateaff == 0) convertbcd(&temp);
  }
}
else {
    if (CR.currentMillis - alternance > (change / 4)){
       alternance = CR.currentMillis;
       digitalWrite(pinled[indiceLed], HIGH);
       digitalWrite(pinled[indiceLed + 7], HIGH);
       (indiceLed >= 7)?indiceLed = 1:indiceLed++;
       digitalWrite(pinled[indiceLed], LOW);
       digitalWrite(pinled[indiceLed + 7], LOW);
    }
}

  //Gestion de la réception de commande via le serial
  byte car ;
  while (Serial.available())
      TreatChar(Serial.read(), Seria);

  //Gestion de l'etat on/off du système
  if (Bouton2.FrontState) {
     if (Bouton2.state){ //recopie de l'etat de state sur StateAlim1240
       StateAlim1240 = 1;
       //On envois un signal général appuis sur le boutton on/off
       txCmd = "A00M03XB1";
       Send();
     }
     else {
       StateAlim1240 = 0;
       //On envois un signal général appuis sur le boutton on/off
       txCmd = "A00M03XB0";
       Send();
     }
  }
}

