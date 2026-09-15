#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Capteur ultrasons HC-SR04
#define PIN_TRIG    7
#define PIN_ECHO    8

// Bouton Grove v1.3
#define PIN_BOUTON  3

// Servomoteur
#define PIN_SERVO   5

#define DISTANCE_SEUIL_CM 5 // Déclenchement à moins de 5 cm
#define STOCK_INITIAL     10

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Servo pushServo;

int stock = STOCK_INITIAL;

long mesurerDistanceCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duree = pulseIn(PIN_ECHO, HIGH, 30000); // Timeout à 30 ms
  if (duree == 0) return 999;
  return duree * 0.034 / 2;
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
    display.print("VIDE !");
  }
  display.display();
}

void setup() {
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  
  // Le module Grove Button intègre déjà sa propre résistance, un simple INPUT suffit
  pinMode(PIN_BOUTON, INPUT);

  pushServo.attach(PIN_SERVO);
  pushServo.write(0); // Position repos

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  majEcran();
}

void loop() {
  // Réarmement du stock lors de l'appui sur le bouton Grove (envoie HIGH à l'appui)
  if (digitalRead(PIN_BOUTON) == HIGH) {
    stock = STOCK_INITIAL;
    majEcran();
    delay(300); // Anti-rebond
  }

  // Mesure de la distance du capteur ultrasons
  long distance = mesurerDistanceCm();

  // Si la main passe à moins de 10 cm et qu'il reste du stock
  if (distance > 0 && distance <= DISTANCE_SEUIL_CM && stock > 0) {
    // Action du moteur push (servomoteur)
    pushServo.write(90); // Pousse
    delay(400);
    pushServo.write(0);  // Revient
    delay(400);

    stock--;
    majEcran();

    // Pause pour laisser le temps de retirer la main
    delay(1500);
  }

  delay(60);
} 