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
 *                                SUR ARDUINO MEGA,                             *
 *       A UTILISER UNIQUEMENT AVEC LA PROTOSHIELD ADAPTE A CE PROGRAMME        *
 *                                  MODULE 1                                    *
 *                      Cette carte est composée:                               *
 *          -Lecteur de carte micro SD                                          *
 *          -De la mémoire RAM                                                  *
 *          -Module NRf24l01+                                                   *
 *          -WatchDog Harware avec un 555, avec une tempos de 9s                *
 *          -Module mémoire eeprom qui utilise le bus I²C                       *
 *                                                                              *
 ********************************************************************************
 *                                                                              *
 *          FONCTION:                                                           *
 *Module qui contient toute la séquence d'éclairage et la transmet les commandes*
 *d'éclairage par radio aux autre module. Les étapes sont successivement        *
 *enregistrer dans la mémoire eeprom pour qu'en cas de reboot le serveur        *
 *ne perde pas l'ordre de la séquence.                                          *
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

//Activation et configuration de la librairie Mirf pour la communication radio
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <Wire.h>
#include <MirfHardwareSpiDriver.h>
#include <avr/pgmspace.h> // Inclusion de la librairie PGMSPACE qui permet l'utilisation de PROGMEM
#include <EEPROM.h>
#include <SpiRAM.h>
#include <SD.h>

#include <Creche.h>

#define DS1307_ADDRESS 0x68 // Adresse I2C du module DS1307

//PROGMEM const String devise = "Mon Dieu faite que je serve uniquement a votre plus grande gloire";

//---------------------------VARIABLES--------------------------------

//Variable qui indique si le système est allumé ou éteint
boolean StateAlim1240;
boolean LastStateAlim1240;

//Variables pour les échanges de données structurées par sériale
String rxCmd ; // Commande reçue (sans délimiteurs)
String txCmd ; // Commande à transmettre

String fCmd[4]; //Message reçu par les cannaux de communication

#define RAM_SS_PIN 47
#define SD_SS_PIN 49

SpiRAM spiRam(0, RAM_SS_PIN);

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

//Variable pour le block executable
unsigned long ExeTime = 0; //Heure de l'action
boolean actif = false, validation = false, synchronisation = false;
int pause = 1;
File ExeFile;
int NumLigne = 0;
word IndiceFile = word('0', '0'); //Nom des fichier programmes: Exe_00.cre
//avec le 00 qui est remplacer par deux char qui identifient le fichier

//Donnée pour le fichier de config
#define BUFFER_SIZE 40

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
    logFile(fCmd[canal], "Recep", canal);
    AdressageMessage(canal);
    break;
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
        if (desti == Serveur_m1) {
           txCmd = fCmd[canal][3] + ByteToString(envoyeur) + "M01";
           //Les 6 premier carractère du message concerne l'adressage, le module prend cette information séparement pour un éventuel retour de message
           TreatCommand(fCmd[canal].substring(6));
        }
        else {
            //Si le message n'est pas a destination de ce module, c'est qu'il est attendu sur un autre module, donc on le fait suivre
            txCmd = fCmd[canal];
            if (desti < 0x10 && envoyeur < 0x10  && fCmd[canal][3] == 'M') //On fait suivre uniquement si l'adressage est correcte
               Send(canal);
            else {
               txCmd = "M00M01ErrMessageM1";
               Send(Module);
            }
        }
        break ;
      case 'A' : case 'a' :
           txCmd = fCmd[canal][3] + ByteToString(envoyeur) + "M01";
           //Les 6 premier carractère du message concer l'adressage, le module prend cette information séparement pour un éventuel retour de message
           TreatCommand(fCmd[canal].substring(6));
           //On fait suivre le message vers les autres modules qui ne l'auraient pas encore reçu
           txCmd = fCmd[canal];
            if (desti < 0x10 && envoyeur < 0x10 && fCmd[canal][3] == 'M') //On fait suivre uniquement si l'adressage est correcte
               Send(canal);
            else {
               txCmd = "M00M01ErrMessageM1";
               Send(Module);
            }
        break ;
      default:
        //Le message ne correspond a rien
        txCmd = "M00M01ErrMessageM1";
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
    logFile(txCmd, "Send", canal);
    if (canal != Radio) {
       //Message a destination de M2
       if (desti == Pwm_m2 || desti == Dbc_m3 || ((cmd[1] == 'A' || cmd[1] == 'a') && envoyeur != Pwm_m2 && envoyeur != Dbc_m3)) {
           Mirf.setTADDR((byte *)"nrf02");
           MirfSend(cmd);
       }
       if (desti == Pc_m0 && (cmd[1] == 'M' || cmd[1] == 'm') && envoyeur != Pwm_m2 && envoyeur != Dbc_m3) {
           Mirf.setTADDR((byte *)"nrf02");
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
       //Messages a destination M4
       if (desti == BoutonServeur_m4 || (cmd[1] == 'A' || cmd[1] == 'a') && envoyeur != BoutonServeur_m4) {
           Serial2.print(cmd);
       }
    }
}

