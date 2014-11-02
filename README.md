 *
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
 **************Principe général de fonction du système d'éclairage,**************
 ********************************************************************************

   Ce texte sert a fixer le fonctionnement des jeux d'éclairage depuis le soft jusqu'aux signaux, générés par le microcontroleur, qui pilote les jeux de lumière.

 ***********Partie soft ou programmation à l'aide d'un IDE spécifique,***********

Le soft permet de créer des effets lumineux aussi varié que possible. Son fonctionnement est le suivant.

-L'utilisateur créer un mode lumineux
-Il en précise les heures de fonctionnement
-L'utilisateur créer dans ce mode des éclairages. (1 à la fois)
-Il définit les paramètres constant de cet éclairage
 qui sont valable dans tous les modes:
  * Un  Nom et une description succinte
  * Un adresse composé de 2 chiffres pour le module et 2 chiffre pour le numéro
    de jeu lumineux i.e.: "02" et "01" pour Module 2 LED 1.
  * Un type d'éclairage listé ici:
    -TOR
    -Clignotant direct
    -Progressif (PWM)
    -Vacillant (PWM)
    -TOR avec Luminosité réglable (PWM)
  * La caractéristique "PWM" précisée juste avant. Vérifié avec la base de donnée si l'adresse sélectionnée autorise cette carractéristique
-Idépendement d'un éclairage il choisit la durée de la séquence lumineuse
-Il crée des "states" ou "key" répartie sur la ligne du temps, à chaque fois que
 cet éclairage doit changer d'état. Il n'est pas nécessaire que le key d'un éclairage
 se répercute sur tous les autres
-Pour chaque key nous avons:
  * Un paramètre lié au type déclairage (type byte ou char)
    -TOR                          | 1 ou 0
    -Clignotant direct            | 1 ou 0
    -Progressif                   | valeur finale de la variation (00->255)
    -Vacillant                    | 1 ou 0
    -TOR avec Luminosité réglable | Valeur (00->255)
  * Une description. (vide si l'utilisateur n'en met pas)

 ***************Partie du soft, compilation des effets lumineux,****************

   Le programme exécuté par le module arduino est enregistrer sous un format texte. Il est composé d'un fichier d'initialisation du système avec des liens vers les fichiers contenant les modes de fonctionnement.
   1 seul fichier d'initialisation avec nom imposé, et les conditions horaires de tous les modes
   1 fichier pour chaque mode (5 max)
   Un mode peut être testé directement sur le système a l'aide de la connection SerialUSB. A ce moment c'est l'ordinateur qui devient serveur du système.
   Chaque key d'un éclairage est traduit par un message pour l'adresse enregistrée avec le paramètre ascociée a cette key. Donc un key ne s'applique pas à l'ensemble du système.
   Consistance d'un message:
      * Module destinataire
      * Module envoyeur
      * Type de commande "Ecriture"
      * Paramètre de la commande
   i.e.: <M02M00EL011> Le PC pour le module 2 Ecriture, Led 1, Allumez

 ***************Partie du soft, compilation des effets lumineux,****************


