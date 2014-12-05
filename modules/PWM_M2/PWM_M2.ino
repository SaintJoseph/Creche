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
 ********PROGRAMME PWM LED POUR L'ECLAIRAGE DE LA CRECHE DE SAINT JOSEPH,********
 ********************************************************************************
 *                            SUR ARDUINO MEGA 2560,                            *
 *       A UTILISER UNIQUEMENT AVEC LA PROTOSHIELD ADAPTE A CE PROGRAMME        *
 *                                                                              *
 *                      Cette carte est composée:                               *
 *        -1 LED rouge & verte (témoin)                                         *
 *        -1 Boutons poussoir (on/off) éclairage                                *
 *        -1 Boutons poussoir reset                                             *
 *        -1 Bornier 3 plots pour la sonde de température DS18B20               *
 *        -1 Bornier 6 plots, sorties PWM vers les drivers des Led de puissance *
 *        -Module NRf24l01+                                                     *
 *        -WatchDog Harware avec un 555, avec une tempos de 8s                  *
 *                                                                              *
 ********************************************************************************
 *                                                                              *
 *          FONCTION:                                                           *
 *Commande de l'éclairage de puissance LED en PWM.Il y a 6 circuits limineux.   *
 *Chaque circuit est composé de 10 LED de 5W.                                   *
 *Les drivers de 10W reçoivent la commande de Arduino Mega 2560 sous forme      *
 *de signal, ceux-ci le convertissent en puissance sachant que chaque driver    *
 *permmet d'allumer 2 LED. Chaque partie étant piloté par des sortie différente *
 *de Arduino.                                                                   *
 *Controle de la température du système avec la sonde. S'il y a dépassement     *
 *de la température maximale authorisée, il coupe le système.                   *
 *-------------------------- 6 circuits lumineux -------------------------------*
 *   - 3 Blanc chaud x 3 = 30 LED                                               *
 *      -> 1 LED blanc chaud = 200 lm                                           *
 *      -> 30 LED = 6000 lm                                                     *
 *      -> Eclairage de journée                                                 *
 *   - 1 Jaune x 1 = 10 LED                                                     *
 *      -> 1 LED jaune = 180 lm                                                 *
 *      -> 10 LED 1800 lm                                                       *
 *      -> Ciel jaune                                                           *
 *   - 1 Rouge x 1 = 10 LED                                                     *
 *      -> 1 LED rouge = 140 lm                                                 *
 *      -> 10 LED 140 lm                                                        *
 *      -> Ciel rouge                                                           *
 *   - 1 Bleu x 1 = 10 LED                                                      *
 *      -> 1 LED bleu = 80 lm                                                   *
 *      -> 10 LED 800 lm                                                        *
 *      -> Nuit                                                                 *
 *   => 6 circuits piloté en PWM                                                *
 *                                                                              *
 *                                                                              *
 ********************************************************************************
 *   Structure des messages recu par le port serie                              *
 **PC->Arduino :un état d'éclairage de départ envoyé brut <SDpdfdfdfdfdfdf>     *
 **PC->Arduino :un état d'éclairage à la suite envoyé brut <SSpffffff>          *
 * S:State D ou S:Départ ou Suite p:pause d:valeur début f:valeur fin           *
 **Message spéciaux <Xav> avec a = R:Reboot a = A:Tout, a = 1-6:led couleur     *
 *  v = 1:Allumé v = 0:eteint                                                   *
 **Demande lecture <La> avec a = T:température a = 1-6:led couleur info         *
 **Ce module est le numero 2                                                    *
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

#include <avr/pgmspace.h> // Inclusion de la librairie PGMSPACE qui permet l'utilisation de PROGMEM
#include <EEPROM.h>
#include <OneWire.h>

//Activation et configuration de la librairie Mirf pour la communication radio
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#include <Creche.h>

PROGMEM const String devise = "Mon Dieu faite que je serve uniquement a votre plus grande gloire";

//---------------------------VARIABLES--------------------------------

//Tableau avec l'état de chacune des LED PWM
unsigned long ledmillis[6] = {
  0,0,0,0,0,0};//heure de la dernière action sur cette LED
