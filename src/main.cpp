#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include "rgb_lcd.h"

// Capteur ultrasons HC-SR04
#define PIN_TRIG    7
#define PIN_ECHO    8

// Bouton Grove v1.3
#define PIN_BOUTON  4

// Servomoteur
#define PIN_SERVO   5

#define DISTANCE_SEUIL_CM 5
#define STOCK_INITIAL     10

rgb_lcd lcd;
Servo pushServo;

int stock = STOCK_INITIAL;

long mesurerDistanceCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duree = pulseIn(PIN_ECHO, HIGH, 30000);
  if (duree == 0) return 999;
  return duree * 0.034 / 2;
}

void majEcran() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BONBONS");

  lcd.setCursor(0, 1);
  if (stock > 0) {
    lcd.print("Stock: ");
    lcd.print(stock);
  } else {
    lcd.print("VIDE !");
  }
}

void setup() {
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_BOUTON, INPUT);

  pushServo.attach(PIN_SERVO);
  pushServo.write(0);

  // Initialisation du LCD (16 colonnes, 2 lignes)
  lcd.begin(16, 2);
  // Couleur du rétroéclairage (Rouge, Vert, Bleu)
  lcd.setRGB(0, 128, 255); 
  
  majEcran();
}

void loop() {
  // Réarmement du stock avec le bouton
  if (digitalRead(PIN_BOUTON) == HIGH) {
    stock = STOCK_INITIAL;
    majEcran();
    delay(300);
  }

  long distance = mesurerDistanceCm();

  if (distance > 0 && distance <= DISTANCE_SEUIL_CM && stock > 0) {
    // Changement de couleur pendant le service (Vert)
    lcd.setRGB(0, 255, 0);

    pushServo.write(90);
    delay(1000);
    pushServo.write(0);
    delay(400);

    stock--;
    majEcran();

    // Retour à la couleur initiale (Bleu)
    lcd.setRGB(0, 128, 255);
    delay(1500);
  }

  delay(60);
}