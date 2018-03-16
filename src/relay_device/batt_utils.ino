//Checks status of attached battery or power source.
//https://learn.adafruit.com/adafruit-feather-m0-radio-with-lora-radio-module?view=all#power-management
float check_batt() {

  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  Serial.print("VBat: " ); Serial.println(measuredvbat);

  return measuredvbat;
}

