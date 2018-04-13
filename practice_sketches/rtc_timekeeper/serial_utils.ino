char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void serial_buffer_clear(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}   
