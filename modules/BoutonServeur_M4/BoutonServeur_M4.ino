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
 ****PROGRAMME BOUTON SERVEUR POUR L'ECLAIRAGE DE LA CRECHE DE SAINT JOSEPH,*****
 ********************************************************************************
 *                                SUR ARDUINO UNO,                              *
 *       A UTILISER UNIQUEMENT AVEC LA PROTOSHIELD ADAPTE A CE PROGRAMME        *
 *                                                                              *
 *                      Cette carte est composée:                               *
 *          -3 LED jaune                                                        *
 *          -3 Boutons poussoir                                                 *
 *          -1 LED rouge témoin des communication radio                         *
 *          -1 Bornier 4 plots (en parallèle avec les boutons poussoir)         *
 *          -WatchDog Harware avec un 555, avec une tempos de 9s                *
 *          -Module mémoire eeprom qui utilise le bus I²C                       *
 *                                                                              *
 ********************************************************************************
 *                                                                              *
 *          FONCTION:                                                           *
 *Module qui permet le pilotage de la séquence d'éclairage pour décharger le    *
 *le module serveur et aussi pour lui assurer une alimentation 3.3V correcte.   *
 *
 *                                                                              *
 ********************************************************************************
 *             copyright       REALISE PAR E.CROONEN,                           *
 ********************************************************************************
 
 
 
//*******************************************************************************
//*******************************************************************************
//                                ATTENTION                                     *
//les messages radios et PC changent (identifiant; valeur; fin de com)          *
//*******************************************************************************
//*******************************************************************************
 */

//---------------------------INCLUDE--------------------------------

#include <Creche.h>

//---------------------------VARIABLES--------------------------------

//Création de 3 boutons avec led Témoins
BOUTON Bouton1(A0, 6), Bouton2(A2, 4), Bouton3(A3,2);

//Tableau avec l'état de chacune des LED 
unsigned long ledmillis[1] = {0};//heure de la dernière action sur cette LED
byte ledata[1][7] = {{'T',0,0,0,10,0,5}};
/*
 -type de fonctionnement (0)
 -demande progressif, ou position séquence vacillement (1)
 -luminosité actuelle, (2)
 -variation (3)
 -intervalle de temps entre deux actions de variation 1ms -> FFms (4)
 -multiplicatif d'intervalle de temps entre deux actions de variation 100ms -> FF00ms (5)
 -pin sur lequel est connecté le circuit LED en question (6)
 */

String fCmd; //Message reçu par les cannaux de communication

//Variable qui indique si le système est allumé ou éteint
boolean StateAlim1240;
boolean LastStateAlim1240;

String rxCmd ; // Commande reçue (sans délimiteurs)
String txCmd ; // Commande à transmettre//Variables pour les échanges de données structurées par sériale

//---------------------------------FONCTIONS------------------------------------

// traitement du car recu
void TreatChar(char car)
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
        if (desti == 4) {
           txCmd = fCmd[3] + ByteToString(envoyeur) + "M04";
           //Les 6 premier carractère du message concerne l'adressage, le module prend cette information séparement pour un éventuel retour de message
           TreatCommand(fCmd.substring(6)) ;
        }
        break ;
      case 'A' : case 'a' :
           txCmd = fCmd[3] + ByteToString(envoyeur) + "M04";
           //Les 6 premier carractère du message concer l'adressage, le module prend cette information séparement pour un éventuel retour de message
           TreatCommand(fCmd.substring(6));
           //On fait suivre le message vers les autres modules qui ne l'auraient pas encore reçu
           txCmd = fCmd;
        break ;
      default:
        //Le message ne correspond a rien
        txCmd = "M00M04ErrMessageM4";
        Send();
        break;
    }
}

// Transmission de la commande. le message est dirigé vers le bon récepteur
void Send() {
    String cmd = '<' + txCmd + '>';
    Serial.println(cmd);
}

