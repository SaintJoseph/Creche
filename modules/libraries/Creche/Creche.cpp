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
 *                                SUR ARDUINO UNO,                              *
 *       A UTILISER UNIQUEMENT AVEC LA PROTOSHIELD ADAPTE A CE PROGRAMME        *
 *                                                                              *
 *          FONCTION:                                                           *
 *  -Watchdog (timer 555), reset après 9s de silence                            *
 *  -Gestion Bouton poussoir avec Led témoin                                    *
 *  -Protocole de communication entre les modules arduino                       *
 *  -Calcul de l'espace de RAM disponible
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
 */

#include "Creche.h"

/*
********************************************************************************
*        CRECHE : gestion du Watchdog, currentmillis
********************************************************************************
*/

CRECHE::CRECHE ()
{
   //Initialisation du watchdog
   WatchdogMillis = millis();
   WatchdogInter = 500;
   PinWatchdog = 13;
}

//Condition d'appel de la fonction de levée de patte du chien de garde
void CRECHE::check(bool mirfOK)
{
   currentMillis = millis();
   if (currentMillis - WatchdogMillis > WatchdogInter && conf && mirfOK){
      WatchdogMillis = currentMillis;
      ResetWatchdog();
   }
}

void CRECHE::configure(byte pin)
{
   //Pin du watchdog
   PinWatchdog = pin;
   conf = true;
   ResetWatchdog();
}

//Fonstion de reset du WatchDog
void CRECHE::ResetWatchdog()
{
    pinMode(PinWatchdog, OUTPUT); //The PinWatchdog line goes Low
    delay(5);
    //Décharge de la capacité suffisante pour une durée suppérieur à 1,5s (durée de l'interval
    //après 4.5s le condensateur est assez déchargé pour tenir 9s
    pinMode(PinWatchdog, INPUT);  //The PinWatchdog line goes Hi-Z
}

CRECHE CR = CRECHE(); //Création d'une instance crêche (unique par execution)

/*
********************************************************************************
*    BOUTON : gestion d'un bouton avec anti-rebont, bascule et led témoin
********************************************************************************
*/

BOUTON::BOUTON(byte pinb, byte pinLed)
{
   ButtonPin = pinb;
   led = true;
   LedPin = pinLed;
   state = false;
   FrontB = false;
   pinMode(ButtonPin, INPUT);
   pinMode(LedPin,OUTPUT);
   FrontState = false;

}

BOUTON::BOUTON(byte pinb)
{
   ButtonPin = pinb;
   led = false;
   LedPin = 0;
   state = false;
   FrontB = false;
   pinMode(ButtonPin, INPUT);
   FrontState = false;
}

void BOUTON::check(unsigned long *currentMillis)
{
   // read the state of the switch into a local variable:
   reading = digitalRead(ButtonPin);
   //Gestion du Boutton avec boucle anti-rebond
   //ecriture pour la détection d'un front
   FrontB = buttonState;
   // check to see if you just pressed the button 
   // (i.e. the input went from LOW to HIGH),  and you've waited 
   // long enough since the last press to ignore any noise:  
   // If the switch changed, due to noise or pressing:
   if (reading != lastButtonState) {
      // reset the debouncing timer
      lastDebounceTime = *currentMillis;
   }
   // inserer le delay pour la boucle anti-rebond
   if ((*currentMillis - lastDebounceTime) > 50) {
      // whatever the reading is at, it's been there for longer
      // than the debounce delay, so take it as the actual current state:
      buttonState = reading;
   }
   // save the reading.  Next time through the loop,
   // it'll be the lastButtonState:
   lastButtonState = reading;
   //A chaque appuye sur un des boutton on bascule l'état des LED correspondante
   if (buttonState != FrontB && buttonState == HIGH){ //front montant
      state = 1 - state;
      FrontState = true; //FrontState passe a true uniquement pendant 1 tour quand state change de valeur
   }
   else FrontState = false;

   //Allumage des LED en fonction des state
   if (led){
      digitalWrite(LedPin, state);
   }
}

/*
********************************************************************************
*   ComProtRadio : Protocole de communication entre les modules
********************************************************************************
*/

char NibbleToChar(byte data)
// Retourne le chiffre hex. (en caractère) des 4 bits les moins significatifs de "data"
{
  data = data & 0x0F ;
  if (data < 10)
    data = '0' + data ;
  else
    data = 'A' + (data-10) ;
  return data ;
}

String ByteToString(byte data)
// Retourne la valeur 8 bits en hex
{
  String res ;

  res = "xx" ;
  res[0] = NibbleToChar(data >> 4)  ;
  res[1] = NibbleToChar(data)  ;
  return res ;
}

String WordToString(word data)
// Retourne la valeur 16 bits en hex
{
  return ByteToString(data >> 8) +ByteToString(data)  ;
}

char CharVal(char car)
// Retourne la valeur binaire du car. hex (retourne -1 si erreur)
{
  if (car >= '0' && car < '9')
    car = car - '0' ;
  else
    if (car >= 'A' && car <= 'F')
      car = car - 'A' + 10 ;
    else
      if (car >= 'a' && car <= 'f')
      car = car - 'a' + 10 ;
    else
      car = -1 ; // 0xFF
  return car ;
}

boolean StringToWord(word& res, String stg, byte fromIndex, byte digits)
// Retourne "vrai" ssi la chaine "stg" contient un nombre hex légal commençant à "fromIndex"
// avec "digits" chiffres (valeur par défaut: 4)
// Si OK, "res" va contenir la valeur binaire de lachaine
{
  word tmp ;
  char digit ;
  boolean ok ;

  tmp = 0 ;
  ok = (fromIndex + digits <= stg.length()) ;
  while (digits && ok)
  {
    digit = CharVal(stg[fromIndex]) ;
    ok = (digit != -1) ;
    if (ok)
     {
      tmp = (tmp << 4) + digit ;
      fromIndex++ ;
      digits-- ;
    }
  }
  if (ok)
    res = tmp ;
  return ok ;
}

boolean StringToByte(byte& res, String stg, byte fromIndex)
{
   bool ok ;
   word tmp ;

   ok = StringToWord(tmp,stg,fromIndex,2) ;
   if (ok)
     res = tmp ;
   return ok ;
}

/*
********************************************************************************
*   freeMemory() : Calcul de l'espace de RAM disponible
********************************************************************************
*/


extern unsigned int __heap_start;
extern void *__brkval;

/*
 * The free list structure as maintained by the
 * avr-libc memory allocation routines.
 */
struct __freelist {
  size_t sz;
  struct __freelist *nx;
};

/* The head of the free list structure */
extern struct __freelist *__flp;

/* Calculates the size of the free list */
int freeListSize() {
  struct __freelist* current;
  int total = 0;
  for (current = __flp; current; current = current->nx) {
    total += 2; /* Add two bytes for the memory block's header  */
    total += (int) current->sz;
  }
  return total;
}

int freeMemory() {
  int free_memory;
  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__heap_start);
  } else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
    free_memory += freeListSize();
  }
  return free_memory;
}

