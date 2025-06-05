#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DS3231.h>
#include <NewPing.h>
#include <math.h> // Bibliothèque pour utiliser la constante pi

// Définition des dimensions de l'écran OLED
#define SCREEN_WIDTH 128 // Largeur de l'écran OLED en pixels
#define SCREEN_HEIGHT 64 // Hauteur de l'écran OLED en pixels
#define OLED_RESET -1 // Numéro de la broche de réinitialisation (ou -1 si elle partage la broche de réinitialisation de l'Arduino)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DS3231 myRTC; // Objet pour le module d'horloge en temps réel

// Définition des broches pour le capteur à ultrasons
#define TRIGGER_PIN 3  // Broche Arduino connectée à la broche de déclenchement du capteur à ultrasons
#define ECHO_PIN 2  // Broche Arduino connectée à la broche d'écho du capteur à ultrasons
#define MAX_DISTANCE 200 // Distance maximale que nous voulons mesurer (en centimètres). La distance maximale du capteur est évaluée à 400-500cm

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // Configuration de NewPing avec les broches et la distance maximale

// Définition des broches pour les boutons
#define BUTTON1_PIN 4 // Broche Arduino connectée au bouton de réglage de l'heure et de la date
#define BUTTON2_PIN 5 // Broche Arduino connectée au bouton de changement d'affichage
#define BUTTON3_PIN 6 // Broche Arduino connectée au bouton de mesure immédiate du niveau d'eau

// Énumération pour les différents modes de réglage de l'horloge
enum Mode {MODE_NORMAL, MODE_SET_HOUR, MODE_SET_MINUTE, MODE_SET_DAY, MODE_SET_MONTH, MODE_SET_YEAR};
Mode mode = MODE_NORMAL;

// Variables pour le réglage de l'heure
unsigned long lastButtonPressTime = 0;
int prevButton1State = HIGH;
int prevButton2State = HIGH;
int prevButton3State = HIGH;
unsigned long button1PressStart = 0;
bool button1LongPressHandled = false;
int setHour = 0, setMinute = 0, setDay = 1, setMonth = 1, setYear = 2013;

// Variables pour la mesure de la distance
unsigned long lastMeasurementTime = 0;
#define MEASUREMENT_INTERVAL (12UL * 60UL * 60UL * 1000UL) // Intervalle de mesure en millisecondes (12 heures)

// Structure pour stocker les mesures
struct Measurement {
  int distance; // Distance mesurée
  float volume; // Volume calculé
  int hour, minute, day, month, year; // Date et heure de la mesure
};

Measurement measurements[10]; // Tableau pour stocker les 10 dernières mesures

// Énumération pour les différents modes d'affichage
enum DisplayMode {DISPLAY_VOLUME, DISPLAY_HISTORY, DISPLAY_TIME};
DisplayMode displayMode = DISPLAY_VOLUME;

// Constantes pour la cuve
#define RADIUS_INTERIOR 1170 // Rayon intérieur de la cuve en mm
#define HEIGHT_TRUNCATED 880 // Hauteur du segment tronqué intérieur en mm
#define HEIGHT_MAX 1520 // Hauteur maximale d'eau dans la cuve en mm
#define SENSOR_HEIGHT 300 // Hauteur du capteur au-dessus du trop-plein d'eau en mm

// Fonction pour calculer le volume en fonction de la distance mesurée
float calculateVolume(int distance) {
  float h = HEIGHT_MAX + SENSOR_HEIGHT - distance * 10; // Conversion de la distance en hauteur d'eau en mm
  if (h < 0) h = 0;
  if (h <= HEIGHT_TRUNCATED) {
    return (M_PI * h * h / 3) * (3 * RADIUS_INTERIOR - h) / 1000; // Formule pour le volume d'un segment sphérique en litres
  } else {
    float h_truncated = HEIGHT_MAX - h;
    return (4.0 / 3.0 * M_PI * RADIUS_INTERIOR * RADIUS_INTERIOR * RADIUS_INTERIOR
            - M_PI * h_truncated * h_truncated / 3 * (3 * RADIUS_INTERIOR - h_truncated)) / 1000; // Formule pour le volume d'une sphère tronquée en litres
  }
}

