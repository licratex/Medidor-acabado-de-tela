
#include <Q2HX711.h>

const byte hx711_data_pin = A5;
const byte hx711_clock_pin = A0;

Q2HX711 hx711(hx711_data_pin, hx711_clock_pin);

float offset = 0, time1 = 0, peso = 0;
float factor = 0.0444444 / 1000;
int i = 0;
int j = 0;
float temp = 16777215.00;
void setup() {
  Serial.begin(9600);
  time1=millis();
}



void loop() {
  peso = hx711.read();
  if (millis()-time1 > 500) {
    Serial.print("Peso: ");
    Serial.print(peso);
    Serial.println(" ");
    time1=millis();
  }
}


//
//void pesar() {
//  i++;
//  time1 = millis();
//  peso += hx711.read();
//  if (i > 20) {
//    peso = abs(((peso / i) - offset))*factor;  
//    i = 0;
//    Serial.print("Peso: ");
//    Serial.println(peso);
//    peso = 0;
//    j++;
//  }
//}
//
//void calibracion(){
//}
//
//void zero() {
//  for (i = 0; i < 10; i++) {
//    offset += hx711.read();
//    if (i % 2 == 0) {
//      Serial.println("ZERO...");
//    } else {
//      Serial.println("ZERO");
//    }
//  }
//  offset /= i;
//  i = 0;
//  Serial.println("ZERO: TERMINADO");
//}
