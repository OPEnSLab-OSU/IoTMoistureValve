/***********************************
 * Author: Peter Dorich 
 * Based off code from OSC by Kenny Noble 
 * Back-end for send/ recieve data between hub/ valve
 * NEW FUNCTIONALITY 2/15/2018: Added the string VWC and elec_conductivity and Temp to
 *      their own variables
 ************************************/

#include <string.h>

union data_vals {
  int32_t i;
  float f;
  uint32_t u;
};

void get_OSC_string(OSCBundle *bndl, char *string) {
  char buf[50];
  char type;
  int n = 0;
  data_vals value;
  OSCMessage* msg = bndl->getOSCMessage(n);
  
  while(msg != NULL) {
    msg->getAddress(buf, 0);
    type = msg->getType(0);
    
    /*
    Serial.print("Address ");
    Serial.print(n+1);
    Serial.print(": ");
    Serial.println(buf);
    */

    strcat(string, buf);

    if (type == 'f') {
      value.f = msg->getFloat(0);
      /*
      Serial.print("Value ");
      Serial.print(n+1);
      Serial.print(": ");
      Serial.println(value.f);*/
      
      snprintf(buf, 50, " f%lu", value.u);
      strcat(string, buf);
    }
    else if (type == 'i') {
      value.i = msg->getInt(0);
      /*Serial.print("Value ");
      Serial.print(n+1);
      Serial.print(": ");
      Serial.println(value.i);*/
      
      snprintf(buf, 50, " i%lu", value.u);
      strcat(string, buf);
    }
    n++;
    msg = bndl->getOSCMessage(n);
    if (msg != NULL) strcat(string, " ");
  }
}

struct soil_data get_OSC_bundle(char *string, OSCBundle* bndl) {
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
//      Serial.print("Address: ");
//      Serial.println(addr);
//      Serial.print("Value: ");
//      Serial.println(value_union.f);
      if(count == 0){
         vwc = value_union.f;
         Serial.print("The VWC is: ");
         Serial.println(vwc);
         s_dat.VWC = vwc;
      //publish to MQTT/adafruit
      }
      if(count == 1){
         temp = value_union.f;
         Serial.print("The Temperature in C is: ");
         Serial.println(temp);
         s_dat.TEMP = temp;
      }
    }
    else if (value[0] == 'i') {
      bndl->add(addr).add(value_union.i);
//      Serial.print("Address: ");
//      Serial.println(addr);
//      Serial.print("Value: ");
//      Serial.println(value_union.i);

      elec_cond = value_union.i;
      Serial.print("The electrical conductivity is: ");
      Serial.println(elec_cond);
      s_dat.ELEC_COND = elec_cond;
    }

    addr = strtok_r(p, " ", &p);
    count++;
    
  }
  return s_dat;
}