void CmdError(String type)
// Doit être appelé quand il y a une erreur dans rxCmd
{
  txCmd = txCmd + "Z ERR " + type;
  Send(Module);
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
    case 'R' : case 'r' :
      //Message reçu : <(RUN exécutable)...>
      modifExecutable() ;
      break ;
    case 'D' : case 'd' :
      //Message reçu : <(Demarrage)...>
      //Un module x vient de redémrrer peut-etre quil faut lui envoyer une commande de mise a jour
      //Format "\DHH"
      //On enregistre dans la mémoire la présence du module depuis l'adresse 0101 pour M1, 0102 pour M2 etc
      saveRAM("M" + rxCmd.substring(1,3) + "XDA01" + rxCmd.substring(1,3) + "V0001");
      break ;
    case 'P' : case 'p' :
      //Message reçu : <(Présent)...>
      //Un module x vient de répondre présent suite a une requete
      //Format "PHH"
      //On enregistre dans la mémoire la présence du module depuis l'adresse 0101 pour M1, 0102 pour M2 etc
      saveRAM("M" + rxCmd.substring(1,3) + "XPA01" + rxCmd.substring(1,3) + "V0001");
      break ;
    case 'Z' : case 'z' : 
      //Message reçu : <(Message sans effet)...>
      //Rien a faire ici
      //Permet de faire passer une indication a l'utilisateur
      break ;
    default:   
      CmdError("type d'operation (X.L.E.D.P.Z)");
      break ;      
  }
}

//Fonction qui permet l'ecriture de certaine variable
void EcritureVariable () {
  Date date;
  byte *adresse, val;
  switch (rxCmd[1])
  {
    case 'D' : case 'd' : //Mise a jour de l'heure système
       //Message reçu: <(Ecriture)(Date)>
       //Message envoyé: <(Ecriture)(Date)(jj/mm/aaRjjHhh:mm:ss)>
       //Format "HH/HH/HHRHH\HHH:HH:HH"
       StringToByte(date.jour, rxCmd, 2);
       StringToByte(date.mois, rxCmd, 5);
       StringToByte(date.annee, rxCmd, 8);
       StringToByte(date.jourDeLaSemaine, rxCmd, 11);
       StringToByte(date.heures, rxCmd, 14);
       StringToByte(date.minutes, rxCmd, 17);
       StringToByte(date.secondes, rxCmd, 20);
       ecrire(&date);
       break;
    case 'R' : case 'r' : //Réception d'une varible pour la RAM
       //Message reçu: <(Ecriture)(RAM)(Module)(TypeCmd)(Cmd)(Adresse)(Valeur)>
       //Format "MHHAA\AHHHHVHHHH"
       saveRAM(rxCmd.substring(2));
       break;
    case 'W' : case 'w' :
       //Message reçu: <(Ecriture)(Adresse radio)(Temporaire/Permanent)(Adresse)>
       //Format "AHH"
       if (rxCmd[2] == 'T' || rxCmd[2] == 't') {
         adresse[3] = rxCmd[3];
         adresse[4] = rxCmd[4];
         adresse[0] = 'n';
         adresse[1] = 'r';
         adresse[2] = 'f';
         Mirf.setTADDR((uint8_t *) adresse);
       }
       if (rxCmd[2] == 'P' || rxCmd[2] == 'p') {
         adresse[3] = rxCmd[3];
         adresse[4] = rxCmd[4];
         adresse[0] = 'n';
         adresse[1] = 'r';
         adresse[2] = 'f';
         Mirf.setTADDR((uint8_t *) adresse);
         EEPROM.write(0,rxCmd[3]);
         EEPROM.write(1,rxCmd[4]);
       }
       break;
    default:
      CmdError("E Type d'ecriture (D.R.W)");
  }
}

