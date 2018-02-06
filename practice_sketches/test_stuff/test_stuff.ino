#include "SDI12.h"

#include "SDI12.h"
#include <string.h>

#define DATAPIN 11        // change to the proper pin
#define LED_PIN 13
#define SENSOR_ADDRESS "?"

SDI12 mySDI12(DATAPIN);


String sdiResponse = "";
String myCommand = "";
char buf[20];
char *p;

void setup() {
  Serial.begin(9600);
  mySDI12.begin();
  deca_gs3_setup();

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
 do {                           // wait for a response from the serial terminal to do anything
    delay (30);
  }
  while (!Serial.available());
  
  char nogo = Serial.read();
  
  if(nogo == 'i'){
    Serial.println("Info:");
    deca_gs3_setup();
  } else if(nogo == 'm'){
    Serial.println("Data:");
    measure_decagon();
  }
}