void CmdError(String type)
// Doit être appelé quand il y a une erreur dans rxCmd
{
  txCmd = txCmd + "Z ERR " + type;
  Send();
}

//Traitement des commandes propre pour ce module
void TreatCommand(String cmd) {
  rxCmd = cmd;
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
    case 'K' : case 'k' : 
      //Message reçu : <(K Led)...>
      EcritureLed() ;
      break ;
    case 'D' : case 'd' : 
      //Message reçu : <(Demarrage)...>
      //Un module x vient de redémrrer peut-etre quil faut lui envoyer une commande de mise a jour
      byte desti;
      StringToByte(desti,rxCmd,1);
      break ;
    case 'Z' : case 'z' : 
      //Message reçu : <(Message sans effet)...>
      //Rien a faire ici
      //Permet de faire passer une indication a l'utilisateur
      break ;
    default:   
      CmdError("type d'operation (X.L.E.D.K)");
      break ;      
  }
}

//Fonction qui permet l'ecriture de certaine variable
void EcritureVariable () {
  Date date;
  switch (rxCmd[1])
  {
    case 'V' : case 'v' :
      if (rxCmd[2] == '1') Bouton2.state = 1;
      if (rxCmd[2] == '0') Bouton2.state = 0;
      break;
    case 'S' : case 's' :
      if (rxCmd[2] == '1') Bouton3.state = 1;
      if (rxCmd[2] == '0') Bouton3.state = 0;
      break;
    default:   
      CmdError("E Type d'ecriture (V.S)");
  }
}

//Fonction qui renvoit la valeur des variables demandée
void LectureVariable() {
  /*
  Le message de lecture variable ne renvois plus de message au demandeur.
  -Il envois un message pour la RAM de M1 avec la valeur a y inscrire
     *pas de risque de message en boucle
  -M1 renvois automatiquement un message vers M0 avec le contenu recopié
     *seul l'utilisateur et l'executant sont suceptible de connaitre la valeur
     *Permet d'introduire des conditions sur les varibles de tous le système
  */
  byte led;
  switch (rxCmd[1])
  {
    case 'M' : case 'm' :
       //Message reçu: <(Lecture)(Memoire)>
       //Message envoyé: <(Lecture)(Memoire)(memoire..)>
       //Format reçu: "HHHH"
       //Format envoyé : "ERMHHAA\AHHHHVHHHH"
       txCmd = "M01M04ERM04LMA" + rxCmd.substring(2,6) + "V" + WordToString(freeMemory());
       Send();
       break ;
    case 'V' : case 'v' :
       //Message reçu: <(Lecture)(Validation)>
       //Message envoyé: <(Lecture)(Validation)(validation..)>
       //Format reçu: "HHHH"
       //Format envoyé : "ERMHHAA\AHHHHVHHHH"
       txCmd = "M01M04ERM04LVA" + rxCmd.substring(2,6) + "V000";
       if (Bouton2.state) txCmd = txCmd + "1";
       else txCmd = txCmd + "0";
       Send();
       break ;
    case 'S' : case 's' :
       //Message reçu: <(Lecture)(Synchronisation)>
       //Message envoyé: <(Lecture)(Synchronisation)(synchronisation..)>
       //Format reçu: "HHHH"
       //Format envoyé : "ERMHHAA\AHHHHVHHHH"
       txCmd = "M01M04ERM04LSA" + rxCmd.substring(2,6) + "V000";
       if (Bouton3.state) txCmd = txCmd + "1";
       else txCmd = txCmd + "0";
       Send();
       break ;
    case 'L' : case 'l' :
       //Message reçu: <(Lecture)(Led)(led)>
       //Message envoyé: <(Lecture)(Led)(led)>
       //Format "HH"
       //Format "HHTA\DHHLHHVHHTHHHHPHH"
       StringToByte(led,rxCmd,2);
       if (led == 1) {
          txCmd = txCmd + "LL" + ByteToString(1) + "T0";
          txCmd.setCharAt(txCmd.length()-1, ledata[led-1][0]);
          txCmd = txCmd + "D" + ByteToString(ledata[led-1][1]) + "L" + ByteToString(ledata[led-1][2]) + "V" + ByteToString(ledata[led-1][3]) + "T" + ByteToString(ledata[led-1][5]) + ByteToString(ledata[led-1][4]);
          Send();
       }
       else {
          CmdError("LL adresse led 01");
       }
       break;
    case 'O' : case 'o' :
       //Message reçu: <(Lecture)(On/Off)>
       //Message envoyé: <(Lecture)(On/Off)(StateAlim1240..)>
       //Format reçu: "HHHH"
       //Format envoyé : "ERMHHAA\AHHHHVHHHH"
       txCmd = "M01M04ERM04LOA" + rxCmd.substring(2,6) + "V000";
       if (StateAlim1240) txCmd = txCmd + "1";
       else txCmd = txCmd + "0";
       Send();
       break;
    default:   
      CmdError("L Type de lecture (M.V.S.L)");
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
       while ( rxCmd[1] < 'T');
    case 'B' : case 'b' :
       //Message reçu: <(X opération)(Boutton)>
       //Format "B"
       switch (rxCmd[2]) {
          case '1':
             //Message reçu: <(X opération)(Boutton)(on)>
             StateAlim1240 = true;
             Bouton1.state = true;
             break;
          case '0':
             //Message reçu: <(X opération)(Boutton)(off)>
             StateAlim1240 = false;
             Bouton1.state = false;
             break;
          default:
             CmdError("XB boutton OnOff (0.1)");
       }
       break;
    case 'P' : case 'p' :
       //Message reçu: <(X opération)(Présence)>
       //Message envoyé: <(Présent)>
       //Format "PHH"
       txCmd = txCmd + "P04";
       Send();
       break;
    default:   
      CmdError("X action speciale (R.B.P)");           
  } 
}