byte ledata[6][7] = {
  {
    'T',0,0,0,10,0,4        }//4
  , //Blanc
  {
    'T',0,0,0,10,0,5        }//5
  , //Blanc
  {
    'T',0,0,0,10,0,11        }//11
  , //Blanc
  {
    'T',0,0,0,10,0,10        }//10
  , //Rouge
  {
    'T',0,0,0,10,0,3        }//3
  , //Orange
  {
    'T',0,0,0,10,0,7        }//7
};//Bleu
/*
 -type de fonctionnement (0)
 -demande progressif, ou position séquence vacillement (1)
 -luminosité actuelle, (2)
 -variation (3)
 -intervalle de temps entre deux actions de variation 1ms -> FFms (4)
 -multiplicatif d'intervalle de temps entre deux actions de variation 100ms -> FF00ms (5)
 -pin sur lequel est connecté le circuit LED en question (6)
 */

//Variables pour le WatchDog (timer 555), reset après 9s de silence
BOUTON Bouton1(26);

//Variables pour le controle du temps
unsigned long previusmillis = 0;//Heure de la dernière action enregistrée
unsigned long sequencemilis = 0;

//Variable qui indique si le système est allumé ou éteint
boolean StateAlim1240;
boolean LastStateAlim1240;

//Variables pour les échanges de données structurées par sériale
String rxCmd ; // Commande reçue (sans délimiteurs)
String txCmd ; // Commande à transmettre//Variables pour les échanges de données structurées par sériale
String fCmd[3]; //Message reçu par les cannaux de communication

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

//Led bicolore
#define LedVerte 25 // Led temoin bicolore en vert
#define LedRouge 24 // Led témoin bicolor rouge

//Variable de puissance instantanée et energie consomée
int puissance = 0; //puissance instantanée
unsigned long puissancemillis = 0; //heure de la dernière action car la mise à jours
//ne se fait pas exactement a 1s

//Nombre de messages qui doivent etre envoyé depuis le serveur
//ATTENTION Avec 15 message on utilise l'intégralité de la RAM
#define NbMessage 15
//Organisation du tableau de l'etat instantanée des lumière, stoker dans la RAM
// 0  -on/off du portique
// 1  -blanc 1 pwm
// 2  -blanc 2 pwm
// 3  -blanc 3 pwm
// 4  -rouge pwm
// 5  -orange pwm
// 6  -bleu pwm
// 7  -9 spots à 20° 1 Rouge, 2 vert, 1 jaune, 5 blanc chaud
// 8  -libre
// 9  -libre
// 10 -libre
// 11 -libre
// 12 -libre
// 13 -libre
// 14 -state validation & validation de synchronisation

//Variables et librairies pour la sonde de température
byte present = 0;
byte type_s;
byte data[12];
byte addr[8];
float temperature;      //Variable avec la température
boolean capture;  //Permet d'indiquer si le programme fait une demande de mesure ou un relever de mesure
boolean sonde; //La sonde de température est bien connectée ou non
unsigned long captureTime = 0; //Heure de la mesure
// OneWire DS18S20, DS18B20, DS1822 Temperature application
OneWire  ds(22);  // on pin 22 (a 4.7K resistor is necessary)
// constants won't change. Used here to 
// set pin numbers:
float tempalu; //Temparature de l'aluminium
float tempamb; //Temparature ambiante (valeur prise au moment de l'alumage)
float tempaluC; //Temparature de l'aluminium obtenu par calcul

//---------------------------------FONCTIONS------------------------------------

//Fonction d'initialisation de la sonde de température
//Indique sur le Sériale la cause des problèmes si nécessaire
boolean InitTemperature(void){
  if ( !ds.search(addr)) {
    //Serial.println("No more addresses.");
    //Serial.println();
    ds.reset_search();
    delay(250);
    return false;
  }
  //Serial.print("ROM =");
  for(byte i = 0; i < 8; i++) {
    //Serial.write(' ');
    //Serial.print(addr[i], HEX);
  }
  if (OneWire::crc8(addr, 7) != addr[7]) {
    //Serial.println("CRC is not valid!");
    return false;
  }
  // the first ROM byte indicates which chip
  switch (addr[0]) {
  case 0x10:
    //Serial.println("  Chip = DS18S20");  // or old DS1820
    type_s = 1;
    break;
  case 0x28:
    //Serial.println("  Chip = DS18B20");
    type_s = 0;
    break;
  case 0x22:
    //Serial.println("  Chip = DS1822");
    type_s = 0;
    break;
  default:
    //Serial.println("Device is not a DS18x20 family device.");
    return false;
  } 
}

