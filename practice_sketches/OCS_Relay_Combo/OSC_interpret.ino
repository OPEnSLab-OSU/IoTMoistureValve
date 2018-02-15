#include <string.h>

union data_vals {
  int32_t i;
  float f;
  uint32_t u;
};

char* get_OSC_string(OSCBundle bndl) {
  char buf[50];
  char string[121];
  char type;
  int n = 0;
  data_vals value;
  OSCMessage* msg = bndl.getOSCMessage(n);

  memset(string, '\0', 121);
  
  while(msg != NULL) {
    msg->getAddress(buf, 0);
    type = msg->getType(0);
    
    /*Serial.print("Address ");
    Serial.print(n+1);
    Serial.print(": ");
    Serial.println(buf);*/

    strcat(string, buf);

    if (type == 'f') {
      value.f = msg->getFloat(0);
      /*Serial.print("Value ");
      Serial.print(n+1);
      Serial.print(": ");
      Serial.println(value);*/

      snprintf(buf, 50, " f%lu", value.u);
      strcat(string, buf);
    }
    else if (type == 'i') {
      value.i = msg->getInt(0);
      /*Serial.print("Value ");
      Serial.print(n+1);
      Serial.print(": ");
      Serial.println(value);*/

      snprintf(buf, 50, " i%lu", value.u);
      strcat(string, buf);
    }
    n++;
    msg = bndl.getOSCMessage(n);
    if (msg != NULL) strcat(string, " ");
  }

  char* ret_val = (char*)malloc((strlen(string) + 1) * sizeof(char));
  strcpy(ret_val, string);
  return ret_val;
}

void get_OSC_bundle(char *string, OSCBundle* bndl) {
  bndl->empty();
  data_vals value_union;
  char buf[121];
  char *p = buf;
  char *addr = NULL, *value = NULL;
  strcpy(buf, string);
  addr = strtok_r(p, " ", &p);
  while (addr != NULL & strlen(addr) > 0) {
    value = strtok_r(NULL, " ", &p);
    value_union.u = strtoul(&value[1], NULL, 0);
    if (value[0] == 'f') {
      bndl->add(addr).add(value_union.f);
      Serial.print("Address: ");
      Serial.println(addr);
      Serial.print("Value: ");
      Serial.println(value_union.f);
    }
    else if (value[0] == 'i') {
      bndl->add(addr).add(value_union.i);
      Serial.print("Address: ");
      Serial.println(addr);
      Serial.print("Value: ");
      Serial.println(value_union.i);
    }

    addr = strtok_r(p, " ", &p);
  }
}


//Check remaining memory between Heap and Stack
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__
 
int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}