//Fonction qui fait changer la luminosité des LED
void luminosite(byte *type, byte *demande, byte *lumino, byte *variation, byte *inter, byte *multinter, byte *pin) {
   switch (*type) {
      case 'T' : //Tout ou rien
         //rien a faire ici
         break;
      case 'P' : //Progressif
         //Cas si on demande d'augmenter la luminosité
         if (*demande >= *lumino + *variation) {
            *lumino = *lumino + *variation; //Augmentation de la luminosité
         }
         else if (*demande > *lumino) // La luminosité est proche de la valeur demandée
            *lumino = *demande;
         //Cas si on demande de la diminuée
         if (*demande <= *lumino - *variation) {
            *lumino = *lumino - *variation; //Diminution de la luminosité
         }
         else if (*demande < *lumino) //
            *lumino = *demande;
         //Cas ou nous avons atteint la luminosité souhaité
         if (*demande == *lumino) {
            *inter = 0xFF; //on augmente l'intervalle pour éviter de surcharger le PIC
            *multinter = 0x08;
         }
         break;
      case 'C' : //Clignotement franc
         *lumino = 255 - *lumino;
         break;
      case 'V' : //Vacilement flame
         switch(*demande) {
           case 1 :
             *lumino += 1;
             *inter = 10;
             if (*lumino >= 160) *demande += 1;
             break;
           case 2 :
             *lumino -= 1;
             *inter = 3;
             if (*lumino < 80) *demande += 1;
             break;
           case 3 :
             *lumino += 1;
             *inter = 2;
             if (*lumino >= 254) *demande += 1;
             break;
           case 4 :
             *lumino -= 1;
             *inter = 3;
             if (*lumino < 150) *demande += 1;
             break;
           case 5 :
             *lumino += 3;
             *inter = 1;
             if (*lumino > 254) *demande += 1;
             break;
           case 6 :
             *lumino -= 1;
             *inter = 2;
             if (*lumino < 100) *demande += 1;
             break;
           case 7 :
             *lumino += 1;
             *inter = 1;
             if (*lumino < 220) *demande += 1;
             break;
           case 8 :
             *lumino -= 1;
             *inter = 2;
             if (*lumino < 2) *demande += 1;
             break;
           case 9 :
             *lumino += 1;
             *inter = 4;
             if (*lumino > 120) *demande += 1;
             break;
           case 10 :
             *lumino += 1;
             *inter = 2;
             if (*lumino > 254) *demande += 1;
             break;
           case 11 :
             *lumino -= 1;
             *inter = 20;
             if (*lumino < 200) *demande += 1;
             break;
           case 12 :
             *lumino -= 1;
             *inter = 2;
             if (*lumino < 40) *demande += 1;
             break;
           case 13 :
             *lumino += 1;
             *inter = 3;
             if (*lumino > 100) *demande += 1;
             break;
           case 14 :
             *lumino += 1;
             *inter = 2;
             if (*lumino > 200) *demande += 1;
             break;
           case 15 :
             *lumino -= 1;
             *inter = 20;
             if (*lumino < 150) *demande += 1;
             break;
           case 16 :
             *lumino -= 1;
             *inter = 2;
             if (*lumino < 2) *demande += 1;
             break;
           default :
             *demande = 1;
             *inter = 1;
         }
         break;
      case 'R' : //Run (Clignote avec variation progressive)
         if (*demande == 1) *lumino += *variation;
         else *lumino -= *variation;
         if (*lumino > 255 - *variation) *demande = 0;
         if (*lumino < 1 + *variation) *demande = 1;
         break;
   }
  //sur la sortie correspondante la variation de luminosité
  analogWrite(*pin, *lumino);
}