// Fonction récupérant la température depuis le DS18B20
// Retourne true si tout va bien, ou false en cas d'erreur
boolean getTemperature(float *temp){
  if (CR.currentMillis - captureTime < 1000) return false;
  //Sauvegarde de l'heure du relevé
  captureTime = CR.currentMillis;
  if (!capture) {
    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1);        // start conversion, with parasite power on at the end
    capture = true;
    // Pas d'erreur, mais pas de mesure
    return false;
  }
  else {  
    present = ds.reset();
    ds.select(addr);    
    ds.write(0xBE);         // Read Scratchpad
    for (byte i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = ds.read();
    }
    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s) {
      raw = raw << 3; // 9 bit resolution default
      if (data[7] == 0x10) {
        // "count remain" gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
    } 
    else {
      byte cfg = (data[4] & 0x60);
      // at lower res, the low bits are undefined, so let's zero them
      if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
      //// default is 12 bit resolution, 750 ms conversion time
    }
    *temp = (float)raw / 16.0;
    //Envois de la valeur de puissance vers le module BCD_M3
    txCmd = "M03M02ET" + ByteToString(*temp);
    Send(Module);
    capture = false;
    // Pas d'erreur
    return true;
  }
}

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

//Traitement d'un message valide reçu
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
        if (desti == 2) {
           txCmd = fCmd[canal][3] + ByteToString(envoyeur) + "M02";
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
           txCmd = fCmd[canal][3] + ByteToString(envoyeur) + "M02";
           //Les 6 premier carractère du message concer l'adressage, le module prend cette information séparement pour un éventuel retour de message
           TreatCommand(fCmd[canal].substring(6));
           //On fait suivre le message vers les autres modules qui ne l'auraient pas encore reçu
           txCmd = fCmd[canal];
           Send(canal);
        break ;
      default:
        //Le message ne correspond a rien
        txCmd = "M00M02ErrMessageM2";
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
    StringToByte(desti, txCmd, 1);
    StringToByte(envoyeur, txCmd, 4);
    String cmd = '<' + txCmd + ">@";
    if (canal != Radio) {
        //Message a destination de M1 ou M4
        if (desti == Serveur_m1 || desti == BoutonServeur_m4 || ((cmd[1] == 'A' || cmd[1] == 'a') && envoyeur != Serveur_m1 && envoyeur != BoutonServeur_m4)) {
            Mirf.setTADDR((byte *)"nrf01");
            MirfSend(cmd);
        }
        if (desti == Pc_m0 && (cmd[1] == 'M' || cmd[1] == 'm') && envoyeur != Serveur_m1 && envoyeur != BoutonServeur_m4) {
            Mirf.setTADDR((byte *)"nrf01");
            MirfSend(cmd);
        }
        //Message a destination de M5
        if (desti == Lanterne_m5 || ((cmd[1] == 'A' || cmd[1] == 'a') && envoyeur != Lanterne_m5)) {
            Mirf.setTADDR((byte *)"nrf05");
            MirfSend(cmd);
        }
        if (desti == Pc_m0 && (cmd[1] == 'M' || cmd[1] == 'm') && envoyeur != Lanterne_m5) {
            Mirf.setTADDR((byte *)"nrf05");
            MirfSend(cmd);
        }
    }
    if (canal != Seria) {
        //Message a destination du PC
        if (desti == Pc_m0 || cmd[1] == 'A' || cmd[1] == 'a') {
            Serial.println(cmd);
        }
    }
    if (canal != Seria2) {
        //Messages a destination M3
        if (desti == Dbc_m3 || (cmd[1] == 'A' || cmd[1] == 'a') && envoyeur != Dbc_m3) {
            Serial2.print(cmd);
        }
    }
}

