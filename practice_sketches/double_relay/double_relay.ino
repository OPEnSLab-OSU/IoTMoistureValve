


const int VALVE_PIN_N_ON   9; //Far relay
const int VALVE_PIN_N_OFF 10; //Near relay

void setup() {  
  pinMode(VALVE_PIN_N_ON, OUTPUT);
  pinmode(VALVE_PIN_N_OFF, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {


  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

  
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

}
