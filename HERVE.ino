#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DS3231.h>
#include <NewPing.h>

#define SCREEN_WIDTH 128 // Largeur de l'écran OLED en pixels
#define SCREEN_HEIGHT 64 // Hauteur de l'écran OLED en pixels
#define OLED_RESET     -1 // Numéro de la broche de réinitialisation (ou -1 si elle partage la broche de réinitialisation de l'Arduino)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DS3231 myRTC;

#define TRIGGER_PIN  3  // Broche Arduino connectée à la broche de déclenchement du capteur à ultrasons
#define ECHO_PIN     2  // Broche Arduino connectée à la broche d'écho du capteur à ultrasons
#define MAX_DISTANCE 200 // Distance maximale que nous voulons mesurer (en centimètres). La distance maximale du capteur est évaluée à 400-500cm

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // Configuration de NewPing avec les broches et la distance maximale

#define BUTTON1_PIN 4 // Broche Arduino connectée au bouton de réglage de l'heure et de la date
#define BUTTON2_PIN 5 // Broche Arduino connectée au bouton de changement d'affichage
#define BUTTON3_PIN 6 // Broche Arduino connectée au bouton de mesure immédiate du niveau d'eau

enum Mode {MODE_NORMAL, MODE_SET_HOUR, MODE_SET_MINUTE, MODE_SET_DAY, MODE_SET_MONTH, MODE_SET_YEAR};
Mode mode = MODE_NORMAL;

unsigned long lastButtonPressTime = 0;
int setHour = 0, setMinute = 0, setDay = 1, setMonth = 1, setYear = 2013;

unsigned long lastMeasurementTime = 0;
#define MEASUREMENT_INTERVAL 12 * 60 * 60 * 1000 // 12 heures en millisecondes

struct Measurement {
  int distance;
  int hour;
  int minute;
  int day;
  int month;
  int year;
};

Measurement measurements[10];

enum DisplayMode {DISPLAY_VOLUME, DISPLAY_HISTORY, DISPLAY_TIME};
DisplayMode displayMode = DISPLAY_VOLUME;

void setup() {
  Serial.begin (9600); // Commencer la communication série à 9600 bauds
  Wire.begin(); // Commencer la communication avec le module d'horloge en temps réel
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialiser l'écran OLED avec l'adresse I2C 0x3C (pour le 128x64)

  pinMode(BUTTON1_PIN, INPUT_PULLUP); // Définir la broche du bouton comme entrée avec résistance de rappel interne
  pinMode(BUTTON2_PIN, INPUT_PULLUP); // Définir la broche du bouton comme entrée avec résistance de rappel interne
  pinMode(BUTTON3_PIN, INPUT_PULLUP); // Définir la broche du bouton comme entrée avec résistance de rappel interne
}