//Fonction qui renvoit la valeur des variables demandée
void LectureVariable() {
  Date date;
  // set up variables using the SD utility library functions:
  /*
  Le message de lecture variable ne renvois plus de message au demandeur.
  -Il envois un message pour la RAM de M1 avec la valeur a y inscrire
     *pas de risque de message en boucle
  -M1 renvois automatiquement un message vers M0 avec le contenu recopié
     *seul l'utilisateur et l'executant sont suceptible de connaitre la valeur
     *Permet d'introduire des conditions sur les varibles de tous le système
  */
  word address = 0;
  word JourAnnee, MinuteJournee;
  switch (rxCmd[1])
  {
    case 'M' : case 'm' :
       //Message reçu: <(Lecture)(Memoire)(Adresse)>
       //Format reçu: "HHHH"
       //Format envoyé à la RAM: "MHHAA\AHHHHVHHHH"
       saveRAM("M01LMA" + rxCmd.substring(2,6) + "V" + WordToString(freeMemory()));
       break ;
    case 'U' : case 'u' :
       //Message reçu: <(Lecture)(battrie)(Adresse)>
       //Message envoyé: <(Lecture)(battrie)(tension battrie..)>
       //Format reçu: "HHHH"
       //Format envoyé à la RAM: "MHHAA\AHHHHVHHHH"
       saveRAM("M01LUA" + rxCmd.substring(2,6) + "V" + WordToString(Tbatterie()));
       break ;
    case 'D' : case 'd' :
       //Message reçu: <(Lecture)(Date)(Adresse)>
       //Message envoyé: <(Lecture)(Date)(jj/mm/aaRjjHhh:mm:ss)>
       //Format "AHHHH"
       //Format envoyé à la RAM: "MHHAA\AHHHHVHHHH"
       lire(&date);
       switch (rxCmd[2]) {
          case 'D': case 'd' : //Jour du mois
             saveRAM("M01ADA" + rxCmd.substring(3,7) + "V00" + ByteToString(date.jour));
             break;
          case 'M': case 'm' : //Mois
             saveRAM("M01AMA" + rxCmd.substring(3,7) + "V00" + ByteToString(date.mois));
             break;
          case 'Y': case 'y' : //Année
             saveRAM("M01AYA" + rxCmd.substring(3,7) + "V00" + ByteToString(date.annee));
             break;
          case 'W': case 'w' : //Jour de le semaine
             saveRAM("M01AWA" + rxCmd.substring(3,7) + "V00" + ByteToString(date.jourDeLaSemaine));
             break;
          case 'H': case 'h' : //Heure
             saveRAM("M01AHA" + rxCmd.substring(3,7) + "V00" + ByteToString(date.heures));
             break;
          case 'N': case 'n' : //Minute
             saveRAM("M01ANA" + rxCmd.substring(3,7) + "V00" + ByteToString(date.minutes));
             break;
          case 'S': case 's' : //Seconde
             saveRAM("M01ASA" + rxCmd.substring(3,7) + "V00" + ByteToString(date.secondes));
             break;
          case 'X': case 'x' : //Jour dans l'année
             JourAnnee = date.jour;
             //permet de calculer le nombre de jours des mois passés, jusqu'au mois actuel
             for (byte i = 1; i < date.mois; i++)
                JourAnnee = JourAnnee + nbjourmois(&i);
             saveRAM("M01AXA" + rxCmd.substring(3,7) + "V" + WordToString(JourAnnee));
             break;
          case 'Z': case 'z' : //Minute dans la journée
             MinuteJournee = date.minutes;
             MinuteJournee = MinuteJournee + date.heures * 60;
             saveRAM("M01AZA" + rxCmd.substring(3,7) + "V" + WordToString(MinuteJournee));
             break;
          default :
             CmdError("Date erroné (D.M.Y.W.H.N.S.X.Z)");
       }
       break ;
    case 'O' : case 'o' :
       //Message reçu: <(Lecture)(On/Off)>
       //Message envoyé: <(Lecture)(On/Off)(StateAlim1240..)>
       //Format "HHHH"
       //Format "MHHAA\AHHHHVHHHH"
       if (StateAlim1240)
          saveRAM("M01LOA" + rxCmd.substring(2,6) + "V0001");
       else
          saveRAM("M01LOA" + rxCmd.substring(2,6) + "V0000");
       break;
    case 'R' : case 'r' :
       //Message reçu: <(Lecture (L))(RAM)(adresse)>
       //Message envoyé: <(Lecture (T))(Ram)(valeur)>
       //Format "HHHH"
       //Format renvoyé "\AHHHHVHHHH"
       //ATTENTION le module d'envois le type et la commande sont celle enregistrer, pas les valeurs réelle
       StringToWord(address, rxCmd, 2);
       address *= 5; //Caque variable est stokée sur 2 bytes
       address += 9;
       if (address > 13 && address < 0x7FFA) { //Les première adresse sont réservé pour l'Executant et limité par le composant
          txCmd = "M00M" + ByteToString((byte)spiRam.read_byte((int)address)) + (char)spiRam.read_byte((int)address + 1) + (char)spiRam.read_byte((int)address + 2) + ByteToString((byte)spiRam.read_byte((int)address + 3)) + ByteToString((byte)spiRam.read_byte((int)address + 4));
          Send(Module);
       }
       break;
    case 'S' : case 's' : //Message d'information
       //Message reçu: <(Lecture)(SD)>
       //Message envoyé: <(Lecture)(SD)(type)(espace mémoire)>
       //Format ""
       //Format "NNNNVHHHH"
       txCmd = txCmd + "LS" + SD.type() + "V" + WordToString((word)SD.size());
       Send(Module);
       break;
    case 'A' : case 'a' :
       //Message reçu: <(Lecture)(Actif)>
       //Message envoyé: <(Lecture)(Actif)(On/Off)>
       //Format "HHHH"
       //Format "B"
       if (actif)
          txCmd = txCmd + "RA1";
       else
          txCmd = txCmd + "RA0";
       Send(Module);
       break;
    case 'W' : case 'w' :
       //Message reçu: <(Lecture)(Adresse radio)>
       //Format "HHHH"
       //Format "\AHHHHVHHHH"
       saveRAM("M01LWA" + rxCmd.substring(2,6) + "V00" + (char) EEPROM.read(0) + (char) EEPROM.read(1));
       break;
    default:   
      CmdError("L Type lecture (M.U.D.O.S.R.A.W)");
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
       //Format "B"
       txCmd = txCmd + "P01";
       Send(Module);
       saveRAM("M01XPA0101V0001");
       break;
    default:   
      CmdError("X action speciale (R.B.P)");           
  } 
}