//Fonction qui converti les données état initiale, état finale et delay vers les valeurs pour permettre au système d'avoir les valeurs instantanée
//temps par défaut = 1s (0x03E8 ms)
void SelectionEclairage(byte type, byte led, unsigned int temps = 0x03E8, byte demandefin = 0, boolean ini = false, byte demandeini = 0) {
   //On entre le type de fonctionnement
   ledata[led][0] = type;
   int nbvariation;
   byte i;
   switch (type) {
      case 'T' : //Tout ou rien:  on/Off et Delay
         if (demandefin == 255) {
            demandeini = 255;
            ledata[led][2] = 255;
         }
         else {
            ledata[led][2] = 0;
            demandeini = 0;
            demandefin = 0;
         }
         //interval maximum
         ledata[led][4] = temps & 0x00FF; //Interval poid faible
         ledata[led][5] = (temps >> 8) & 0x00FF; //interval poid fort
         ledata[led][3] = 0; //pas de variation
         luminosite(&ledata[led][0], &demandeini, &ledata[led][3], &ledata[led][3],&ledata[led][4], &ledata[led][5], &ledata[led][6]);
         break;
      case 'P' : //Progressif valeur: temps de variation, valeur final, bool ini, initiale, 
         //Valeur de luminosité finale
         ledata[led][1] = demandefin;
         //On varie la luminosité par unité
         ledata[led][3] = 1;
         //Si il n'y a pas de valeur ini précisée, on récupère la dernière valeur de luminosité
         if (!ini) {
            demandeini = ledata[led][2];
         }
         //Si non on applique instantanément la nouvelle valeur de luminositée
         else {
            ledata[led][2] = demandeini;
            luminosite(&ledata[led][0], &demandeini, &demandeini, &ledata[led][3],&ledata[led][4], &ledata[led][5], &ledata[led][6]);
         }
         //Valeur de delay entre deux variations (Chaque variation est unitaire de 1 jusque 255), on change alors le delay 
         nbvariation = demandefin - demandeini;
         if (nbvariation < 0) nbvariation = -1 * nbvariation;
         ledata[led][4] = temps / (nbvariation + 1);
         ledata[led][5] = 0;
         //si l'intervall est trop court alors on impose une variation plus grande pour un delay fixe
         i = 0;
         while (ledata[led][4] < 20)
         {
            ledata[led][4] = temps / ((nbvariation / 2) + 1);
            ledata[led][3]++;
            i++;
            if (i > 10) break;
         }
         break;
      case 'C' : //Clignotement franc: période de clignotement, 
         ledata[led][1] = 0;
         ledata[led][2] = 255;
         //interval maximum
         ledata[led][4] = temps & 0x00FF; //Interval poid faible
         ledata[led][5] = temps >> 8; //interval poid fort
         ledata[led][3] = 0; //pas de variation
         luminosite(&ledata[led][0], &ledata[led][1], &ledata[led][2], &ledata[led][3],&ledata[led][4], &ledata[led][5], &ledata[led][6]);
         break;
      case 'V' : //Vacilement flame: aucun paramètre
         ledata[led][1] = 1;
         ledata[led][2] = 50; //Commence directetement par une luminosité non null
         ledata[led][3] = 0; //variation pas utilisee
         ledata[led][4] = 0; //Interval poid faible
         ledata[led][5] = 0; //interval poid fort
         luminosite(&ledata[led][0], &ledata[led][1], &ledata[led][2], &ledata[led][3],&ledata[led][4], &ledata[led][5], &ledata[led][6]);
         break;
      case 'R' : //Run (Clignote avec variation progressive): période de clignotement
         ledata[led][1] = 0;
         demandefin = 1;
         ledata[led][2] = demandefin;
         //Valeur de delay entre deux variations (Chaque variation est unitaire de 1 jusque 255), on change alors le delay 
         ledata[led][4] = temps / 256;
         ledata[led][5] = 0; //interval poid fort
         ledata[led][3] = 1; //Variation
         //si l'intervall est trop court alors on impose une variation plus grande pour un delay fixe
         i = 1;
         while (ledata[led][4] < 20)
         {
            ledata[led][4] = temps / 256 * ledata[led][3];
            ledata[led][3]++;
            i++;
            if (i > 10) break;
         }
         luminosite(&ledata[led][0], &demandeini, &demandeini, &ledata[led][3],&ledata[led][4], &ledata[led][5], &ledata[led][6]);
         break;
   }
   //on attend le temps d'intervalle avant la première variation
   ledmillis[led] = CR.currentMillis;
}

