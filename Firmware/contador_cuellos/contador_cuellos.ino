// INTERRUPCION PARA ENCODER FUNCINANDO CON CONTROL DE RUIDO EN LECTURA

// PINS
const byte ledPin       = 13;   // pin de led solo para monitorear
const byte interruptPin = 2;    // pin al que esta conectada la interrupcion
const byte zeroPin      = 3;    // boton setear a cero la longiud y el peso
const byte selector     = 4;    // boton setear a cero la longiud y el peso
const int bounce     = 100;    // retardo para evitar rebote en lectura de interrupcion, entre mayor mas estable, pero si el tiempo es muy largo podria dejar pasar objetos sin detectar

volatile byte state = LOW;      // para el led de la board
unsigned long time1, time2;     // variables para guardar tiempo de ejecucion y eliminar lecuras falsas
bool p = false, f = true;       // bandeas para controlar y eliminar lecturas falsas
const float largo_radio = 0.06; // perimetro entre un radio y otro
int cantidad= 0;                // variable global para acumular el largo
int tipo=1;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(zeroPin, INPUT_PULLUP);                                           // activar pullup interno para el boton de tare/zero
  pinMode(selector, INPUT_PULLUP);                                           // activar pullup interno para el boton de tare/zero
  pinMode(interruptPin, INPUT_PULLUP);                                      // activar pullup interno de la interrupcion
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, FALLING);     // definir pin de interrupcion, nombre de la interrupcion, trigger de la interrupccion

}

void loop() {
  interrupcion();
  seleccion();
  zero();
  pantalla();
}

void pantalla(){
  int c;
  switch(tipo){
    case 1:
    Serial.print("(1) Cuellos: ");     // se envia por serial la informacion, probablementese cambie al loop principal
    c=cantidad;
    break;
    case 2:
    Serial.print("(2) Tiras: ");     // se envia por serial la informacion, probablementese cambie al loop principal
    c=cantidad/2;
    break;
    case 3:
    Serial.print("(3) Combos: ");     // se envia por serial la informacion, probablementese cambie al loop principal
    c=cantidad/3;
    break;
  }
  Serial.println(c);  
}

void seleccion(){
    if(!digitalRead(selector)){
    tipo++;
    if(tipo>3){
      tipo=1;
    }
    delay(100);
    while(!digitalRead(selector)){
      delay(100);
    }
  }
}

void zero() { // funcion que lleva todas las variables a zero, sirve para calibrar la balanza y el odometro a zero
  if (!digitalRead(zeroPin)) {
    cantidad = 0;
    delay(100);
  }
}

void interrupcion() {
  if (p) {
    state = !state;
    p = false;
    cantidad++;
    digitalWrite(ledPin, state);  
  }
  time2 = millis();            // las lineas siguiente controlan el tiempo que ocaciona rebote o lecturas en falso.
  if (time2 - time1 > bounce) {
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