// Fonction de convertion bcd -> décimal
byte bcd2dec(byte bcd) {
  return ((bcd / 16 * 10) + (bcd % 16));
}

// Fonction de convertion decimal -> bcd
byte dec2bcd(byte dec) {
  return ((dec / 10 * 16) + (dec % 10));
}

//Fonction qui ecrit dans la ram de la RTC les variables necessaire pour le prog serveur
//sauf la startdate qui ne doit pas changer
void writeram(word *pause, boolean *actif, boolean *validation, boolean *synchronisation) {
  Wire.beginTransmission(DS1307_ADDRESS);
  //ecriture a partir de la position 0x08
  Wire.write(0x08);
  //ecriture du states (position dans la séquence  d'éclairage)
  Wire.write(*actif);
  //ecriture du mode (normal J/N, messe (sans effet), séquence1, autres)
  Wire.write(*validation);
  //ecriture de la priorité du mode dans la ram
  Wire.write(*synchronisation);
  Wire.endTransmission();
}

//Fonction qui lit les variables stoker dans la ram
void readram(byte *states, byte *mode, byte *priorite) {
  Wire.beginTransmission(DS1307_ADDRESS); // Début de transaction I2C
  Wire.write(0x08); // Demande les info à partir de l'adresse 0x0B (soit les infos LED)
  Wire.endTransmission(); // Fin de transaction I2C
  Wire.requestFrom(DS1307_ADDRESS, 3); // Récupère les info
  *states = Wire.read();
  *mode = Wire.read();
  *priorite = Wire.read();
}

// Fonction configurant le DS1307 avec la date/heure fourni
void ecrire(Date *date) {
  Wire.beginTransmission(DS1307_ADDRESS); // Début de transaction I2C
  Wire.write(0); // Arrête l'oscillateur du DS1307
  Wire.write(dec2bcd(date->secondes)); // Envoi des données
  Wire.write(dec2bcd(date->minutes));
  Wire.write(dec2bcd(date->heures));
  Wire.write(dec2bcd(date->jourDeLaSemaine));
  Wire.write(dec2bcd(date->jour));
  Wire.write(dec2bcd(date->mois));
  Wire.write(dec2bcd(date->annee));
  Wire.write(0); // Redémarre l'oscillateur du DS1307
  Wire.endTransmission(); // Fin de transaction I2C
}

//Retourne la tension de la pile 
int Tbatterie() {
    const int potar = 0; // la pin de mesure analogique 0
    int valeurLue; //variable pour stocker la valeur lue après conversion
    //float tension; //on convertit cette valeur en une tension
    //on convertit en nombre binaire la tension lue en sortie du potentiomètre
    valeurLue = analogRead(potar);
    //on traduit la valeur brute en tension (produit en croix)
    //tension = valeurLue * 5.0 / 1024;
    return valeurLue;
}

//Fonction qui retourne le nombre de jours d'un mois donné
byte nbjourmois(byte *mois){
  if (*mois == 1 || *mois == 3 || *mois == 5 || *mois == 7 || *mois == 8 || *mois == 10 || *mois == 12) return 31;
  if (*mois == 4 || *mois == 6 || *mois == 9 || *mois == 11) return 30;
  if (*mois == 2) return 28;
  return 0;
}

// Fonction récupérant l'heure, la date courante et toutes les variables stoker dans la ram du DS1307
void lire(Date *date) {
  Wire.beginTransmission(DS1307_ADDRESS); // Début de transaction I2C
  Wire.write(0); // Demande les info à partir de l'adresse 0 (soit toutes les info)
  Wire.endTransmission(); // Fin de transaction I2C
  Wire.requestFrom(DS1307_ADDRESS, 7); // Récupère les info (7 octets = 7 valeurs correspondant à l'heure et à la date courante)
  date->secondes = bcd2dec(Wire.read()); // stockage et conversion des données reçu
  date->minutes = bcd2dec(Wire.read());
  date->heures = bcd2dec(Wire.read() & 0b111111);
  date->jourDeLaSemaine = bcd2dec(Wire.read());
  date->jour = bcd2dec(Wire.read());
  date->mois = bcd2dec(Wire.read());
  date->annee = bcd2dec(Wire.read());
}

