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
 ************LIBRAIRIE POUR L'ECLAIRAGE DE LA CRECHE DE SAINT JOSEPH,************
 ********************************************************************************
 *                            SUR ARDUINO UNO et MEGA,                          *
 *     A UTILISER UNIQUEMENT AVEC LES PROTOSHIELDS ADAPTES A CE PROGRAMME       *
 *                                                                              *
 *          FONCTION:                                                           *
 *  -Watchdog (timer 555), reset après 9s de silence                            *
 *  -Gestion Bouton poussoir avec Led témoin                                    *
 *  -Protocole de communication repris du code de monsieur Guy Tessier          *
 * avec l'ajout de l'adresse du destinataire et de l'envoyeur dans le message   *
 *  -Fonction de conversion entre chaine de carractère en Hexadécimal vers      *
 * une variable et inversément (byte<->String et int<->String)                  *
 *                                                                              *
 ********************************************************************************
 *                                                                              *
 *          FONCTION:                                                           *
 *                                                                              *
 ********************************************************************************
 *             copyright       REALISE PAR E.CROONEN,                           *
 ********************************************************************************
 */

#include "Arduino.h"

#ifndef CRECHE_H
#define CRECHE_H

/*
********************************************************************************
*        CRECHE : gestion du Watchdog, currentmillis
********************************************************************************
*/

class CRECHE
{

   public:
      CRECHE ();
      void check(bool mirfOk);
      unsigned long currentMillis;
      void configure(byte pin);

   private:
      //Fonstion de reset du WatchDog
      void ResetWatchdog();
      unsigned long WatchdogMillis; //Heure de la dernière levée du chien de garde 
      byte PinWatchdog;  //Pin du watchdog
      int WatchdogInter; //interval entre deux levée de patte du chien de garde
      boolean conf; //Booléen qui indique si l'instance est configurée
};

extern CRECHE CR;

/*
********************************************************************************
*    BOUTON : gestion d'un bouton avec anti-rebont, bascule et led témoin
********************************************************************************
*/

class BOUTON {

   public:
      BOUTON(byte pinb, byte pinLed);
      BOUTON(byte pinb);
      void check(unsigned long *currentMillis);
      boolean state;             // bistable
      boolean FrontB;       // detection d'un front pour le bouton
      boolean FrontState;    // Front sur un state

   private:
      byte LedPin;               //Pin led temoin
      byte ButtonPin;            //Pin bouton
      boolean buttonState;       // the current reading from the input pin
      boolean lastButtonState;   // the previous reading from the input pin
      boolean reading;           // état du du bouton
      boolean led;               // présence led témoin
      long lastDebounceTime;     // the last time the output pin was toggled
      long debounceDelay;        // the debounce time; increase if the output flickers
};

/*
 ********************************************************************************
 * La strutucture suivante "Date" permet de manipuler facilement les données de date / heure
 * Pour faire simple une structure est une sorte de "super variable" pouvant contenir d'autres variables de type différents.
 * Son principe de fonctionnement est cependant totalement différent d'un tableau, pour accéder à un membre d'une structure il faut utiliser .
 * Exemple : Date date;
 *           date.mois = 1;
 *
 * Attention ! Lorsque la structure est passé en argument via un pointeur, ou qu'on accède à celle ci via un pointeur il faut utiliser ->
 * Exemple : Date *date, date2;
 *           date = &date2; // le pointeur date (*date) pointe désormais sur l'adresse de date2 (&date2)
 *           date->mois = 1; // correspond à date2.mois = 1;
 * /!\ Quand une strucuture ou une variable est passé en référence via un pointeur à une fonction c'est la variable pointé qui est modifié ET NON UNE COPIE de la variable comme c'est le cas dans un passage d'arguments normal. 
 ********************************************************************************
 */

typedef struct {
  uint8_t secondes;
  uint8_t minutes;
  uint8_t heures; // format 24h
  uint8_t jourDeLaSemaine; // 1~7 => lundi, mardi, ...
  uint8_t jour;
  uint8_t mois;
  uint8_t annee; // format yy (ex 2012 -> 12)
} Date;

/*
 ********************************************************************************
 * Structure avec les donnée de variation de base uniquement pour le MODULE 2
 ********************************************************************************
 */

typedef struct{
  unsigned long pause;
  unsigned int ledDemIni[6];
  unsigned int ledDemFin[6];
  bool Ini;
} Etat;

/*
********************************************************************************
*   Conversion: varible -> ecriture hexa et l'inverse
********************************************************************************
*/

//Liste des modules du système (pour gérer l'envois et la réception)
enum ArduinoModel{ Pwm_m2 = 2, Serveur_m1 = 1, Pc_m0 = 0, Dbc_m3 = 3, BoutonServeur_m4 = 4, Lanterne_m5 = 5};
//Liste de canaux de communication
enum Mode{Seria = 0, Radio = 1, Seria2 = 2, Module = 4, Executant = 3};

// Binaire à String
char NibbleToChar(byte data) ;
String ByteToString(byte data) ;
String WordToString(word data) ;
//String à Binaire
char CharVal(char car) ;
boolean StringToWord(word& res, String stg, byte fromIndex, byte digits=4) ;
boolean StringToByte(byte& res, String stg, byte fromIndex) ;

/*
 ********************************************************************************
 *   freeMemory() : Calcul de l'espace de RAM disponible
 * MemoryFree library based on code posted here:
 * http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1213583720/15
 * Extended by Matthew Murdoch to include walking of the free list.
 ********************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif
int freeMemory();
#ifdef  __cplusplus
}
#endif

#endif /* CRECHE_H */