void CmdError(String type)
// Doit être appelé quand il y a une erreur dans rxCmd
{
  txCmd = txCmd + "Z ERR " + type;
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
    case 'K' : case 'k' : 
      //Message reçu : <(K Led)...>
      EcritureLed() ;
      break ;
    case 'D' : case 'd' : 
      //Message reçu : <(Demarrage)...>
      //Un module x vient de redémrrer peut-etre qu'il faut lui envoyer une commande de mise a jour
      StringToByte(desti,rxCmd,1);
      if (desti == 3 && !StateAlim1240) {
         txCmd = txCmd + "XB0";
         Send(Module);
      }
      break ;
    case 'P' : case 'p' : 
      //Message reçu : <(Présent)...>
      //Un module x vient de répondre présent suite a une requete
      StringToByte(desti,rxCmd,1);
      break;
    case 'Z' : case 'z' : 
      //Message reçu : <(Message sans effet)...>
      //Rien a faire ici
      //Permet de faire passer une indication a l'utilisateur
      break ;
    default:   
      CmdError("type d'operation (X.L.K.D.P.Z)");
      break ;      
  }
}

//Fonction qui renvoit la valeur des variables demandée
void LectureVariable() {
  String mes = " ";
  byte led;
  switch (rxCmd[1])
  {
    case 'T' : case 't' :
       //Message reçu: <(Lecture)(Température)>
       //Message envoyé: <(Lecture)(Température)(température..)>
       txCmd = txCmd + "LT" + ((sonde)?("AL" + ByteToString(tempalu) + "AMB" + ByteToString(tempamb)):"NoDevice");
       Send(Module);
       break ;
    case 'P' : case 'p' :
       //Message reçu: <(Lecture)(Puissance)>
       //Message envoyé: <(Lecture)(Puissance)(puissance..)>
       txCmd = txCmd + "LP" + ByteToString(puissance);
       Send(Module);
       break ;
    case 'M' : case 'm' :
       //Message reçu: <(Lecture)(Memoire)>
       //Message envoyé: <(Lecture)(Memoire)(memoire..)>
       txCmd = txCmd + "LM" + WordToString(freeMemory());
       Send(Module);
       break ;
    case 'L' : case 'l' :
       //Message reçu: <(Lecture)(Led)(led)>
       //Message envoyé: <(Lecture)(Led)(led)>
       //Format "HH"
       //Format "HHTA\DHHLHHVHHTHHHHPHH"
       StringToByte(led,rxCmd,2);
       if (led == 1 || led == 2 || led == 3 || led == 4 || led == 5 || led == 6) {
          txCmd = txCmd + "LL" + ByteToString(led) + "T0";
          txCmd.setCharAt(txCmd.length()-1, ledata[led-1][0]);
          txCmd = txCmd + "D" + ByteToString(ledata[led-1][1]) + "L" + ByteToString(ledata[led-1][2]) + "V" + ByteToString(ledata[led-1][3]) + "T" + ByteToString(ledata[led-1][5]) + ByteToString(ledata[led-1][4]);
          Send(Module);
       }
       else {
          CmdError("LL led 01.02.03.04.05.06");
       }
       break;
    case 'O' : case 'o' :
       //Message reçu: <(Lecture)(On/Off)>
       //Message envoyé: <(Lecture)(On/Off)(StateAlim1240..)>
       txCmd = txCmd + "LO";
       if (StateAlim1240) txCmd = txCmd + "1";
       else txCmd = txCmd + "0";
       Send(Module);
       break;
    default:   
      CmdError("L Type de lecture (T.P.M.L.O)");
  } 
}

