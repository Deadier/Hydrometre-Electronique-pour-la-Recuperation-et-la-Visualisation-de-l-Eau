# Hydromètre Électronique pour la Récupération et la Visualisation de l'Eau (HERVE)

Ce projet consiste à créer un système de mesure du niveau d'eau dans une cuve à l'aide d'un Arduino Nano, d'un capteur à ultrasons JSN-SR04T et d'un écran OLED I2C. Le système sera alimenté par une batterie ou un panneau solaire et comprendra des fonctionnalités pour économiser l'énergie et prolonger la durée de vie de la batterie.

## Cuve de récupération d'eau de pluie Ecociter ETY05000

Le système HERVE est conçu pour fonctionner avec la cuve d'eau de pluie Ecociter ETY05000. Vous pouvez trouver la [fiche technique de la cuve sur le site d'Ecociter](https://www.plasteau.com/wp-content/uploads/2015/07/images_fiches_techniques_pdf_Jardin_Eco_citer_ETY05000.pdf). mais aussi sur [ce dépôt](https://github.com/Deadier/Hydrometre-Electronique-pour-la-Recuperation-et-la-Visualisation-de-l-Eau/blob/main/Fiches_techniques_pdf_Jardin_Eco_citer_ETY05000.pdf).

### Forme et Dimensions de la Cuve

-   **Forme :** Sphère tronquée (forme intérieure identique à la forme extérieure)
-   **Diamètre extérieur :** 2400 mm
-   **Rayon extérieur :** 1200 mm
-   **Hauteur totale de la sphère tronquée :** 1520 mm
-   **Hauteur du segment tronqué :** 880 mm
-   **Rayon du cercle à la base du segment tronqué extérieur :** Environ 678 mm
-   **Épaisseur de la paroi :** 30 mm (avec marge de sécurité)
-   **Rayon intérieur :** 1170 mm (rayon extérieur moins l'épaisseur de la paroi)
-   **Rayon du cercle à la base du segment tronqué intérieur :** Environ 660 mm
-   **Volume intérieur max :** 5000 L
-   **Matériau :** Polyéthylène


### Calcul du Volume d'Eau en Fonction de la Hauteur de l'Eau
Le volume d'eau dans la cuve est calculé en utilisant deux formules différentes en fonction de la hauteur de l'eau :

1. **Si la hauteur de l'eau est inférieure à la hauteur du segment tronqué (h ≤ 880 mm)** :
   ![Equation 1](https://github.com/Deadier/Hydrometre-Electronique-pour-la-Recuperation-et-la-Visualisation-de-l-Eau/blob/main/CodeCogsEqn1.png)

2. **Si la hauteur de l'eau est supérieure à la hauteur du segment tronqué (880 mm < h ≤ 1520 mm)** :
   ![Equation 2](https://github.com/Deadier/Hydrometre-Electronique-pour-la-Recuperation-et-la-Visualisation-de-l-Eau/blob/main/CodeCogsEqn2.png)

où `R_intérieur` est le rayon intérieur de la cuve (1170 mm), et `h` est la hauteur de l'eau dans la cuve.


## Matériel nécessaire

1. [Arduino Nano](https://fr.aliexpress.com/item/1005005857558580.html)
2. [Capteur à ultrasons JSN-SR04T](https://fr.aliexpress.com/item/32665460264.html)
3. [Écran OLED I2C](https://fr.aliexpress.com/item/1005005639243863.html)
4. [Boutons poussoirs](https://fr.aliexpress.com/item/1005004066257419.html)
5. [Module d'horloge en temps réel DS3231](https://fr.aliexpress.com/item/32822420722.html)
6. [Batterie](https://fr.aliexpress.com/item/1005004524573100.html) ou [panneau solaire](https://fr.aliexpress.com/item/1005005373553931.html)
7. [Câbles de connexion](https://fr.aliexpress.com/item/1005004611997111.html), résistances, breadboard ou PCB, boîtier

## Bibliothèques nécessaires

Pour ce projet, vous aurez besoin des bibliothèques suivantes :

1.  [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306) : pour l'écran OLED I2C.
2.  [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library) : pour dessiner des formes et du texte sur l'écran OLED.
3.  [RTClib](https://github.com/adafruit/RTClib) : pour le module d'horloge en temps réel DS3231.
4.  [NewPing](https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home) : pour le capteur à ultrasons JSN-SR04T.

Ces bibliothèques peuvent être installées via le gestionnaire de bibliothèques de l'IDE Arduino. Allez dans `Croquis > Inclure une bibliothèque > Gérer les bibliothèques...` et recherchez les noms des bibliothèques. Cliquez sur `Installer` pour chaque bibliothèque.

## Fonctionnalités

1. Mesure du niveau d'eau
2. Affichage des mesures précédentes
3. Réglage de l'heure et de la date
4. Mesure immédiate du niveau d'eau
5. Économie d'énergie

## Câblage

Voici comment vous pouvez connecter vos composants à l'Arduino Nano :

1. Capteur à ultrasons JSN-SR04T :
    - Broche VCC : Connectez-la à la broche 5V de l'Arduino Nano.
    - Broche GND : Connectez-la à une broche GND de l'Arduino Nano.
    - Broche Trig : Connectez-la à la broche numérique D3 de l'Arduino Nano.
    - Broche Echo : Connectez-la à la broche numérique D2 de l'Arduino Nano.
2. Écran OLED I2C :
    - Broche VCC : Connectez-la à la broche 5V de l'Arduino Nano.
    - Broche GND : Connectez-la à une broche GND de l'Arduino Nano.
    - Broche SDA : Connectez-la à la broche A4 (SDA) de l'Arduino Nano.
    - Broche SCL : Connectez-la à la broche A5 (SCL) de l'Arduino Nano.
3. Module d'horloge en temps réel DS3231 :
    - Broche VCC : Connectez-la à la broche 5V de l'Arduino Nano.
    - Broche GND : Connectez-la à une broche GND de l'Arduino Nano.
    - Broche SDA : Connectez-la à la broche A4 (SDA) de l'Arduino Nano.
    - Broche SCL : Connectez-la à la broche A5 (SCL) de l'Arduino Nano.
4. Boutons poussoirs :
    - Bouton 1 : Connectez une broche du bouton à la broche numérique D4 de l'Arduino Nano, et l'autre broche à GND.
    - Bouton 2 : Connectez une broche du bouton à la broche numérique D5 de l'Arduino Nano, et l'autre broche à GND.
    - Bouton 3 : Connectez une broche du bouton à la broche numérique D6 de l'Arduino Nano, et l'autre broche à GND.

## Installation du code Arduino

1. Téléchargez le code source depuis ce dépôt.
2. Ouvrez le fichier `HERVE.ino` dans l'IDE Arduino.
3. Connectez votre Arduino Nano à votre ordinateur via USB.
4. Dans l'IDE Arduino, sélectionnez le bon port COM pour votre Arduino Nano.
5. Téléchargez le code sur votre Arduino Nano.

## Utilisation

Le système dispose de trois boutons qui fonctionnent de la manière suivante :

1. Bouton 1 (Réglage de l'heure et de la date) : Un appui long (10 secondes) sur ce bouton active le mode de réglage de l'heure et de la date. En mode de réglage, l'heure ou la date clignote sur l'écran. Un appui bref sur le bouton 1 incrémente l'heure ou la date. Si le bouton n'est pas pressé pendant 10 secondes, le système passe au réglage suivant (heure, minute, jour, mois, année).
2. Bouton 2 (Changement d'affichage) : Un appui bref sur ce bouton fait défiler trois affichages différents sur l'écran OLED : le volume d'eau restant dans la cuve, une liste des 10 dernières mesures de volume avec leur date et heure, et la date et l'heure actuelles du système.
3. Bouton 3 (Mesure immédiate du niveau d'eau) : Un appui long sur ce bouton déclenche une mesure immédiate du niveau d'eau. Le volume d'eau mesuré est affiché en litres sur l'écran OLED.

## Licence

Ce projet est sous licence MIT - voir le fichier `LICENSE.md` pour plus de détails.
