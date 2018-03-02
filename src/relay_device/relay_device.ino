#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <OSCBundle.h>
#include "SDI12.h"
//#include <MemoryFree.h>

#define VALVE_PIN_ROT_OPEN  9 //Near relay
#define VALVE_PIN_ROT_CLOSE  10 //Far relay

//Pins for data.
#define DATAPIN 12         
#define SENSOR_ADDRESS '?' //Needs to change if running multiple sensors on single data-pin.

//M0 LoRa radio pins.
#define RFM95_CS  8
#define RFM95_RST 4
#define RFM95_INT 3 //Change to 7 for 32u4 board.

//Addresses for RHReliableDatagram (Hardcoded for now?)
#define HUB_ADDRESS   1
#define RELAY_ADDRESS 2

//Time increments
#define SECOND    1000
#define HALF_SEC  500
#define TENTH_SEC 100

//Size of message for LoRa
#define MSG_SIZE 121

//Delay (spoof wake-up) time.
#define WAIT 8

//IDString constructor
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x) //to concatenate a predefined number to a string literal, use STR(x)

#define FAMILY "/LOOM"
#define DEVICE "/IrrVal/"
#define INSTANCE_NUM 0  // Unique instance number for this device, useful when using more than one of the same device type in same space

#define IDString FAMILY DEVICE STR(INSTANCE_NUM) // C interprets subsequent string literals as concatenation: "/Loom" "/Ishield" "0" becomes "/Loom/Ishield0"

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

//Setup Pin-read
SDI12 mySDI12(DATAPIN);

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

RHReliableDatagram manager(rf95, RELAY_ADDRESS);

void setup() {
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  
  //Setup pins for Valve-control
  pinMode(VALVE_PIN_ROT_OPEN, OUTPUT);
  pinMode(VALVE_PIN_ROT_CLOSE, OUTPUT);
  
  
  while(!Serial); //Comment or remove when not tethering to PC.
  Serial.begin(9600);
  mySDI12.begin(); //Init SDI12 object.
  delay(2000);

  Serial.println("Initializing manager...");
  if (!manager.init())
    Serial.println("init failed");

  Serial.println("Setting Frequency...");
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.println("Setting power...");
  rf95.setTxPower(23, false);


  manager.setRetries(10);
  manager.setTimeout(HALF_SEC);

  Serial.print("Num retries: ");
  Serial.println(manager.retries());

  //Serial.print("Mem after setup - "); Serial.println(freeMemory());
}

//Setup for sensor read.
String sdiResponse = "";
String resp_buf = "";
String myCommand = "";
char delimit[]="+-"; // Delimiters for responce parsing.
float VWC, temp;
int elec;

int count = 0; //Testing loop counter.

OSCBundle bndl;

void loop() {
  
//  Serial.print("freeMemory()=");
//  Serial.println(freeMemory());
//
//  delay(1000);

  Serial.print("Beginning run: #"); Serial.println(++count);

  sdiResponse = "";
  resp_buf = "";
  myCommand = "";

//-----------------------------
//----- Begin sensor read -----
//-----------------------------

//first command to take a measurement
  myCommand = String(SENSOR_ADDRESS) + "M!";
  Serial.println(myCommand);     // echo command to terminal

  mySDI12.sendCommand(myCommand);
  delay(30);                     // wait a while for a response

  while (mySDI12.available()) {  // build response string
    char c = mySDI12.read();
    if ((c != '\n') && (c != '\r')) {
      sdiResponse += c;
      delay(5);
    }
  }
  if (sdiResponse.length() > 1) Serial.println(sdiResponse); //write the response to the screen
  mySDI12.clearBuffer();


  delay(1000);                 // delay between taking reading and requesting data
  sdiResponse = "";           // clear the response string
  resp_buf = "";
  VWC = 99999.9;
  temp = 99999.9;
  elec = 99999;

// next command to request data from last measurement
  myCommand = String(SENSOR_ADDRESS) + "D0!";
  Serial.println(myCommand);  // echo command to terminal

  mySDI12.sendCommand(myCommand);
  delay(30);                     // wait a while for a response

  while (mySDI12.available()) {  // build string from response
    char c = mySDI12.read();
    if ((c != '\n') && (c != '\r')) {
      sdiResponse += c;
      delay(5);
    }
  }
  if (sdiResponse.length() > 1) {
    Serial.println(sdiResponse); //write the response to the screen

    char * pch;
    char * buf = strdup(sdiResponse.c_str());
    pch = strtok (buf, delimit);// Burn sensor number.
    
    pch = strtok (NULL, delimit);  // VWC value
    VWC = atof(pch);
    
    pch = strtok (NULL, delimit);  // Temp value
    temp = atof(pch);
        
    pch = strtok (NULL, delimit);  // ElecCond value
    elec = atoi(pch);
  }
  Serial.print("VWC: "); Serial.print(VWC);
  Serial.print(" Temp: "); Serial.print(temp);
  Serial.print(" ElecCond: "); Serial.println(elec);
  
  mySDI12.clearBuffer();
//-----------------------------
//------ End sensor read ------
//-----------------------------



  //------ Package read data ------

  bndl.empty();
  
  bndl.add(IDString "/VWC").add((float)VWC);
  Serial.print("Add VWC ");
  bndl.add(IDString "/temp").add((float)temp);
  Serial.print("Add Temp ");
  bndl.add(IDString "/ElecCond").add((int32_t)elec);
  Serial.println("Add EC");
  
  char message[MSG_SIZE];

  memset(message, '\0', MSG_SIZE);
  
  get_OSC_string(&bndl, message);

  //Serial.println(freeMemory());  

  Serial.println(message);
  Serial.print("Message length: ");
  Serial.println(strlen(message));
  Serial.print("Max message length: ");
  Serial.println(RH_RF95_MAX_MESSAGE_LEN);

  delay(2000);
  
  Serial.print("Sending...");
  if (manager.sendtoWait((uint8_t*)message, strlen(message), HUB_ADDRESS)){
    Serial.println("ok, listening for reply instructions.");

    
  } else {
    Serial.println("failed");
  }
  
//  Serial.print("freeMemory()=");
//  Serial.println(freeMemory());
//
//  delay(1000);


  Serial.println("----- END RUN -----";
  Serial.println();
  Serial.println();
  
  delay(WAIT * SECOND);

  
}
