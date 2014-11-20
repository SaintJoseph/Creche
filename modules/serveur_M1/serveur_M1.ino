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
 *          -3 LED jaune                                                        *
 *          -3 Boutons poussoir                                                 *
 *          -1 LED rouge témoin des communication radio                         *
 *          -1 Bornier 4 plots (en parallèle avec les boutons poussoir)         *
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

PROGMEM const String devise = "Mon Dieu faite que je serve uniquement a votre plus grande gloire";

//---------------------------VARIABLES--------------------------------

//Variable qui indique si le système est allumé ou éteint
boolean StateAlim1240;
boolean LastStateAlim1240;

//Variables pour les échanges de données structurées par sériale
String rxCmd ; // Commande reçue (sans délimiteurs)
String txCmd ; // Commande à transmettre

String fCmd[3]; //Message reçu par les cannaux de communication

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
int pause = 0; //Temps d'attente avant la prochaine action
unsigned long ExeTime = 0; //Heure de l'action
boolean actif = false, validation = false, synchronisation = false;

//Donnée pour le fichier de config
#define BUFFER_SIZE = 32;
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
    logFile(fCmd[canal]);
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
           TreatCommand(fCmd[canal].substring(6)) ;
        }
        else {
            //Si le message n'est pas a destination de ce module, c'est qu'il est attendu sur un autre module, donc on le fait suivre
            txCmd = fCmd[canal];
            Send(canal);
        }
        break ;
      case 'A' : case 'a' :
           txCmd = fCmd[canal][3] + ByteToString(envoyeur) + "M01";
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
        txCmd = "ErrMessageM1";
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
    logFile(txCmd);
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
    case 'R' : case 'r' : 
      //Message reçu : <(RUN exécutable)...>
      modifExecutable() ;
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

//Fonction qui permet l'ecriture de certaine variable
void EcritureVariable () {
  Date date;
  word address = 0;
  byte wValue0, wValue1;
  switch (rxCmd[1])
  {
    case 'D' : case 'd' :
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
    case 'R' : case 'r' :
       //Message reçu: <(Ecriture)(RAM)(adresse)(valeur)>
       //Format "HHHHVHHHH"
       StringToWord(address, rxCmd, 2);
       StringToByte(wValue0, rxCmd, 7);
       StringToByte(wValue1, rxCmd, 9);
       spiRam.write_byte((int)address, wValue0);
       spiRam.write_byte((int)address + 1, wValue1);
       break;
    default:   
      CmdError("E Type d'ecriture (D.R)");
  }
}