void loop() {
  /* Mesurer la distance */
  if (millis() - lastMeasurementTime >= MEASUREMENT_INTERVAL || (digitalRead(BUTTON3_PIN) == LOW && mode == MODE_NORMAL)) {
    lastMeasurementTime = millis();
    lastButtonPressTime = millis();

    delay(50); // Attendre 50ms entre les pings (environ 20 pings/sec). 29ms devrait être le délai le plus court entre les pings
    unsigned int uS = sonar.ping(); // Envoyer un ping, obtenir le temps du ping en microsecondes (uS)
    int distance = uS / US_ROUNDTRIP_CM; // Convertir le temps du ping en distance en cm

    /* Envoyer la distance à l'ordinateur en utilisant le protocole Série */
    Serial.println(distance);

    /* Afficher la distance sur l'OLED */
    display.clearDisplay(); // Effacer l'affichage
    display.setTextSize(1); // Définir la taille du texte
    display.setTextColor(WHITE); // Définir la couleur du texte
    display.setCursor(0,0); // Définir la position du curseur
    display.println("Distance:"); // Afficher "Distance:"
    display.println(distance); // Afficher la distance
    display.display(); // Mettre à jour l'affichage

    /* Stocker la mesure */
    for (int i = 9; i > 0; i--) {
      measurements[i] = measurements[i-1];
    }
    measurements[0].distance = distance;
    bool h12, PM_time, Century;
    measurements[0].hour = myRTC.getHour(h12, PM_time);
    measurements[0].minute = myRTC.getMinute();
    measurements[0].day = myRTC.getDate();
    measurements[0].month = myRTC.getMonth(Century);
    measurements[0].year = myRTC.getYear();
  }

  /* Vérifier si le bouton de réglage de l'heure et de la date est pressé */
  if (digitalRead(BUTTON1_PIN) == LOW) {
    lastButtonPressTime = millis();
    if (mode == MODE_NORMAL) {
      if (millis() - lastButtonPressTime > 10000) {
        mode = MODE_SET_HOUR;
        setHour = 0;
        setMinute = 0;
        setDay = 30;
        setMonth = 10;
        setYear = 2013;
      }
    } else {
      if (millis() - lastButtonPressTime > 10000) {
        if (mode == MODE_SET_HOUR) {
          mode = MODE_SET_MINUTE;
        } else if (mode == MODE_SET_MINUTE) {
          mode = MODE_SET_DAY;
        } else if (mode == MODE_SET_DAY) {
          mode = MODE_SET_MONTH;
        } else if (mode == MODE_SET_MONTH) {
          mode = MODE_SET_YEAR;
        } else if (mode == MODE_SET_YEAR) {
          mode = MODE_NORMAL;
          myRTC.setHour(setHour); // Set the hour
          myRTC.setMinute(setMinute); // Set the minute
          myRTC.setDate(setDay); // Set the date
          myRTC.setMonth(setMonth); // Set the month
          myRTC.setYear(setYear); // Set the year
        }
      } else {
        if (mode == MODE_SET_HOUR) {
          setHour = (setHour % 24) + 1;
        } else if (mode == MODE_SET_MINUTE) {
          setMinute = (setMinute % 60) + 1;
        } else if (mode == MODE_SET_DAY) {
          setDay = (setDay % 31) + 1;
        } else if (mode == MODE_SET_MONTH) {
          setMonth = (setMonth % 12) + 1;
        } else if (mode == MODE_SET_YEAR) {
          setYear++;
        }
      }
    }
  }

  /* Vérifier si le bouton de changement d'affichage est pressé */
  if (digitalRead(BUTTON2_PIN) == LOW) {
    lastButtonPressTime = millis();
    displayMode = (DisplayMode)((displayMode + 1) % 3);
  }

  /* Afficher les informations appropriées en fonction du mode d'affichage */
  display.clearDisplay(); // Effacer l'affichage
  display.setTextSize(1); // Définir la taille du texte
  display.setTextColor(WHITE); // Définir la couleur du texte
  display.setCursor(0,0); // Définir la position du curseur
  if (displayMode == DISPLAY_VOLUME) {
    display.println("Volume:");
    display.println(measurements[0].distance);
  } else if (displayMode == DISPLAY_HISTORY) {
    for (int i = 0; i < 10; i++) {
      display.print(measurements[i].distance);
      display.print(" ");
      display.print(measurements[i].day);
      display.print("/");
      display.print(measurements[i].month);
      display.print("/");
      display.print(measurements[i].year);
      display.print(" ");
      display.print(measurements[i].hour);
      display.print(":");
      display.println(measurements[i].minute);
    }
  } else if (displayMode == DISPLAY_TIME) {
    bool h12, PM_time, Century;
    display.print(myRTC.getDate());
    display.print("/");
    display.print(myRTC.getMonth(Century));
    display.print("/");
    display.print(myRTC.getYear());
    display.print(" ");
    display.print(myRTC.getHour(h12, PM_time));
    display.print(":");
    display.println(myRTC.getMinute());
  }
  display.display(); // Mettre à jour l'affichage

  /* Éteindre l'écran OLED si aucun bouton n'a été pressé pendant une minute */
  if (millis() - lastButtonPressTime > 60000) {
    display.ssd1306_command(SSD1306_DISPLAYOFF);
  } else {
    display.ssd1306_command(SSD1306_DISPLAYON);
  }
}
