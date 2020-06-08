#include <HX711.h>

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
const byte pot_ancho    = 4;     // potenciometro para poner el ancho (esto se cambiará por 4 celdas de carga que llevarán el carrito)
const byte pot_peso     = 6;    // potenciometro para poner el ancho (esto se cambiará por 4 celdas de carga que llevarán el carrito)

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
int tipo=1;                     // variable para que el usuario seleccione el tipo de tela

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
  interrupcion();             // lleva el conteo del encoder para calcular los metros de largo
  zero();                     // boton de zero o tara, para calibrar a zero el peso al encender la máquina o si se descalibra.
  tela();
  wide();
  weight();
  pantalla();                 // cada 250 ms se refresca la pantalla del LCD con la info nueva
  teclado();                  // envía por teclado la información, SOLO ENVÍA BAJO REQUEST
}

void wide(){
  ancho=analogRead(pot_ancho); // lee el valor del potenciometrom
  ancho=map(ancho, 1, 1023, 1.19, 2.20);
}

void weight(){
  peso=analogRead(pot_peso); // lee el valor del potenciometro
  peso=map(peso  , 1, 1023, 5, 30);
}

void tela(){
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
  if(!digitalRead(enviar)){
    tipo++;
    if(tipo>9){
      tipo=1;
    }
  while(!digitalRead(enviar)){
    delay(300);
  }
  }
}

void pantalla() {

  if (millis() % 250 == 0) {
    gramaje = (peso * 1000) / (ancho * largo);
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
    lcd.setCursor(11, 3);
    switch(tipo){
      case 1:
      lcd.print("Jersey");
      break;
      case 2:
      lcd.print("Pique");
      break;
      case 3:
      lcd.print("Rib");
      break;
      case 4:
      lcd.print("Sabina");
      break;
      case 5:
      lcd.print("MicrofLy");
      break;
      case 6:
      lcd.print("Interloc");
      break;
      case 7:
      lcd.print("Microfib");
      break;
      case 8:
      lcd.print("AlgLycr");
      break;
      case 9:
      lcd.print("PolAlLy");
      break;
    }
  }
}
void teclado() {
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
     switch(tipo){
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
    while (!digitalRead(enviar)) {
      delay(500);
    }
    delay(500);
  }
}

void zero() { // funcion que lleva todas las variables a zero, sirve para calibrar la balanza y el odometro a zero
  if (!digitalRead(zeroPin)) {
    largo = 0;
    peso = 0;
    rollo++;
    lcd.clear();
    int c=0;        //variable para determinar si el boton de zero se dejo presionado mas de 5 segundos, con lo gual se pone en zero tambien la cantidad de rollos.
    while (!digitalRead(zeroPin)) {
      c++;
      delay(100);
    }
    if(c>50){
      rollo=0;
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