//Fonction qui rempli le fichier log sur la carte micro SD
void logFile(String mesg, String type, Mode canal) {
   File dataFile = SD.open("datalog.txt", FILE_WRITE);
   Date date;
   lire(&date);
   // if the file is available, write to it:
   if (dataFile) {
      dataFile.print(ByteToString(date.jour) + "/" + ByteToString(date.mois) + "/" + ByteToString(date.annee) + "R" + ByteToString(date.jourDeLaSemaine) + "H" + ByteToString(date.heures) + ":" + ByteToString(date.minutes) + ":" + ByteToString(date.secondes) + "\t");
      dataFile.print(type + "\t");
      switch(canal) {
         case Seria:
            dataFile.print("Seria\t");
            break;
         case Radio:
            dataFile.print("Radio\t");
            break;
         case Seria2:
            dataFile.print("Seria2\t");
            break;
         case Executant:
            dataFile.print("Execut\t");
            break;
         case Module:
            dataFile.print("Module\t");
            break;
         default :
            dataFile.print("No canal\t");
      }
      dataFile.println(mesg);
      dataFile.close();
   }  
}

//Fonction pour l'enregistrement dans la RAM des messages de lectures
void saveRAM(String mesg) {
   //Message reçu: <(Ecriture)(RAM)(Module)(TypeCmd)(Cmd)(Adresse)(Valeur)>
   //Format "MHHAA\AHHHHVHHHH"
   word address = 0;
   byte wValue0, wValue1,module, typeCmd, Cmd;
   StringToWord(address, mesg, 6);
   StringToByte(wValue0, mesg, 11);
   StringToByte(wValue1, mesg, 13);
   StringToByte(module, mesg, 1);
   typeCmd = mesg[3];
   Cmd = mesg[4];
   //Sauvegarde dans la RAM
   //Chaque variable comprend 1 byte pour identifier le module + typeCmd + Cmd + variable sur 2 bytes => 5B
   address *= 5;
   address += 9;
   if (address > 13 && address < 0x7FFA) { //Les première adresse sont réservé pour l'Executant et limité par le composant
      spiRam.write_byte((int)address, module);
      spiRam.write_byte((int)address + 1, typeCmd);
      spiRam.write_byte((int)address + 2, Cmd);
      spiRam.write_byte((int)address + 3, wValue0);
      spiRam.write_byte((int)address + 4, wValue1);
   }
   txCmd = "M00M" + ByteToString(module) + (char)mesg[3] + (char)mesg[4] + ByteToString((byte)spiRam.read_byte((int)address + 3)) + ByteToString((byte)spiRam.read_byte((int)address + 4));
   Send(Module);
}


