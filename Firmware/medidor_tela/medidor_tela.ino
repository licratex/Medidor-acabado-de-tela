#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Constructor de la librería de LCD 16x2
// Aqui se configuran los pines asignados a la pantalla del PCF8574
// Este constructor es para usar con el modulo I2C que se muestra en las
// fotografias de nuestro sitio web. Para otros modelos, puede ser necesario
// cambiar los valores de acuerdo al esquemático del adaptador I2C. Los pines
// del arduino SIEMPRE son los correspondientes al I2C (SDA y SCL)

// Constructor sin control de backlight (retroiluminacion)
//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7);
// Constructor con control de backlignt (retroiluminacion)
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// DEFINICION DE PINES

#define ANCHO_INC 8      // BOTON PARA ENVIAR LA INFO AL PC
#define ANCHO_DEC 9      // BOTON PARA ENVIAR LA INFO AL PC
#define ENVIAR 7          // BOTON PARA ENVIAR LA INFO AL PC
#define TESTIGO 5     // BOTON PARA CALIBRAR CON UN PESO DE CONTROL
#define ZERO 4        // BOTON PARA PONER EN CERO EL MEDIDOR
const byte HERRADURA = 7; // AQUI VA CONECTADO EL SENSOR DE HERRADURA

//  VARIABLES GLOBALES
bool p=false;
bool p2=true;
int largo_radio= (3.1416*2*10)/4; // 10 es el radio de la rueda y 4 son los radios que tiene
int largo_rollo=0;

void setup() {
  lcd.begin(16, 24);     // INDICO AL ARDUINO LAS DIMENSIONES DEL LCD
  lcd.clear();          // LIMPIO PANTALLA Y VOY AL PUNTO 0,0
  lcd.print(" LICRATEX ");
  pinMode(ENVIAR,INPUT);
  pinMode(TESTIGO, INPUT);
  pinMode(ZERO, INPUT);
  pinMode(HERRADURA, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HERRADURA), paso, FALLING);  
  delay(1000);
}

void loop() {
 largo();
 ancho();
 pantalla();
}

void ancho(){
  
  }

void largo(){
  if(p){  // si se detecto un radio incrementar
  largo_rollo+=largo_radio;
  p=false;    
  }
}



void pantalla(){
  lcd.clear();
  lcd.print(largo_rollo);         // aquí iran las variables
  lcd.setCursor ( 6, 0 );
  lcd.print("ANCHO");     
  lcd.setCursor ( 0, 1 );  
  lcd.print("PESO");     
  lcd.setCursor ( 6, 1 );    
  lcd.print("GRAMAJE");  
}

void paso(){ // interrupcion cada vez que se interrupme el sensor de herradura
  p=true;    
  }
