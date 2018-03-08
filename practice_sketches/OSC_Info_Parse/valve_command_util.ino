void valve_send_on() {
  
}

void valve_send_off() {
  
}

void prepare_bndl() {
  void get_OSC_bundle(char *string, OSCBundle* bndl) {
  bndl->empty();
  data_vals value_union;
  char buf[121];
  char *p = buf;
  char *addr = NULL, *value = NULL;
  strcpy(buf, string);
  addr = strtok_r(p, " ", &p);
  int count = 0;
  int elec_cond = 0;
  float vwc = 0;
  float temp = 0;
  while (addr != NULL & strlen(addr) > 0) {
    value = strtok_r(NULL, " ", &p);
    value_union.u = strtoul(&value[1], NULL, 0);
    if (value[0] == 'f') {
      bndl->add(addr).add(value_union.f);
      Serial.print("Address: ");
      Serial.println(addr);
      Serial.print("Value: ");
      Serial.println(value_union.f);
      if(count == 0){
         vwc = value_union.f;
         Serial.print("The VWC is: ");
         Serial.println(vwc);
      //publish to MQTT/adafruit
      }
      if(count == 1){
         temp = value_union.f;
         Serial.print("The Temperature in C is: ");
         Serial.println(temp);
      //publish to MQTT/Adafruit
      }
    }
    else if (value[0] == 'i') {
      bndl->add(addr).add(value_union.i);
      Serial.print("Address: ");
      Serial.println(addr);
      Serial.print("Value: ");
      Serial.println(value_union.i);

      elec_cond = value_union.i;
      Serial.print("The electrical conductivity is: ");
      Serial.println(elec_cond);
    }

    addr = strtok_r(p, " ", &p);
    count++;
  }
}