//Fonction qui fait changer la luminosité des LED
void luminosite(byte *type, byte *demande, byte *lumino, byte *variation, byte *inter, byte *multinter, byte *pin) {
   switch (*type) {
      case 'T' : //Tout ou rien
         //rien a faire ici
         digitalWrite(LedRouge,LOW);
         digitalWrite(LedVerte,HIGH);
         break;
      case 'P' : //Progressif
         //Cas si on demande d'augmenter la luminosité
         if (*demande >= *lumino + *variation) {
            *lumino = *lumino + *variation; //Augmentation de la luminosité
            digitalWrite(LedRouge,HIGH);
            digitalWrite(LedVerte,LOW);
         }
         else if (*demande > *lumino) // La luminosité est proche de la valeur demandée
            *lumino = *demande;
         //Cas si on demande de la diminuée
         if (*demande <= *lumino - *variation) {
            *lumino = *lumino - *variation; //Diminution de la luminosité
            digitalWrite(LedRouge,HIGH);
            digitalWrite(LedVerte,LOW);
         }
         else if (*demande < *lumino) //
            *lumino = *demande;
         //Cas ou nous avons atteint la luminosité souhaité
         if (*demande == *lumino) {
            *inter = 0xFF; //on augmente l'intervalle pour éviter de surcharger le PIC
            *multinter = 0x08;
            digitalWrite(LedRouge,LOW);
            digitalWrite(LedVerte,HIGH);
         }
         break;
      case 'C' : //Clignotement franc
         *lumino = 255 - *lumino;
         digitalWrite(LedRouge,LOW);
         digitalWrite(LedVerte,HIGH);
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
             digitalWrite(LedRouge,HIGH);
             digitalWrite(LedVerte,LOW);
         }
         break;
      case 'R' : //Run (Clignote avec variation progressive)
         if (*demande == 1) *lumino += *variation;
         else *lumino -= *variation;
         if (*lumino > 255 - *variation) *demande = 0;
         if (*lumino < 1 + *variation) *demande = 1;
         digitalWrite(LedRouge,HIGH);
         digitalWrite(LedVerte,LOW);
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
   if (led < 1 || led > 6)
      CmdError(" led 01.02.03.04.05.06");
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

//Fonction qui réagit a des commandes spéciales
void DonneSpeciaux() {
  byte led;
  switch (rxCmd[1])
  {
    case 'R' : case 'r' :
       //Message reçu: <(X opération)(Reboot)>
       while ( rxCmd[1] < 'T');
    case 'B' : case 'b' :
       //Message reçu: <(X opération)(Boutton)>
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
       txCmd = txCmd + "P02";
       Send(Module);
       break;
    default:   
      CmdError("X action speciale (R.A.L.B.P)");           
  } 
}

//Fonction SETUP
void setup() {

  //Initialise les Serial
  Serial.begin(9600) ;
  Serial2.begin(9600);

  //Initialisation de la librairie Creche
  CR.configure(12);
  CR.check(true);
  
  //Initialisation de la librairie Mirf
  Mirf.cePin = 48; // CE sur D8
  Mirf.csnPin = 53; // CSN sur D7
  Mirf.spi = &MirfHardwareSpi; // On veut utiliser le port SPI hardware
  Mirf.init(); // Tout est bon ? Ok let's go !
  Mirf.channel = 25; // On va utiliser le canal 0 pour communiquer (128 canaux disponible, de 0 à 127)
  Mirf.payload = sizeof(byte); // = 4, ici il faut déclarer la taille du "payload" soit du message qu'on va transmettre, au max 32 octets
  Mirf.config(); // Tout est bon ? Ok let's go !
  Mirf.setRADDR((byte *)"nrf02"); // On définit ici l'adresse du module en question

  //Commande demmarrage du module (attenttion réécrire le numéro du module après type démarage)
  txCmd = "A00M02D02";
  Send(Module);

  //Initialisation de la Led témoin rouge
  pinMode(LedRouge, OUTPUT);
  //Initialisation de la Led témoin vert
  pinMode(LedVerte, OUTPUT);

  //Au démarrage on considère que le système est allumé
  LastStateAlim1240 = true;
  StateAlim1240 = true;
  
  //Etat du boutton initial
  Bouton1.state = true;
  
  //Initialisation des pins utilisée par la PWM
  for (byte led = 0; led < 6; led++)
    pinMode(ledata[led][4],OUTPUT);

  //Appel de la fonction d'initialisation de la sonde de température
  (InitTemperature())?sonde = true:sonde = false;

  //1er capture de la température ambiante
  if (sonde){
    if(getTemperature(&tempamb)) {
      //La température ambianté de départ est aussi celle de l'aluminium
      tempalu = tempamb;
      //On envois un signal sonde connectée + val T
      txCmd = "A00M02LTAL" + ByteToString(tempalu) + "AMB" + ByteToString(tempamb);
      Send(Module);
    }
  }
  else {
    tempamb = 20;
    tempalu = 20;
   //On envois un signal sonde déconnectée
   txCmd = "A00M02LTNoDevice";
   Send(Module);
  }
}

//Fonction LOOP
void loop(){
  // controle par la librairie Creche
  CR.check(true);
  //Lecture de chaque bouton
  Bouton1.check(&CR.currentMillis);

  //action sur les LED
  if (StateAlim1240) { //La condition sur le relais de l'alimentation générale est désactivée car déconnection électrique
     for (byte led = 0; led < 6; led++){
       //s'il est temps de changer l'état de la LED on entre dans la condition
       if (CR.currentMillis - ledmillis[led] > (ledata[led][3])) {
         //enregistrement de l'heure de l'action, le retard d'action est comptabilisé
         //ledmillis[led] = 2 * CR.currentMillis - ledmillis[led] - ledata[led][3];
         ledmillis[led] = CR.currentMillis;
         luminosite(&ledata[led][0], &ledata[led][1], &ledata[led][2], &ledata[led][3], &ledata[led][4], &ledata[led][5], &ledata[led][6]);
       }
     }
     //On vient de réallumé le système
     if (!LastStateAlim1240) LastStateAlim1240 = true;
  }
  else { // l'alimentation est éteinte, alors on fait clignoter la LED rouge rapidement
    if (CR.currentMillis - ledmillis[0] > 200) {
      //enregistrement de l'heure de l'action
      ledmillis[0] = CR.currentMillis;
      if (ledata[0][1]){
        ledata[0][1] = 0;
        digitalWrite(LedRouge,HIGH);
        digitalWrite(LedVerte,LOW);
      }
      else{
        ledata[0][1] = 1;
        digitalWrite(LedRouge,LOW);
        digitalWrite(LedVerte,LOW);
      }
    }
    //On vient d'éteindre le système
    if (LastStateAlim1240) {
       LastStateAlim1240 = false;
       //mise à zero de toute les LED de puissance
       byte TOR = 'T';
       for (byte led = 1; led < 6; led++) {
          luminosite(&TOR, 0, 0, &ledata[led][3], &ledata[led][4], &ledata[led][5], &ledata[led][6]);
       }
    }
  }

  //Gestion de la réception de commande via le serial
  byte car ;
  while (Serial.available())
      TreatChar(Serial.read(), Seria);
  if(!Mirf.isSending() && Mirf.dataReady()){
    Mirf.getData(&car);
    TreatChar(car, Radio);    
  }
  while (Serial2.available())
      TreatChar(Serial2.read(), Seria2);

  //Gestion de l'etat on/off du système
  if (Bouton1.FrontState) {
     if (Bouton1.state){ //recopie de l'etat de state sur StateAlim1240
       StateAlim1240 = 1;
       //On envois un signal général appuis sur le boutton on/off
       txCmd = "A00M02XB1";
       Send(Module);
     }
     else {
       StateAlim1240 = 0;
       //On envois un signal général appuis sur le boutton on/off
       txCmd = "A00M02XB0";
       Send(Module);
     }
  }

  //Capture, affichage et controle de la température in
  if (sonde){
    if(getTemperature(&tempalu)) {
      //si la rempérature dépasse 50°C dans le radiateur, on coupe l'alimentation 12V 40A
      //et le programme continu a tourner dans le vide. Seule une pression
      //sur le boutton reset permettra de relancer la machine.
      if (tempalu > 50) {
         StateAlim1240 = 0;
         //On envois un signal général appuis sur le boutton on/off
         txCmd = "A00M02XB0";
         Send(Module);
      }
    }
  }

  //Calcul de la puissance
  if(CR.currentMillis - puissancemillis > 1000){
    int puis = 0;
    //Somme des puissances instantanée de toute les LED
    for(byte a = 0; a < 6; a++) puis = puis + ledata[a][1];
    // On transforme la valeur en base 256 en une puissance en W : 255 luminosité => 50W
    // 6 * 50W = 300W, et on ramène la valeur en % => puis * 50 / (255 * 3) => 10 / 153
    puissance = puis * 10 / 153;
    //Controle que la puissance demandée ne soit pas supérieur aux capacités de l'alimentation
    //if (puissance / 12 > 40) StateAlim1240 = 0;
    //Sauvegarde de l'heure du calcul avec prise en compte du retard
    //puissancemillis = CR.currentMillis;
    puissancemillis = 2 * CR.currentMillis - puissancemillis - 1000;
    //Envois de la valeur de puissance vers le module BCD_M3
    txCmd = "M03M02EP" + ByteToString(puissance);
    Send(Module);
  }

}