//Fonction qui permet l'ecriture des variables des LEDs
void EcritureLed () {
   byte led = 0, demandefin = 0, demandeini = 0;
   unsigned int temps = 0;
   StringToByte(led,rxCmd,2);
   if (led != 1)
      CmdError(" adresse led 01");
   else {
      switch (rxCmd[1]) {
         case 'T' : case 't' ://Tout ou rien:  on/Off et Delay
            //Message reçu: <(Led)(TOR)(led)(on/off)>
            //Format : "HHOB"
            if (rxCmd[5] == '1')
               SelectionEclairage('T', led - 1, 0x03E8, 255);
            else 
               SelectionEclairage('T', led - 1, 0x03E8, 0);
            break;
         case 'P' : case 'p' ://Progressif valeur: temps de variation, valeur final,
            //Message reçu: <(Led)(Progressif)(led)(temps)(valeur final)>
            //Format : "HHDHHHHLHH"
            StringToWord(temps, rxCmd, 5);
            StringToByte(demandefin, rxCmd, 10);
            SelectionEclairage('P', led - 1, temps, demandefin);
            break;
         case 'D' : case 'd' ://Progressif Debut valeur: temps de variation, valeur final, bool ini, initiale,
            //Message reçu: <(Led)(progressif debut)(led)(temps)(demandefin)(demandeini)>
            //Format : "HHDHHHHLHHIHH"
            StringToWord(temps, rxCmd, 5);
            StringToByte(demandefin, rxCmd, 10);
            StringToByte(demandeini, rxCmd, 13);
            SelectionEclairage('P', led - 1, temps, demandefin, true, demandeini);
            break;
         case 'C' : case 'c' ://Clignotement franc: période de clignotement, 
            //Message reçu: <(Led)(Clignotement)(led)(temps)>
            //Format : "HHTHHHH"
            StringToWord(temps, rxCmd, 5);
            SelectionEclairage('C', led - 1, temps);
            break;
         case 'V' : case 'v' ://Vacilement flame: aucun paramètre
            //Message reçu: <(Led)(Vacilement)(led)>
            //Format : "HH"
            SelectionEclairage('V', led - 1);
            break;
         case 'R' : case 'r' ://Run (Clignote avec variation progressive): période de clignotement
            //Message reçu: <(Led)(Run)(led)(temps)>
            //Format : "HHTHHHH"
            StringToWord(temps, rxCmd, 5);
            SelectionEclairage('R', led - 1, temps);
            break;
         default :
            CmdError(" LED type (T.P.D.C.V.R)");
      }
   }
}