//Fonction qui réagit au commande pour modifier le bloc executable
void modifExecutable () {
  word time, address, value, ligne, timeD;
  switch (rxCmd[1])
  {
    case 'A' : case 'a' :
       //(Run executable)(Actif)(On/Off)
       //Format : "B"
       switch (rxCmd[2])
       {
          case '1' :
             //On écrit dans la RAM l'état actif
             actif = true;
             //On allume la Led témoin rouge de M4
             txCmd = "M04M01KR01T1388";
             Send(Module);
             break;
          case '0' :
             //On écrit dans la RAM l'état actif
             actif = false;
             //On reprendra le programme au début
             NumLigne = 0x0000;
             IndiceFile = word('0', '0');
             //Permet a l'utilisateur de modifier ces valeur av de reprendre l'état actif
             //I.E. : pour executer un programme secondaire de test
             txCmd = "M04M01KT01O0";
             Send(Module);
             break;
       }
       break;
    case 'V' : case 'v' :
       //(Run executable)(Validation)
       //Format : "B"
       switch (rxCmd[2])
       {
          case '1' :
             validation = true;
             break;
          case '0' :
             validation = false;
             break;
       }
       break;
    case 'S' : case 's' :
       //(Run executable)(Synchronisation)
       //Format : "B"
       synchronisation = false;
       switch (rxCmd[2])
       {
          case '1' :
             synchronisation = true;
             break;
          case '0' :
             synchronisation = false;
             break;
       }
       break;
    case 'D' : case 'd' :
       //(Run executable)(Delais)(delais)
       //Format : "HHHH"
       word time;
       StringToWord(time, rxCmd, 2);
       pause = (int)time;
       break;
    case 'F' : case 'f' :
       //(Run executable)(File)(Condition ouverture fichier)
       //Format : "HHHHAAHHHHFNN"
       //adresse memoire de la condition, type de condition : =(E) >(S) <(I) !(D), adresse ou valeur a comparer et le code du fichier
       StringToWord(address, rxCmd, 2);
       StringToWord(value, rxCmd, 8);
       //On compare normalement une valeur a une adresse mémoire par défaut : rxCmd[7] == 'V'
       if (rxCmd[7] == 'A') { //On compare des valeurs entre 2 adresses mémoire
          value *= 5; //Chaque variable est stokée sur 5 bytes
          value += 9;
          if (value > 13 && value < 0x7FFA)
             timeD = word((byte)spiRam.read_byte((int)value + 3), (byte)spiRam.read_byte((int)value + 4));
       }
       address *= 5; //Chaque variable est stokée sur 5 bytes
       address += 9;
       if (address > 13 && address < 0x7FFA) { //Les première adresse sont réservé pour l'Executant et limité par le composant
          time = word((byte)spiRam.read_byte((int)address + 3), (byte)spiRam.read_byte((int)address + 4));
          //Pour les comparaison de valeurs et dans le cas d'une valeur, alors on réattribue la variable timeD
          switch(rxCmd[6]) {
             case 'E':
                if (rxCmd[7] == 'V') timeD = value;
                if (time == timeD) {
                   NumLigne = 0x0000;
                   IndiceFile = word(rxCmd[13], rxCmd[14]);
                }
                break;
             case 'S':
                if (rxCmd[7] == 'V') timeD = value;
                if (time > timeD) {
                   NumLigne = 0x0000;
                   IndiceFile = word(rxCmd[13], rxCmd[14]);
                }
                break;
             case 'I':
                if (rxCmd[7] == 'V') timeD = value;
                if (time < timeD) {
                   NumLigne = 0x0000;
                   IndiceFile = word(rxCmd[13], rxCmd[14]);
                }
                break;
             case 'D':
                if (rxCmd[7] == 'V') timeD = value;
                if (time != timeD) {
                   NumLigne = 0x0000;
                   IndiceFile = word(rxCmd[13], rxCmd[14]);
                }
                break;
             case 'M':
                if (rxCmd[7] == 'A' && spiRam.read_byte((int)address) == spiRam.read_byte((int)value)) {
                   IndiceFile = word(rxCmd[13], rxCmd[14]);
                   NumLigne = 0x0000;
                }
                break;
             case 'T':
                if (rxCmd[7] == 'A' && spiRam.read_byte((int)address + 1) == spiRam.read_byte((int)value + 1)) {
                   IndiceFile = word(rxCmd[13], rxCmd[14]);
                   NumLigne = 0x0000;
                }
                break;
             case 'C':
                if (rxCmd[7] == 'A' && spiRam.read_byte((int)address + 2) == spiRam.read_byte((int)value + 2)) {
                   IndiceFile = word(rxCmd[13], rxCmd[14]);
                   NumLigne = 0x0000;
                }
                break;
          }
       }
       break;
    case 'G' : case 'g' :
       //(Run executable)(Goto)(Condition pour goto)
       //Format : "HHHHAAHHHHAHHHH"
       //adresse memoire de la condition, type de condition : =(E) >(S) <(I) !(D), Module Type Commande
       //Adresse ou Valeur a comparer et la ligne à laquelle sauter, ou adresse avec la ligne où aller
       StringToWord(address, rxCmd, 2);
       StringToWord(value, rxCmd, 8);
       StringToWord(ligne, rxCmd, 13);
       //On compare normalement une valeur a une adresse mémoire par défaut : rxCmd[7] == 'V'
       if (rxCmd[7] == 'A') { //On compare des valeurs entre 2 adresses mémoire
          value *= 5; //Chaque variable est stokée sur 5 bytes
          value += 9;
          if (value > 13 && value < 0x7FFA)
             timeD = word((byte)spiRam.read_byte((int)value + 3), (byte)spiRam.read_byte((int)value + 4));
       }
       //On accède normalement à la ligne, sauf si le num de ligne est a une adresse mémoire : rxCmd[12] == 'L'
       if (rxCmd[12] == 'A') { //On compare des valeurs entre 2 adresses mémoire
          ligne *= 5; //Chaque variable est stokée sur 5 bytes
          ligne += 9;
          if (ligne > 13 && ligne < 0x7FFA)
             ligne = word((byte)spiRam.read_byte((int)ligne + 3), (byte)spiRam.read_byte((int)ligne + 4));
       }
       address *= 5; //Chaque variable est stokée sur 5 bytes
       address += 9;
       if (address > 13 && address < 0x7FFA) { //Les première adresse sont réservé pour l'Executant et limité par le composant
          time = word((byte)spiRam.read_byte((int)address + 3), (byte)spiRam.read_byte((int)address + 4));
          String str = "A/V: " + ByteToString(rxCmd[7]) + ", M : " + ByteToString((byte)spiRam.read_byte((int)timeD)) + ", " + ByteToString((byte)spiRam.read_byte((int)time));
          //str = str + ", T : " + (char)spiRam.read_byte((int)timeD + 1) + ", " + (char)spiRam.read_byte((int)time + 1) + ", C : " + (char)spiRam.read_byte((int)timeD + 2) + ", " + (char)spiRam.read_byte((int)time + 2);
          //logFile( str, "Test G", Seria);
          //str = "addresse " + WordToString(address) + ", time " + WordToString(time) + ", ligne " + WordToString(ligne) + ", value " + WordToString(value) + ", timeD " + WordToString(timeD);
          //logFile( str, "Test G", Seria);
          //Pour les comparaison de valeurs et dans le cas d'une valeur, alors on réattribue la variable timeD
          switch(rxCmd[6]) {
             case 'E':
                if (rxCmd[7] == 'V') timeD = value;
                if (time == timeD) NumLigne = ligne - 1;
                break;
             case 'S':
                if (rxCmd[7] == 'V') timeD = value;
                if (time > timeD) NumLigne = ligne - 1;
                break;
             case 'I':
                if (rxCmd[7] == 'V') timeD = value;
                if (time < timeD) NumLigne = ligne - 1;
                break;
             case 'D':
                if (rxCmd[7] == 'V') timeD = value;
                if (time != timeD) NumLigne = ligne - 1;
                break;
             case 'M':
                if (rxCmd[7] == 'A' && spiRam.read_byte((int)address) == spiRam.read_byte((int)value)) {
                   NumLigne = ligne - 1;
                }
                break;
             case 'T':
                if (rxCmd[7] == 'A' && spiRam.read_byte((int)address + 1) == spiRam.read_byte((int)value + 1)) {
                   NumLigne = ligne - 1;
                }
                break;
             case 'C':
                if (rxCmd[7] == 'A' && spiRam.read_byte((int)address + 2) == spiRam.read_byte((int)value + 2)) {
                   NumLigne = ligne - 1;
                }
                break;
          }
       }
       break;
    case 'P' : case 'p' :
       //(Run executable)(Passe)(Condition de poursuivre sur réception de message)
       //Format : "MHHAA"
       //provenance du message, type de Cmd et Cmd du message
       break;
    case 'H' : case 'h' :
       //(Run executable)(Horaire)(Condition on Date and Time)
       //Format : "A\DHHFHHLHHHH"
       //Type (mois, jour, minute), Valeur début et fin, Si false on fait un goto
       break;
    case 'O' : case 'o' :
       //(Run executable)(Opération)(valeurs et type d'opération(P +)(M -)(D /)(X *))
       //Format : "HHHHAAHHHH"
       //Adresse ou valeur 1, type d'opération, adresse ou valeur 2
       StringToWord(address, rxCmd, 2);
       StringToWord(value, rxCmd, 8);
       //On compare normalement une valeur a une adresse mémoire par défaut : rxCmd[2] == 'V'
       address *= 5; //Chaque variable est stokée sur 5 bytes
       address += 9;
       if (address > 13 && address < 0x7FFA)
          ligne = word((byte)spiRam.read_byte((int)address + 3), (byte)spiRam.read_byte((int)address + 4));
       //On compare normalement une valeur a une adresse mémoire par défaut : rxCmd[8] == 'V'
       if (rxCmd[7] == 'A') { //On compare des valeurs entre 2 adresses mémoire
          value *= 5; //Chaque variable est stokée sur 5 bytes
          value += 9;
          if (value > 13 && value < 0x7FFA)
             value = word((byte)spiRam.read_byte((int)value + 3), (byte)spiRam.read_byte((int)value + 4));
       }
       switch(rxCmd[6]) {
          case 'P':
             ligne += value;
             break;
          case 'M':
             ligne -= value;
             break;
          case 'D':
             ligne /= value;
             break;
          case 'X':
             ligne *= value;
             break;
       }
       if (address > 13 && address < 0x7FFA) { //Les première adresse sont réservé pour l'Executant et limité par le composant
          //On recopie le résultat dans la RAM
          spiRam.write_byte((int)address + 4, ligne & 0x00FF);
          spiRam.write_byte((int)address + 3, (ligne >> 8) & 0x00FF);
       }
       break;
    default:   
      CmdError("R executable (A.V.S.D.F.G.P.H.O)");           
  } 
}

