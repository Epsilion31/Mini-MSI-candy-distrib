#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Broches capteur ultrason
#define PIN_TRIG    2
#define PIN_ECHO    4

#define PIN_BOUTON  3
#define PIN_SERVO   9

#define DISTANCE_SEUIL_CM 5 // Déclenche à moins de 5 cm
#define STOCK_INITIAL     10

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Servo pushServo;

int stock = STOCK_INITIAL;

long mesurerDistanceCm() {
  // Envoi d'une impulsion de 10 µs sur Trig
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // Lecture du temps de retour de l'écho en µs
  long duree = pulseIn(PIN_ECHO, HIGH, 30000); // Timeout à 30 ms (~5 m max)
  
  if (duree == 0) return 999; // Pas d'obstacle détecté
  return duree * 0.034 / 2;    // Vitesse du son : 340 m/s
}

void majEcran() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.print("BONBONS");

  display.setCursor(10, 40);
  if (stock > 0) {
    display.print("Stock: ");
    display.print(stock);
  } else {
    display.print("CHEH");
  }
  display.display();
}

void setup() {
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_BOUTON, INPUT_PULLUP);

  pushServo.attach(PIN_SERVO);
  pushServo.write(0); // Position repos

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  majEcran();
}

void loop() {
  // Réarmement du stock avec le bouton
  if (digitalRead(PIN_BOUTON) == LOW) {
    stock = STOCK_INITIAL;
    majEcran();
    delay(300);
  }

  // Mesure de distance
  long distance = mesurerDistanceCm();

  // Si un obstacle est détecté à moins de 10 cm
  if (distance > 0 && distance <= DISTANCE_SEUIL_CM && stock > 0) {
    // Action du servo (pousse puis revient)
    pushServo.write(90);
    delay(400);
    pushServo.write(0);
    delay(400);

    stock--;
    majEcran();

    // Délai d'attente pour retirer la main sans re-déclencher
    delay(1500);
  }

  delay(60); // Cadence de mesure stable
}