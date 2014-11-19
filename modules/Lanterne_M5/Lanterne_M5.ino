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
 ********PROGRAMME SERVEUR POUR L'ECLAIRAGE DE LA CRECHE DE SAINT JOSEPH,********
 ********************************************************************************
 *                                SUR ARDUINO UNO,                              *
 *       A UTILISER UNIQUEMENT AVEC LA PROTOSHIELD ADAPTE A CE PROGRAMME        *
 *                                  MODULE 5                                    *
 *                      Cette carte est composée:                               *
 *          -Une sortie PWM avec bornier pour un jeux de LED 12V                *
 *          -Module NRf24l01+                                                   *
 *          -WatchDog Harware avec un 555, avec une tempos de 9s                *
 *                                                                              *
 ********************************************************************************
 *                                                                              *
 *          FONCTION:                                                           *
 *Module qui assure la commande d'un jeux de LED                                *
 *                                                                              *
 ********************************************************************************
 *             copyright       REALISE PAR E.CROONEN,                           *
 ********************************************************************************
 
 
 
//*******************************************************************************
//*******************************************************************************
//                                ATTENTION                                     *
//les messages radios et PC changent                                            *
//*******************************************************************************
//*******************************************************************************
*/

//---------------------------INCLUDE--------------------------------

//Activation et configuration de la librairie Mirf pour la communication radio
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <Wire.h>
#include <MirfHardwareSpiDriver.h>

#include <Creche.h>

//PROGMEM const String devise = "Mon Dieu faite que je serve uniquement a votre plus grande gloire";

//---------------------------VARIABLES--------------------------------

//Tableau avec l'état de chacune des LED 
unsigned long ledmillis[1] = {0};//heure de la dernière action sur cette LED
byte ledata[1][7] = {{'V',1,0,0,10,0,3}};
/*
 -type de fonctionnement (0)
 -demande progressif, ou position séquence vacillement (1)
 -luminosité actuelle, (2)
 -variation (3)
 -intervalle de temps entre deux actions de variation 1ms -> FFms (4)
 -multiplicatif d'intervalle de temps entre deux actions de variation 100ms -> FF00ms (5)
 -pin sur lequel est connecté le circuit LED en question (6)
 */


//Variable qui indique si le système est allumé ou éteint
boolean StateAlim1240;
boolean LastStateAlim1240;

String rxCmd ; // Commande reçue (sans délimiteurs)
String txCmd ; // Commande à transmettre//Variables pour les échanges de données structurées par sériale

/* Hardware SPI on arduino mega 2560 (arduino uno):
 * MISO -> 50 (12)
 * MOSI -> 51 (11)
 * SCK -> 52 (13)
 * 
 * Configurable:
 * CE -> 48 (8)
 * CSN -> 53 (7)
 *
 * Pins on nrf24L01: 
 * 1 GND
 * 2 VCC 3V3
 * 3 CE
 * 4 CSN
 * 5 SCK
 * 6 MOSI
 * 7 MISO
 * 8 IRQ (retourne état du module) not connected
 *
 * Note: To see best case latency comment out all Serial.println
 * statements not displaying the result and load 
 * 'ping_server_interupt' on the server.
 */

String fCmd[2]; //Message reçu par les cannaux de communication

//---------------------------------FONCTIONS------------------------------------

// traitement du car recu
void TreatChar(char car, Mode canal)
{
  switch (car)
  {
  case '<' :
    fCmd[canal] = "";
    break ;
  case '>' :
    AdressageMessage(canal);
    break ;
  default:
    fCmd[canal] = fCmd[canal] + car ;
  }
}

//Traitement d'un message valid reçu
void AdressageMessage(Mode canal)
{
    txCmd = "";
    //On fait une analyse du destinataire et de l'envoyeur pour faire suivre le message vers la bonne adresse
    byte desti, envoyeur;
    //On relève les addresse des destinataires et envoyeur
    //Dans le cas All la valeur destinataire doit etre inscrite mais est inutile
    StringToByte(desti,fCmd[canal], 1);
    StringToByte(envoyeur,fCmd[canal], 4);
    switch (fCmd[canal][0])
    {
      case 'M' : case 'm' :
        if (desti == Lanterne_m5) {
           txCmd = fCmd[canal][3] + ByteToString(envoyeur) + "M05";
           //Les 6 premier carractère du message concerne l'adressage, le module prend cette information séparement pour un éventuel retour de message
           TreatCommand(fCmd[canal].substring(6)) ;
        }
        else {
            //Si le message n'est pas a destination de ce module, c'est qu'il est attendu sur un autre module, donc on le fait suivre
            txCmd = fCmd[canal];
            Send(canal);
        }
        break ;
      case 'A' : case 'a' :
           txCmd = fCmd[canal][3] + ByteToString(envoyeur) + "M05";
           //Les 6 premier carractère du message concer l'adressage, le module prend cette information séparement pour un éventuel retour de message
           TreatCommand(fCmd[canal].substring(6));
           //On fait suivre le message vers les autres modules qui ne l'auraient pas encore reçu
           txCmd = fCmd[canal];
           Send(canal);
        break ;
      case 'E' : case 'e' :
        //Module répond "ErrMessage" (normalement programmer le renvois du même message)
        txCmd = "Message non recu, a faire";
        Send(Module);
        break;
      default:
        //Le message ne correspond a rien
        txCmd = "ErrMessageM5";
        Send(Module);
        break;
    }
}

