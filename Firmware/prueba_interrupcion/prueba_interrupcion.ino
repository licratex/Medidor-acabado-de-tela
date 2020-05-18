// INTERRUPCION PARA ENCODER FUNCINANDO CON CONTROL DE RUIDO EN LECTURA

// PINS
const byte ledPin       = 13;   // pin de led solo para monitorear
const byte interruptPin = 2;    // pin al que esta conectada la interrupcion
const byte zeroPin      = 3;    // boton setear a cero la longiud y el peso

volatile byte state = LOW;      // para el led de la board
unsigned long time1, time2;     // variables para guardar tiempo de ejecucion y eliminar lecuras falsas
bool p = false, f = true;       // bandeas para controlar y eliminar lecturas falsas
const float largo_radio = 0.06; // perimetro entre un radio y otro
float largo = 0;                // variable global para acumular el largo
float peso = 0;                 // variable global para acumular el peso

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(zeroPin, INPUT_PULLUP);                                           // activar pullup interno para el boton de tare/zero
  pinMode(interruptPin, INPUT_PULLUP);                                      // activar pullup interno de la interrupcion
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, FALLING);     // definir pin de interrupcion, nombre de la interrupcion, trigger de la interrupccion
}

void loop() {
  digitalWrite(ledPin, state);
  interrupcion();
  zero();
  Serial.print("Largo: ");     // se envia por serial la informacion, probablementese cambie al loop principal
  Serial.println(largo);
}

void zero() { // funcion que lleva todas las variables a zero, sirve para calibrar la balanza y el odometro a zero
  if (!digitalRead(zeroPin)) {
    largo = 0;
    peso = 0;
    delay(100);
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