//Fonction qui renvoit la valeur des variables demandée
void LectureVariable() {
  Date date;
  // set up variables using the SD utility library functions:
  Sd2Card card;
  SdVolume volume;
  word address = 0;
  byte wValue0, wValue1;
  switch (rxCmd[1])
  {
    case 'M' : case 'm' :
       //Message reçu: <(Lecture)(Memoire)>
       //Message envoyé: <(Lecture)(Memoire)(memoire..)>
       txCmd = txCmd + "LM" + WordToString(freeMemory());
       Send(Module);
       break ;
    case 'U' : case 'u' :
       //Message reçu: <(Lecture)(battrie)>
       //Message envoyé: <(Lecture)(battrie)(tension battrie..)>
       txCmd = txCmd + "LU" + WordToString(Tbatterie());
       Send(Module);
       break ;
    case 'D' : case 'd' :
       //Message reçu: <(Lecture)(Date)>
       //Message envoyé: <(Lecture)(Date)(jj/mm/aaRjjHhh:mm:ss)>
       lire(&date);
       txCmd = txCmd + "LD" + ByteToString(date.jour) + "/" + ByteToString(date.mois) + "/" + ByteToString(date.annee) + "R" + ByteToString(date.jourDeLaSemaine) + "H" + ByteToString(date.heures) + ":" + ByteToString(date.minutes) + ":" + ByteToString(date.secondes);
       Send(Module);
       break ;
    case 'O' : case 'o' :
       //Message reçu: <(Lecture)(On/Off)>
       //Message envoyé: <(Lecture)(On/Off)(StateAlim1240..)>
       txCmd = txCmd + "LO";
       if (StateAlim1240) txCmd = txCmd + "1";
       else txCmd = txCmd + "0";
       Send(Module);
       break;
    case 'R' : case 'r' :
       //Message reçu: <(Lecture)(RAM)(adresse)>
       //Message envoyé: <(Lecture)(Ram)(valeur)>
       //Format "HHHH"
       //Format "HHHHVHHHH"
       StringToWord(address, rxCmd, 2);
       wValue0 = (byte)spiRam.read_byte((int)address);
       wValue1 = (byte)spiRam.read_byte((int)address + 1);
       txCmd = txCmd + "LR" + WordToString(address) + "V" + ByteToString(wValue0) + ByteToString(wValue1);
       Send(Module);
       break;
    case 'S' : case 's' :
       //Message reçu: <(Lecture)(SD)>
       //Message envoyé: <(Lecture)(SD)(type)(espace mémoire)>
       //Format ""
       //Format "NNNNVHHHH"
       txCmd = txCmd + "LS";
       if (!card.init(SPI_HALF_SPEED, SD_SS_PIN)) {
       switch(card.type()) {
          case SD_CARD_TYPE_SD1:
             txCmd = txCmd + " SD1";
             break;
          case SD_CARD_TYPE_SD2:
             txCmd = txCmd + " SD2";
             break;
          case SD_CARD_TYPE_SDHC:
             txCmd = txCmd + "SDHC";
             break;
          default:
             txCmd = txCmd + "Unkw";
       }
       txCmd = txCmd + "V";
       // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
       if (!volume.init(card)) {
          txCmd = txCmd + "0000";
          Send(Module);
          break;
       }
       // print the type and size of the first FAT-type volume
       uint32_t volumesize;
       volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
       volumesize *= volume.clusterCount();       // we'll have a lot of clusters
       volumesize /= 2; // SD card blocks are always 512 bytes / 1024 => kb
       volumesize /= 1024; // => Mb
       txCmd = txCmd + WordToString((word)volumesize);
       Send(Module);
       break;
       }
       else {
          CmdError("LS SD ini faild");
          break;
       }
    case 'A' : case 'a' :
       //Message reçu: <(Lecture)(Actif)>
       //Message envoyé: <(Lecture)(Actif)(On/Off)>
       //Format ""
       //Format "B"
       break;
    default:   
      CmdError("L Type de lecture (M.U.D.O.S.R.A)");
  } 
}

//Fonction qui réagit a des commandes spéciales
void DonneSpeciaux() {
  byte led;
  switch (rxCmd[1])
  {
    case 'R' : case 'r' :
       //Message reçu: <(X opération)(Reboot)>
       while ( rxCmd[1] < 't');
    case 'B' : case 'b' :
       //Message reçu: <(X opération)(Boutton)>
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
       txCmd = txCmd + "P01";
       Send(Module);
       break;
    default:   
      CmdError("X action speciale (R.B.P)");           
  } 
}


//Ecriture d'un byte dans la mémoire eeprom a l'adresse indiquée
boolean eepromWrite( unsigned int *eeaddress, byte data, int deviceadd)
{
  Wire.beginTransmission(deviceadd);
  Wire.write((int)(*eeaddress >> 8));    // Address High Byte
  Wire.write((int)(*eeaddress & 0xFF));  // Address Low Byte
  Wire.write(data);
  if(Wire.endTransmission() == 0) return true;
  else return false;
}