//Boucle d'envois d'un message via la radio
void MirfSend(String cmd) {
  int i = 0, j;
  byte time = 'A';
  for (int i = 0; i < cmd.length(); i++) {
    time = cmd[i];
    Mirf.send(&time);
    j = 0;
    while(Mirf.isSending() && j < 10) {
       j++;
       delay(1);
    }
    delay(10);
  }
}

// Transmission de la commande. le message est dirigé vers le bon récepteur
void Send(Mode canal) {
    //On fait une analyse du destinataire et de l'envoyeur pour faire suivre le message vers la bonne adresse
    byte desti, envoyeur;
    //On relève les addresse des destinataires et envoyeur
    //Dans le cas All la valeur destinataire doit etre inscrite mais est utilité
    StringToByte(desti,txCmd, 1);
    StringToByte(envoyeur,txCmd, 4);
    String cmd = '<' + txCmd + ">@";
    if (canal != Radio) {
       //Message a destination de M1 et M4
       if (desti == Serveur_m1 || desti == BoutonServeur_m4 || ((cmd[1] == 'A' || cmd[1] == 'a') && envoyeur != Serveur_m1 && envoyeur != BoutonServeur_m4)) {
           Mirf.setTADDR((byte *)"nrf01");
           MirfSend(cmd);
       }
       if (desti == Pc_m0 && (cmd[1] == 'M' || cmd[1] == 'm') && envoyeur != Serveur_m1 && envoyeur != BoutonServeur_m4) {
           Mirf.setTADDR((byte *)"nrf01");
           MirfSend(cmd);
       }
       //Message a destination de M2 et M3
       if (desti == Pwm_m2 || desti == Dbc_m3 || ((cmd[1] == 'A' || cmd[1] == 'a') && envoyeur != Pwm_m2 && envoyeur != Dbc_m3)) {
           Mirf.setTADDR((byte *)"nrf02");
           MirfSend(cmd);
       }
       if (desti == Pc_m0 && (cmd[1] == 'M' || cmd[1] == 'm') && envoyeur != Pwm_m2 && envoyeur != Dbc_m3) {
           Mirf.setTADDR((byte *)"nrf02");
           MirfSend(cmd);
       }
    }
    if (canal != Seria) {
       //Message a destination du PC
       if (desti == Pc_m0 || cmd[1] == 'A' || cmd[1] == 'a') {
           Serial.println(cmd);
       }
    }
}

void CmdError(String type)
// Doit être appelé quand il y a une erreur dans rxCmd
{
  txCmd = txCmd + "ERR " + type;
  Send(Module);
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
    case 'K' : case 'k' : 
      //Message reçu : <(K Led)...>
      EcritureLed() ;
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
      CmdError("type d'operation (X.L.E.D.P.K)");
      break ;      
  }
}

//Fonction qui permet l'ecriture de certaine variable
void EcritureVariable () {
  Date date;
       switch (rxCmd[1]) {
          default :
             CmdError("Ecriture (Nihil)");
       }
}

//Fonction qui renvoit la valeur des variables demandée
void LectureVariable() {
  byte led;
  switch (rxCmd[1])
  {
    case 'M' : case 'm' :
       //Message reçu: <(Lecture)(Memoire)>
       //Message envoyé: <(Lecture)(Memoire)(memoire..)>
       //Format "HHHH"
       txCmd = txCmd + "LM" + WordToString(freeMemory());
       Send(Module);
       break ;
    case 'O' : case 'o' :
       //Message reçu: <(Lecture)(On/Off)>
       //Message envoyé: <(Lecture)(On/Off)(StateAlim1240..)>
       //Format "B"
       txCmd = txCmd + "LO";
       if (StateAlim1240) txCmd = txCmd + "1";
       else txCmd = txCmd + "0";
       Send(Module);
       break;
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
          Send(Module);
       }
       else {
          CmdError("LL adresse led 01");
       }
       break;
    default:
      CmdError("L Type de lecture (M.O.L)");
  } 
}

