


#define VALVE_PIN_ROT_OPEN   9 //Far relay
#define VALVE_PIN_ROT_CLOSE 10 //Near relay

void setup() {  
  pinMode(VALVE_PIN_ROT_OPEN, OUTPUT);
  pinMode(VALVE_PIN_ROT_CLOSE, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {


  digitalWrite(VALVE_PIN_ROT_OPEN, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(VALVE_PIN_ROT_CLOSE, LOW);    // turn the LED off by making the voltage LOW
  delay(2000);                       // wait for a second

  
  digitalWrite(VALVE_PIN_ROT_OPEN, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(VALVE_PIN_ROT_CLOSE, HIGH);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

}
