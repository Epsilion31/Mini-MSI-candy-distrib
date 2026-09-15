#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define PIN_CAPTEUR 2
#define PIN_BOUTON  3
#define PIN_SERVO   9

#define STOCK_INITIAL 10

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Servo pushServo;

int stock = STOCK_INITIAL;

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
  pinMode(PIN_CAPTEUR, INPUT);
  pinMode(PIN_BOUTON, INPUT_PULLUP);

  pushServo.attach(PIN_SERVO);
  pushServo.write(0); // Position repos

  // Initialisation écran OLED (adresse 0x3C standard)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  majEcran();
}

void loop() {
  // Recharger le stock quand on presse le bouton
  if (digitalRead(PIN_BOUTON) == LOW) {
    stock = STOCK_INITIAL;
    majEcran();
    delay(300); // Anti-rebond
  }

  // Détection main (la plupart des modules IR passent à LOW quand ils détectent)
  if (digitalRead(PIN_CAPTEUR) == LOW && stock > 0) {
    // Mouvement de poussée
    pushServo.write(90);  // Pousse
    delay(400);
    pushServo.write(0);   // Revient
    delay(400);

    stock--;
    majEcran();

    // Pause pour éviter de distribuer plusieurs fois de suite
    delay(1500);
  }
}