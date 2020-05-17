// INTERRUPCION PARA ENCODER FUNCINANDO CON CONTROL DE RUIDO EN LECTURA

const byte ledPin = 13;
const byte interruptPin = 2;
volatile byte state = LOW;
unsigned long time1,time2;
bool p=false,f=true;
int cont=0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, FALLING);
}

void loop() {
  digitalWrite(ledPin, state);
  
  interrupcion();
}

void interrupcion(){
   if(p){
   state = !state;
   p=false;    
   cont++;
   Serial.print("Contador: ");
   Serial.println(cont);
   }
   time2=millis();
   if(time2-time1>100){
    f=true;
   }
}

void blink() {
    if(f){
      f=false;
      p=true;
      time1=millis();     
    }
}