void setup(){
  //Initialise les Serial
  Serial.begin(9600) ;
  Serial2.begin(9600);

  //Initialisation de la librairie Creche
  CR.configure(12);
  CR.check(true);

  //Initialise les canaux de réception
  fCmd[Seria] = "";
  fCmd[Radio] = "";
  fCmd[Seria2] = "";
  fCmd[Executant] = "";
  
  //Initialisation de la librairie Mirf
  Mirf.cePin = 48; // CE sur D8
  Mirf.csnPin = 53; // CSN sur D7
  Mirf.spi = &MirfHardwareSpi; // On veut utiliser le port SPI hardware
  Mirf.init(); // Tout est bon ? Ok let's go !
  Mirf.channel = 25; // On va utiliser le canal 0 pour communiquer (128 canaux disponible, de 0 à 127)
  Mirf.payload = sizeof(byte); // = 4, ici il faut déclarer la taille du "payload" soit du message qu'on va transmettre, au max 32 octets
  Mirf.config(); // Tout est bon ? Ok let's go !
  Mirf.setRADDR((byte *)"nrf01"); // On définit ici l'adresse du module en question
/*  //Lecture de l'adresse d'envois dans la mémoire eeprom et application
  byte *adresse;
  adresse[3] = EEPROM.read(0);
  adresse[4] = EEPROM.read(1);
  adresse[0] = 'n';
  adresse[1] = 'r';
  adresse[2] = 'f';
  Mirf.setTADDR((uint8_t *) adresse);
*/

  //Teste la présence de la RTC sur le bus I²C, si c'est negatif, il y a une boucle infini,
  //qui va faire travailler le watchdog
  Wire.begin();
  byte mode = 3;
  Wire.beginTransmission(DS1307_ADDRESS);
  if (Wire.endTransmission() != 0) while (mode != 0) {
    txCmd = "A00M01ZNo RTC Connected";
    Send(Module);
    delay(2000);
  }

  //Initializing SD card...
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
  //pinMode(SD_SS_PIN, OUTPUT);     // change this to 49 on a mega
  //digitalWrite(SD_SS_PIN, HIGH);
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  mode = 3;
  if (!SD.begin(SD_SS_PIN)) while (mode != 0){
    txCmd = "A00M01ZNo SD card";
    Send(Module);
    txCmd = "M04M01KC01T00FA";
    Send(Module);
    delay(2000);
  }

  //Le fichier Exe_oo.cre est le point d'entrée du programme exécutable
  //pour les autres fichier executable, il faut changer le 00 par une autre combinaison
  //de chiffre ou de lettre. La casse est prise en compte.
  if (!SD.exists("Exe_00.cre")) {
    txCmd = "A00M01ZNo Exe_00.cre File";
    actif = false;
    Send(Module);
    txCmd = "M04M01KC01T00FA";
    Send(Module);
  }

  //Au démarrage on considère que le système est allumé
  LastStateAlim1240 = true;
  StateAlim1240 = true;

  //Commande demmarrage du module (attenttion réécrire le numéro du module après type démarage)
  txCmd = "A00M01D01";
  Send(Module);
  txCmd = "M04M01KT01O0";
  Send(Module);

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
  while (Serial2.available())
      TreatChar(Serial2.read(), Seria2);

  //Bloc executant le programme lumineu
  if (actif && !Serial.available() && StateAlim1240) {
    if (CR.currentMillis - ExeTime > pause && !validation) {
      pause = 50; //Temps entre 2 instructions consécutive
      //Doit etre assez long pour permettre a tout le système de suivre
      //Assez court Pour qu'il ne faille pas attendre tout le temps la lecture de l'instruction
      ExeTime = CR.currentMillis;
      /* Déclare le buffer qui stockera une ligne du fichier, ainsi que les deux pointeurs key et value */
      char buffer[BUFFER_SIZE];
      /* Déclare l'itérateur et le compteur de lignes */
      byte i, buffer_lenght;
      // Noméro de ligne
      word ligne_test;
      /* Ouvre le  fichier de configuration */
      char *Execre = "Exe_00.cre";
      Execre[4] = (IndiceFile >> 8) & 0x00FF;
      Execre[5] = IndiceFile & 0x00FF;
      ExeFile = SD.open(Execre, FILE_READ);
      if(ExeFile) { // Gère les erreurs
         /* Tant que non fin de fichier */
         while(ExeFile.available() > 0 ){
            /* Récupère une ligne entière dans le buffer */
            i = 0;
            while((buffer[i++] = ExeFile.read()) != '\n') {
               /* Si la ligne dépasse la taille du buffer */
               if(i == BUFFER_SIZE) {
                  /* On finit de lire la ligne mais sans stocker les données */
                  while(ExeFile.read() != '\n');
                  break; // Et on arrête la lecture de cette ligne
               }
            }
            /* On garde de côté le nombre de char stocké dans le buffer */
            buffer_lenght = i;
            /* Finalise la chaine de caractéres ASCIIZ en supprimant le \n au passage */
            buffer[--i] = '\0';
            /* Ignore les lignes vides ou les lignes de commentaires */
            if(buffer[0] == '\0' || buffer[0] == '#') continue;
            // On teste le numéro de ligne avec celui qui doit etre executé
            StringToWord(ligne_test, buffer, 0);
            if (ligne_test == NumLigne) {
               //Comme ont est à la bonne ligne, on execute l'instruction
               i = 4; //Les 4 premier char correspondent au numéro de la ligne en Hexa
               boolean msg = false;
               while (buffer[i] != '\0') {
                  //A la fin du msg on l'envois, meme pour les msg a destination de serveur M1
                  TreatChar(buffer[i], Executant);
                  i++;
               }
               NumLigne++;
               break;
            }
            //Quand on à dépassé la ligne qui doit etre executé, on incrémente le compteur et on quitter la boucle
            else if ((ligne_test > NumLigne)) {
               NumLigne++;
               break;
            }
         }
      }
      
      ExeFile.close();
      //Toutes les variables sont recopiée dans la RAM pour sauvegarde
      spiRam.write_byte(0x0001, pause & 0x00FF);
      spiRam.write_byte(0x0000, (pause >> 8) & 0x00FF);
      int ValBool = (actif)?0x0001:0x0000;
      ValBool = ValBool << 8;
      ValBool = ValBool | (validation)?0x0001:0x0000;
      ValBool = ValBool << 8;
      ValBool = ValBool | (synchronisation)?0x0001:0x0000;
      ValBool = ValBool << 8;
      spiRam.write_byte(0x0003, ValBool & 0x00FF);
      spiRam.write_byte(0x0005, NumLigne & 0x00FF);
      spiRam.write_byte(0x0007, IndiceFile & 0x00FF);
      spiRam.write_byte(0x0002, (ValBool >> 8) & 0x00FF);
      spiRam.write_byte(0x0004, (NumLigne >> 8) & 0x00FF);
      spiRam.write_byte(0x0006, (IndiceFile >> 8) & 0x00FF);
    }
  }

}