void setup() {
  Serial.begin (9600); // Commencer la communication série à 9600 bauds
  Wire.begin(); // Commencer la communication avec le module d'horloge en temps réel
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialiser l'écran OLED avec l'adresse I2C 0x3C (pour le 128x64)

  pinMode(BUTTON1_PIN, INPUT_PULLUP); // Définir la broche du bouton comme entrée avec résistance de rappel interne
  pinMode(BUTTON2_PIN, INPUT_PULLUP); // Définir la broche du bouton comme entrée avec résistance de rappel interne
  pinMode(BUTTON3_PIN, INPUT_PULLUP); // Définir la broche du bouton comme entrée avec résistance de rappel interne
}

void loop() {
  int button1State = digitalRead(BUTTON1_PIN);
  int button2State = digitalRead(BUTTON2_PIN);
  int button3State = digitalRead(BUTTON3_PIN);

  // Mesure de la distance si l'intervalle est écoulé ou si le bouton 3 vient d'
  // être pressé
  if (millis() - lastMeasurementTime >= MEASUREMENT_INTERVAL ||
      (button3State == LOW && prevButton3State == HIGH && mode == MODE_NORMAL)) {
    lastMeasurementTime = millis();
    if (button3State == LOW && prevButton3State == HIGH) {
      lastButtonPressTime = millis();
    }

    delay(50); // Délai entre les pings
    unsigned int uS = sonar.ping(); // Mesure du temps de ping en microsecondes
    int distance = uS / US_ROUNDTRIP_CM; // Conversion du temps de ping en distance en cm
    float volume = calculateVolume(distance); // Calcul du volume en litres

    Serial.println(distance); // Envoi de la distance à la console série

    // Affichage de la distance et du volume sur l'écran OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Distance:");
    display.println(distance);
    display.println("Volume:");
    display.println(volume);
    display.display();

    // Stockage de la mesure dans le tableau
    for (int i = 9; i > 0; i--) {
      measurements[i] = measurements[i-1];
    }
    measurements[0].distance = distance;
    measurements[0].volume = volume;
    bool h12, PM_time, Century;
    measurements[0].hour = myRTC.getHour(h12, PM_time);
    measurements[0].minute = myRTC.getMinute();
    measurements[0].day = myRTC.getDate();
    measurements[0].month = myRTC.getMonth(Century);
    measurements[0].year = myRTC.getYear();
  }

  // Gestion du bouton de réglage de l'heure et de la date
  if (button1State == LOW && prevButton1State == HIGH) {
    button1PressStart = millis();
    button1LongPressHandled = false;
    lastButtonPressTime = millis();
  }

  if (button1State == LOW && !button1LongPressHandled &&
      millis() - button1PressStart >= 10000) {
    button1LongPressHandled = true;
    lastButtonPressTime = millis();
    if (mode == MODE_NORMAL) {
      mode = MODE_SET_HOUR;
      setHour = 0;
      setMinute = 0;
      setDay = 30;
      setMonth = 10;
      setYear = 2013;
    } else {
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
        myRTC.setHour(setHour);
        myRTC.setMinute(setMinute);
        myRTC.setDate(setDay);
        myRTC.setMonth(setMonth);
        myRTC.setYear(setYear);
      }
    }
  }

  if (button1State == HIGH && prevButton1State == LOW) {
    unsigned long pressDuration = millis() - button1PressStart;
    if (pressDuration < 10000) {
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
    lastButtonPressTime = millis();
  }

  if (mode != MODE_NORMAL && millis() - lastButtonPressTime >= 10000) {
    lastButtonPressTime = millis();
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
      myRTC.setHour(setHour);
      myRTC.setMinute(setMinute);
      myRTC.setDate(setDay);
      myRTC.setMonth(setMonth);
      myRTC.setYear(setYear);
    }
  }

  // Gestion du bouton de changement d'affichage
  if (button2State == LOW && prevButton2State == HIGH) {
    lastButtonPressTime = millis();
    displayMode = (DisplayMode)((displayMode + 1) % 3);
  }

  // Affichage des informations en fonction du mode d'affichage
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  if (displayMode == DISPLAY_VOLUME) {
    display.println("Volume:");
    display.println(measurements[0].volume);
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
  display.display();

  // Éteindre l'écran OLED si aucun bouton n'a été pressé pendant une minute
  if (millis() - lastButtonPressTime > 60000) {
    display.ssd1306_command(SSD1306_DISPLAYOFF);
  } else {
    display.ssd1306_command(SSD1306_DISPLAYON);
  }

  prevButton1State = button1State;
  prevButton2State = button2State;
  prevButton3State = button3State;
}