void setup(){

  //Initialisation de la librairie Creche
  CR.configure(9);
  CR.check(true);

  //Initialise les Serial
  Serial.begin(9600);
  
  fCmd = "";

  //Commande demmarrage du module (attenttion réécrire le numéro du module après type démarage)
  txCmd = "A00M04D04";
  Send();

  //Initialisation de la led rouge
  pinMode(5, OUTPUT);  //The 9 line goes Hi-Z pour le WatchDog
  
  //Au démarrage on considère que le système est allumé
  LastStateAlim1240 = true;
  StateAlim1240 = true;
  Bouton1.state = true;
}

void loop(){
  // controle par la librairie Creche
  CR.check(true);
  
  //Les variables test permettent d'éviter que l'utilisateur active la Validation et la synchronisation en dehors du système
  boolean testV, testS;
  testV = Bouton2.state;
  testS = Bouton3.state;
  //Lecture de chaque bouton
  Bouton1.check(&CR.currentMillis);
  Bouton2.check(&CR.currentMillis);
  Bouton3.check(&CR.currentMillis);
  //Gestion de la validation
  //Un message est envoyé lorsque la variable bouton state passe de 1 vers 0
  //Si l'utilisateur active le state par appuie sur le bouton, ceci n'est pas prit en compte et est annulé
  if (Bouton2.FrontState) {
     if (!Bouton2.state && testV){ 
       //On envois un signal appuis sur validation
       txCmd = "M01M04RV0";
       Send();
     }
  }
  //Gestion de la synchronisation
  if (Bouton3.FrontState) {
     if (!Bouton3.state && testS){ 
       //On envois un signal appuis sur synchronisation
       txCmd = "M01M04RS0";
       Send();
     }
  }
  if (!testV && Bouton2.state) Bouton2.state = false;
  if (!testS && Bouton3.state) Bouton3.state = false;

  //Gestion de l'etat on/off du système
  if (Bouton1.FrontState) {
     if (Bouton1.state){ //recopie de l'etat de state sur StateAlim1240
       StateAlim1240 = 1;
       //On envois un signal général appuis sur le boutton on/off
       txCmd = "A00M04XB1";
       Send();
     }
     else {
       StateAlim1240 = 0;
       //On envois un signal général appuis sur le boutton on/off
       txCmd = "A00M04XB0";
       Send();
     }
  }

  //Gestion de la réception de commande via le serial
  byte car ;
  while (Serial.available())
      TreatChar(Serial.read());
  
  //action sur les LED
  byte led = 0;
  //s'il est temps de changer l'état de la LED on entre dans la condition
  if (CR.currentMillis - ledmillis[led] > word(ledata[led][5], ledata[led][4])) {
     //enregistrement de l'heure de l'action, le retard d'action est comptabilisé
     //ledmillis[led] = 2 * CR.currentMillis - ledmillis[led] - word(ledata[led][5], ledata[led][4]);
     ledmillis[led] = CR.currentMillis;
     luminosite(&ledata[led][0], &ledata[led][1], &ledata[led][2], &ledata[led][3], &ledata[led][4], &ledata[led][5], &ledata[led][6]);
  }

}