//Lecture d'un byte dans la mémoire eeprom a l'adresse indiquée
boolean eepromRead( unsigned int *eeaddress, byte *rdata, int deviceadd)
{
  Wire.beginTransmission(deviceadd);
  Wire.write((int)(*eeaddress >> 8));    // Address High Byte
  Wire.write((int)(*eeaddress & 0xFF));  // Address Low Byte
  Wire.endTransmission();
  Wire.requestFrom(deviceadd,1);
  //On attend ques des données se présentent sur le bus I²C
  //après 30 ms si rien ne s'est présenté c'est qu'il y a un défaut et on retourne un false
  byte i = 0;
  while (!Wire.available() && i++ != 30) delay(1);
  *rdata = Wire.read();
  if(i == 30) return true;
  else return false;
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
void writeram(byte *states, byte *mode, byte *priorite) {
  Wire.beginTransmission(DS1307_ADDRESS);
  //ecriture a partir de la position 0x08
  Wire.write(0x08);
  //ecriture du states (position dans la séquence  d'éclairage)
  Wire.write(*states);
  //ecriture du mode (normal J/N, messe (sans effet), séquence1, autres)
  Wire.write(*mode);
  //ecriture de la priorité du mode dans la ram
  Wire.write(*priorite);
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
    const int potar = 0; // le potentiomètre, branché sur la broche analogique 0
    int valeurLue; //variable pour stocker la valeur lue après conversion
    float tension; //on convertit cette valeur en une tension
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
void logFile(String mesg) {
   File dataFile = SD.open("datalog.txt", FILE_WRITE);
   Date date;
   lire(&date);
   // if the file is available, write to it:
   if (dataFile) {
      dataFile.print(ByteToString(date.jour) + "/" + ByteToString(date.mois) + "/" + ByteToString(date.annee) + "R" + ByteToString(date.jourDeLaSemaine) + "H" + ByteToString(date.heures) + ":" + ByteToString(date.minutes) + ":" + ByteToString(date.secondes) + "\t");
      dataFile.println(mesg);
      dataFile.close();
   }  
}

//Fonction qui réagit au commande pour modifier le bloc executable
void modifExecutable () {
  word time;
  switch (rxCmd[1])
  {
    case 'A' : case 'a' :
       //(Run executable)(Actif)(On/Off)
       //Format : "B"
       switch (rxCmd[2])
       {
          case '1' :
             actif = true;
             //On allume la Led témoin rouge de M4
             txCmd = "M04M01KR01T03E8";
             Send(Module);
             break;
          case '0' :
             actif = false;
             txCmd = "M04M01KT01O0";
             Send(Module);
             break;
       }
       break;
    case 'V' : case 'v' :
       //(Run executable)(Validation)
       //Format : ""
       validation = false;
       break;
    case 'S' : case 's' :
       //(Run executable)(Synchronisation)
       //Format : ""
       synchronisation = false;
       break;
    case 'D' : case 'd' :
       //(Run executable)(Delais)(delais)
       //Format : "HHHH"
       StringToWord(time, rxCmd, 2);
       pause = (int)time;
       break;
    default:   
      CmdError("R Run executable (A.V.S.D)");           
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
  
  //Initialisation de la librairie Mirf
  Mirf.cePin = 48; // CE sur D8
  Mirf.csnPin = 53; // CSN sur D7
  Mirf.spi = &MirfHardwareSpi; // On veut utiliser le port SPI hardware
  Mirf.init(); // Tout est bon ? Ok let's go !
  Mirf.channel = 25; // On va utiliser le canal 0 pour communiquer (128 canaux disponible, de 0 à 127)
  Mirf.payload = sizeof(byte); // = 4, ici il faut déclarer la taille du "payload" soit du message qu'on va transmettre, au max 32 octets
  Mirf.config(); // Tout est bon ? Ok let's go !
  Mirf.setRADDR((byte *)"nrf01"); // On définit ici l'adresse du module en question

  //Commande demmarrage du module (attenttion réécrire le numéro du module après type démarage)
  txCmd = "A00M01D01";
  Send(Module);

  //Teste la présence de la RTC sur le bus I²C, si c'est negatif, il y a une boucle infini,
  //qui va faire travailler le watchdog
  Wire.begin();
  byte mode = 3;
  Wire.beginTransmission(DS1307_ADDRESS);
  if (Wire.endTransmission() != 0) while (mode != 0) {
    txCmd = "A00M01No RTC Connected";
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
    txCmd = "A00M01No SD card";
    Send(Module);
    txCmd = "M04M01KC01T00FA";
    Send(Module);
    delay(2000);
  }


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
  while (Serial2.available())
      TreatChar(Serial2.read(), Seria2);

  //Bloc executant le programme lumineu
  if (actif) {
    if (CR.currentMillis - ExeTime > pause && !validation) {
      pause = 1;
    }
  }
  
}