//Fonction qui réagit a des commandes spéciales
void DonneSpeciaux() {
  switch (rxCmd[1])
  {
    case 'R' : case 'r' :
       //Message reçu: <(X opération)(Reboot)>
       while ( rxCmd[1] < 'z');
    case 'B' : case 'b' :
       //Message reçu: <(X opération)(Boutton)>
       //Format "B"
       switch (rxCmd[2]) {
          case '1':
             //Message reçu: <(X opération)(Boutton)(on)>
             StateAlim1240 = true;
             break;
          case '0':
             //Message reçu: <(X opération)(Boutton)(off)>
             StateAlim1240 = false;
             break;
          default:
             CmdError("XB boutton OnOff (0.1)");
       }
       break;
    case 'P' : case 'p' :
       //Message reçu: <(X opération)(Présence)>
       //Message envoyé: <(Présent)>
       //Format "P99"
       txCmd = txCmd + "P05";
       Send(Module);
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
  //Initialise les Serial
  Serial.begin(9600) ;

  //Initialisation de la librairie Creche
  CR.configure(A0);
  CR.check(true);

  //Initialise les canaux de réception
  fCmd[Seria] = "";
  fCmd[Radio] = "";
  
  //Initialisation de la librairie Mirf
  Mirf.cePin = 8; // CE sur D8
  Mirf.csnPin = 7; // CSN sur D7
  Mirf.spi = &MirfHardwareSpi; // On veut utiliser le port SPI hardware
  Mirf.init(); // Tout est bon ? Ok let's go !
  Mirf.channel = 25; // On va utiliser le canal 0 pour communiquer (128 canaux disponible, de 0 à 127)
  Mirf.payload = sizeof(byte); // = 4, ici il faut déclarer la taille du "payload" soit du message qu'on va transmettre, au max 32 octets
  Mirf.config(); // Tout est bon ? Ok let's go !
  Mirf.setRADDR((byte *)"nrf05"); // On définit ici l'adresse du module en question

  //Commande demmarrage du module (attenttion réécrire le numéro du module après type démarage)
  txCmd = "A00M05D05";
  Send(Module);

  //Au démarrage on considère que le système est allumé
  LastStateAlim1240 = true;
  StateAlim1240 = true;
  
}

void loop(){
  // controle par la librairie Creche
  CR.check(true);

  //Gestion de la réception de commande via le serial
  byte car ;
  while (Serial.available())
      TreatChar(Serial.read(), Seria);
  if(!Mirf.isSending() && Mirf.dataReady()){
    Mirf.getData(&car);
    TreatChar(car, Radio);    
  }
  
  //action sur les LED
  if (StateAlim1240) { //La condition sur le relais de l'alimentation générale est désactivée car déconnection électrique
     byte led = 0;
       //s'il est temps de changer l'état de la LED on entre dans la condition
       if (CR.currentMillis - ledmillis[led] > word(ledata[led][5], ledata[led][4])) {
 /*        Serial.print(", ledmillis[led] ");
         Serial.print(ledmillis[led]);
         Serial.print(", currentMillis ");
         Serial.print(CR.currentMillis - ledmillis[led]);
         Serial.print(", Interval ");
         Serial.println(word(ledata[led][5], ledata[led][4]));*/
         
         //enregistrement de l'heure de l'action, le retard d'action est comptabilisé
         //ledmillis[led] = 2 * CR.currentMillis - ledmillis[led] - word(ledata[led][5], ledata[led][4]);
         ledmillis[led] = CR.currentMillis;
         luminosite(&ledata[led][0], &ledata[led][1], &ledata[led][2], &ledata[led][3], &ledata[led][4], &ledata[led][5], &ledata[led][6]);
       }
     //On vient de réallumé le système
     if (!LastStateAlim1240) LastStateAlim1240 = true;
  }
  else {
     //On vient d'éteindre le système
     byte led = 0;
     if (LastStateAlim1240) {
        LastStateAlim1240 = false;
        //mise à zero de toute les LED de puissance
        byte TOR = 'T';
        luminosite(&TOR, 0, 0, &ledata[led][3], &ledata[led][4], &ledata[led][5], &ledata[led][6]);
     }
  }
  
}


