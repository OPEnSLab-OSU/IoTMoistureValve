//Set digital logic pins for relay in main file.

#define START_VALVE_OPEN 1 //In SECOND increments.
#define OPEN_RELIEF_TIMER 1 //In HALF_SEC increments.
#define FINISH_VALVE_OPEN 2 //In SECOND increments.

#define START_VALVE_CLOSE 2 //In SECOND increments.
#define CLOSE_RELIEF_TIMER 1 //In HALF_SEC increments.
#define FINISH_VALVE_CLOSE 1 //In SECOND increments.

//Full action, no relief pause.
#define NO_STOP 3 //In SECOND increments.

//Opens valve with considerations for pressure relief.
void valve_open() {
  //Partial open sequence.
  digitalWrite(VALVE_PIN_ROT_OPEN, HIGH); 
  digitalWrite(VALVE_PIN_ROT_CLOSE, LOW);    
  delay(START_VALVE_OPEN * HALF_SEC); //Partial open timing.

  //Pressure relief delay.
  valve_off();
  delay(OPEN_RELIEF_TIMER * SECOND); //Set for time needed to relieve pressure.

  //Full open sequence.
  digitalWrite(VALVE_PIN_ROT_OPEN, HIGH); 
  digitalWrite(VALVE_PIN_ROT_CLOSE, LOW);    
  delay(FINISH_VALVE_OPEN * SECOND);  //Adust for time valve takes to full open from partial.

  //End sequence.
  valve_off();
}

//Close valve straight, no delays or pressure relief.
void valve_close() {
  digitalWrite(VALVE_PIN_ROT_OPEN, LOW); 
  digitalWrite(VALVE_PIN_ROT_CLOSE, HIGH);    
  delay(NO_STOP * SECOND); //Adjust for time valve takes to close.

  valve_off();
}

//Sets both pins to low state.
void valve_off() {
  digitalWrite(VALVE_PIN_ROT_OPEN, LOW);
  digitalWrite(VALVE_PIN_ROT_CLOSE, LOW);
}
