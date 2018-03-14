//Set digital logic pins for relay in main file.

//Opens valve with considerations for pressure relief.
void valve_open() {
  //Partial open sequence.
  digitalWrite(VALVE_PIN_ROT_OPEN, HIGH); 
  digitalWrite(VALVE_PIN_ROT_CLOSE, LOW);    
  delay(1 * HALF_SEC); //Partial open timing.

  //Pressure relief delay.
  valve_off();
  delay(2 * SECOND); //Set for time needed to relieve pressure.

  //Full open sequence.
  digitalWrite(VALVE_PIN_ROT_OPEN, HIGH); 
  digitalWrite(VALVE_PIN_ROT_CLOSE, LOW);    
  delay(3 * SECOND);  //Adust for time valve takes to full open from partial.

  //End sequence.
  valve_off();
}

//Close valve straight, no delays or pressure relief.
void valve_close() {
  digitalWrite(VALVE_PIN_ROT_OPEN, LOW); 
  digitalWrite(VALVE_PIN_ROT_CLOSE, HIGH);    
  delay(3 * SECOND); //Adjust for time valve takes to close.

  valve_off();
}

//Sets both pins to low state.
void valve_off() {
  digitalWrite(VALVE_PIN_ROT_OPEN, LOW);
  digitalWrite(VALVE_PIN_ROT_CLOSE, LOW);
}
