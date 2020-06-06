#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#include <Keyboard.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address


// INTERRUPCION PARA ENCODER FUNCINANDO CON CONTROL DE RUIDO EN LECTURA

// PINS
const byte ledPin       = 13;   // pin de led solo para monitorear
const byte interruptPin = 7;    // pin al que esta conectada la interrupcion
const byte zeroPin      = 5;    // boton setear a cero la longiud y el peso
const byte enviar       = 8;    // boton para enviar

volatile byte state = LOW;      // para el led de la board
unsigned long time1, time2;     // variables para guardar tiempo de ejecucion y eliminar lecuras falsas
bool p = false, f = true;       // bandeas para controlar y eliminar lecturas falsas
const float largo_radio = 0.03125; // perimetro entre un radio y otro
float largo = 20;                // variable global para acumular el largo en metros
float peso = 6.42;                 // variable global para acumular el peso en kg
float ancho = 1.60;                // en metros
int gramaje = 0;                // en gramos x m2
int humedad = 0;                 // en %
int rollo = 1;
float rendimiento = 0;          // en metros / kg


void setup() {
  lcd.begin(20, 4);
  lcd.setCursor(6, 0);
  lcd.print("LICRATEX");
  lcd.setCursor(8, 1);
  lcd.print("****");
  lcd.setCursor(2, 2);
  lcd.print("Metricas Abierta");
  lcd.setCursor(8, 3);
  lcd.print("****");
  pinMode(enviar, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(zeroPin, INPUT_PULLUP);                                           // activar pullup interno para el boton de tare/zero
  pinMode(interruptPin, INPUT_PULLUP);                                      // activar pullup interno de la interrupcion
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, FALLING);     // definir pin de interrupcion, nombre de la interrupcion, trigger de la interrupccion
  delay(2000);
  lcd.begin(20, 4);
}

void loop() {
  digitalWrite(ledPin, state);
  interrupcion();
  zero();

  if (millis() % 250 == 0) {
    gramaje = (peso * 1000) / (ancho * largo);
    rendimiento = largo / peso;
    pantalla();
  }
  teclado();
}

void teclado() {
  if (!digitalRead(enviar)) {
    Keyboard.print(rollo);  //        // TAB
    Keyboard.write(0xB3);  //        // TAB
    Keyboard.print(peso);  //        // TAB
    Keyboard.write(0xB3);  //        // TAB
    Keyboard.print(ancho);  //        // TAB
    Keyboard.write(0xB3);  //        // TAB
    Keyboard.print(largo);  //        // TAB
    Keyboard.write(0xB3);  //        // TAB
    Keyboard.print(rendimiento);  //        // TAB
    Keyboard.write(0xB3);  //        // TAB
    Keyboard.print(gramaje);  //        // TAB
    Keyboard.write(0xB0);  //        // TAB
    largo = 0;
   // peso = 0;
    rollo++;
    lcd.clear();
    while(!digitalRead(enviar)){
      delay(100);
    }
    delay(100);
  }
}


void pantalla() {
  lcd.setCursor(0, 0);
  lcd.print("MTS :");
  lcd.print(largo);
  lcd.setCursor(0, 1);
  lcd.print("PESO:");
  lcd.print(peso);
  lcd.setCursor(0, 2);
  lcd.print("ANCH:");
  lcd.print(ancho);
  lcd.setCursor(11, 4);
  lcd.print("ROLL:");
  lcd.print(rollo);

  lcd.setCursor(11, 0);
  lcd.print("GRAM:");
  lcd.print(gramaje);
  lcd.setCursor(11, 1);
  lcd.print("REND:");
  lcd.print(rendimiento);
  lcd.setCursor(11, 2);
  lcd.print("HUME:");
  lcd.print(humedad);
}

void zero() { // funcion que lleva todas las variables a zero, sirve para calibrar la balanza y el odometro a zero
  if (!digitalRead(zeroPin)) {
    largo = 0;
    peso = 0;
    rollo++;
    lcd.clear();
    while (!digitalRead(zeroPin)) {
      delay(100);
    }
    delay(1000);
  }
}

void interrupcion() {
  if (p) {
    state = !state;
    p = false;
    largo += largo_radio;
  }
  time2 = millis();            // las lineas siguiente controlan el tiempo que ocaciona rebote o lecturas en falso.
  if (time2 - time1 > 100) {
    f = true;
  }
}

void blink() {
  if (f) {
    f = false;
    p = true;
    time1 = millis();
  }
}
