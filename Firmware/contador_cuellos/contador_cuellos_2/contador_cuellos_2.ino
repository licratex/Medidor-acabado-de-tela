#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int lcd_key     =     0;
int adc_key_in  =     0;
#define btnRIGHT      0
#define btnUP         1
#define btnDOWN       2
#define btnLEFT       3
#define btnSELECT     4
#define btnNONE       5
#define interruptPin  2
#define zeroPin       3


unsigned long t1 = 0;
int teclado = 1;
int cantidad = 0;
volatile byte state = LOW;      // para el led de la board
unsigned long time1, time2;     // variables para guardar tiempo de ejecucion y eliminar lecuras falsas
bool p = false, f = true;       // bandeas para controlar y eliminar lecturas falsas
const int bounce     = 100;    // retardo para evitar rebote en lectura de interrupcion, entre mayor mas estable, pero si el tiempo es muy largo podria dejar pasar objetos sin detectar

int tipo = 1;

void setup() {
  pinMode(interruptPin, INPUT_PULLUP);                                      // activar pullup interno de la interrupcion
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, FALLING);     // definir pin de interrupcion, nombre de la interrupcion, trigger de la interrupccion
  lcd.begin(16, 2);              // Inicializar el LCD
  lcd.setCursor(1, 0);
  lcd.print("***LICRTEX***");     // print a simple message
  delay(2000);
  lcd.clear();
}

void loop() {
  if (millis() - t1 > 100) {
    teclado = botones_f();
    seleccion();
    pantalla();
    t1 = millis();
  }
  interrupcion();
}

void pantalla() {
  int c;
  lcd.setCursor(0, 0);
  switch (tipo) {
    case 1:
      lcd.print("Cuellos UND:    ");     // se envia por serial la informacion, probablementese cambie al loop principal
      c = cantidad;
      lcd.setCursor(0, 1);
      lcd.print(c);
      break;
    case 2:
      lcd.print("Tiras PAR:      ");     // se envia por serial la informacion, probablementese cambie al loop principal
      c = cantidad / 2;
      lcd.setCursor(0, 1);
      lcd.print(c);
      break;
    case 3:
      lcd.print("Combos TRIO: ");     // se envia por serial la informacion, probablementese cambie al loop principal
      c = cantidad / 3;
      lcd.setCursor(0, 1);
      lcd.print(c);
      break;
  }
  lcd.print("   ");
 }

void seleccion() {
  if (teclado != 5) {
    if (teclado == btnUP) {
      tipo++;
      if (tipo > 3) {
        tipo = 3;
      }
    }
    if (teclado == btnDOWN) {
      tipo--;
      if (tipo < 1 ) {
        tipo = 1;
      }
    }
  }
}

void zero() { // funcion que lleva todas las variables a zero, sirve para calibrar la balanza y el odometro a zero
  if (teclado == btnSELECT) {
    cantidad = 0;
    delay(100);
  }
}

int botones_f() {
  adc_key_in = analogRead(0);      // Leemos A0
  // Mis botones dan:  0, 145, 329,507,743
  // Y ahora los comparamos con un margen comodo
  if (adc_key_in > 900) return btnNONE;     // Ningun boton pulsado
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 250)  return btnUP;
  if (adc_key_in < 450)  return btnDOWN;
  if (adc_key_in < 650)  return btnLEFT;
  if (adc_key_in < 850)  return btnSELECT;
  return btnNONE;  // Por si todo falla
}

void interrupcion() {
  if (p) {
    state = !state;
    p = false;
    cantidad++;
    time1 = millis();    
  }
  if (millis()- time1 > bounce) {
    f = true;
  }
}

void blink() {
  if (f) {
    f = false;
    p = true;
  }
}
