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
const byte pot_ancho    = 21;    // potenciometro para poner el ancho (esto se cambiará por 4 celdas de carga que llevarán el carrito)
const byte pot_peso     = 19;    // potenciometro para poner el ancho (esto se cambiará por 4 celdas de carga que llevarán el carrito)
const byte boton_tela   = 20;    // Boton para selección de tela
float largo_radio  = 0.0623666343;

volatile byte state = LOW;        // para el led de la board
bool p = false, f = true;         // bandeas para controlar y eliminar lecturas falsas
float largo = 0;                // variable global para acumular el largo en metros
float peso = 0;                 // variable global para acumular el peso en kg
float ancho = 0;                // en metros
int gramaje = 0;                // en gramos x m2+
int humedad = 0;                 // en %
int rollo = 1;
float rendimiento = 0;          // en metros / kg
int tipo = 1;                   // variable para que el usuario seleccione el tipo de tela
int pasos = 0;
int estado = 1;
void setup() {
  lcd.begin(20, 4);
  lcd.setCursor(6, 0);
  lcd.print("LICRATeX");
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
  interrupcion();
  zero();                     // boton de zero o tara, para calibrar a zero el peso al encender la máquina o si se descalibra.
  tela();
  wide();
  weight();
  pantalla();                 // cada 250 ms se refresca la pantalla del LCD con la info nueva
  teclado();                  // envía por teclado la información, SOLO ENVÍA BAJO REQUES
}

void teclado() {
  unsigned long time1 = millis();
  // enviamos aquí toda la info por teclado, esto solo ocurre cuando el usuario presiona el botón de enviar,
  // sobreentendiendo que ya temrinó de salir el rollo y que el cursor está en la primera celda donde se desea
  // escribir la info, cuando ya se envía la info al pc, se coloca en zero nuevamente el valor del cuentametros
  // para estar preparados para el proximo rollo de tela.
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
    Keyboard.write(0xB3);  //        // TAB
    switch (tipo) {
      case 1:
        Keyboard.print("Jersey");
        break;
      case 2:
        Keyboard.print("Pique");
        break;
      case 3:
        Keyboard.print("Rib");
        break;
      case 4:
        Keyboard.print("Sabina");
        break;
      case 5:
        Keyboard.print("Microfibra Lycrada");
        break;
      case 6:
        Keyboard.print("Interlock");
        break;
      case 7:
        Keyboard.print("Microfibra");
        break;
      case 8:
        Keyboard.print("Algodon Lycra");
        break;
      case 9:
        Keyboard.print("Polyester Algodon Lycra");
        break;
    }
    Keyboard.write(0xB3);  //        // TAB
    largo = 0;
    rollo++;
    lcd.clear();
    while (!digitalRead(enviar) || millis() - time1 < 1000) {
    }
  }
}

void wide() {
  ancho = analogRead(pot_ancho); // lee el valor del potenciometrom
  ancho = map(ancho, 1, 1023, 1900, 1500);
  ancho = ancho / 1000;
}

void weight() {
  peso = analogRead(pot_peso); // lee el valor del potenciometro
  peso = map(peso  , 1, 1023, 2500, 1500);
  peso = peso / 100;
}

void pantalla() {

  if (millis() % 250 == 0) {
    gramaje = (peso * 1000) / (ancho * largo);
    if (gramaje < 100 || gramaje > 500) {
      gramaje = 0;
    }
    rendimiento = largo / peso;
    lcd.setCursor(0, 0);
    lcd.print("MTS :");
    lcd.print(largo);
    lcd.setCursor(0, 1);
    lcd.print("PESO:");
    lcd.print(peso);
    lcd.setCursor(0, 2);
    lcd.print("ANCH:");
    lcd.print(ancho);
    lcd.setCursor(11, 3);
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
    lcd.setCursor(0, 3);
    switch (tipo) {
      case 1:
        lcd.print("Jersey    ");
        break;
      case 2:
        lcd.print("Pique     ");
        break;
      case 3:
        lcd.print("Rib       ");
        break;
      case 4:
        lcd.print("Sabina   ");
        break;
      case 5:
        lcd.print("MicrofLyc");
        break;
      case 6:
        lcd.print("Interlock");
        break;
      case 7:
        lcd.print("Microfibr");
        break;
      case 8:
        lcd.print("AlgLycra ");
        break;
      case 9:
        lcd.print("PolyAlgLyc");
        break;
    }
  }
}

void zero() { // funcion que lleva todas las variables a zero, sirve para calibrar la balanza y el odometro a zero
  unsigned long time1;
  if (!digitalRead(zeroPin)) {
    lcd.clear();
    time1 = millis();
    largo = 0;
    peso = 0;
    pasos = 0;
    while (!digitalRead(zeroPin)) {
      lcd.setCursor(6, 2);
      lcd.print("BORRANDO...");
      if (millis() - time1 > 5000) {
        lcd.clear();
        rollo = 1;
        lcd.setCursor(0, 2);
        lcd.print("BORRADO FINALIZADO");
      }
    }
    lcd.clear();

  }
}

void tela() {
  // Al presionar el boton de tipo de tela se cambia en la pantalla y en el valor enviado el tipo de tela
  // 1 Jersey
  // 2 PIQUE
  // 3 RIB
  // 4 SABINA
  // 5 MicroFibraLycrada
  // 6 Interlock
  // 7 Microfibra
  // 8 Cotton Lycra
  // 9 PolyCottonLycra
  unsigned long time1 = millis();
  if (analogRead(boton_tela) < 500) {
    tipo++;
    if (tipo > 9) {
      tipo = 1;
    }
    while (millis - time1 < 200) {
      bool b;
    }
    time1 = millis();
    while (analogRead(boton_tela) == 0 || millis() - time1 < 200) {
      bool b;
    }
    while (millis - time1 < 200) {
      bool b;
    }
  }
}

void interrupcion() {
  if (p) {
    p = false;
    largo += largo_radio;
    pasos++;
    f = true;
  }
}

void blink() {
  if (f) {
    f = false;
    p = true;
  }
}
