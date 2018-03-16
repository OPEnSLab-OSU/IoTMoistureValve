#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <OSCBundle.h>
#include "SDI12.h"
#include <FlashStorage.h>

//For relay shields and valve control.
#define VALVE_PIN_ROT_OPEN  10 //Near relay
#define VALVE_PIN_ROT_CLOSE  11 //Far relay

//Pins for data.
#define DATAPIN 12
#define SENSOR_ADDRESS '?' //Needs to change if running multiple sensors on single data-pin.

//M0 LoRa radio pins.
#define RFM95_CS  8
#define RFM95_RST 4
#define RFM95_INT 3 //Change to 7 for 32u4 board.

//Addresses for RHReliableDatagram (Hardcoded for now?)
#define HUB_ADDRESS   30
#define RELAY_ADDRESS 31

//Time increments
#define SECOND    1000
#define HALF_SEC  500
#define TENTH_SEC 100

//Delay (spoof wake-up) time.
#define WAIT 20

//Size of message for LoRa
#define MSG_SIZE RH_RF95_MAX_MESSAGE_LEN

//IDString constructor
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x) //to concatenate a predefined number to a string literal, use STR(x)

#define FAMILY "/LOOM"
#define THIS_DEVICE "/IrrVal/"
#define HUB_DEVICE "/Hub/"
#define INSTANCE_NUM 0  // Unique instance number for this device, useful when using more than one of the same device type in same space

#define MyIDString FAMILY THIS_DEVICE STR(INSTANCE_NUM) // C interprets subsequent string literals as concatenation: "/Loom" "/Ishield" "0" becomes "/Loom/Ishield0"
#define HubIDString FAMILY HUB_DEVICE STR(INSTANCE_NUM) // To read from Hub.

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

//For checking battery voltage of M0.
//https://learn.adafruit.com/adafruit-feather-m0-radio-with-lora-radio-module?view=all#power-management
#define VBATPIN A7

//Struct for storage of recieved instructions.
typedef struct {
  boolean valid;
  int mode;
  float vwc_low;
  float vwc_high;
  unsigned long start;
  unsigned long dur;
} Trigger_Vals;

//Setup Pin-read
SDI12 mySDI12(DATAPIN);

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

RHReliableDatagram manager(rf95, RELAY_ADDRESS);

uint8_t inst_buf[RH_RF95_MAX_MESSAGE_LEN];

FlashStorage(trigger_flash_store, Trigger_Vals);

Trigger_Vals trig_vals;

float last_batt_val;

void setup() {
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  //Setup pins for Valve-control
  pinMode(VALVE_PIN_ROT_OPEN, OUTPUT);
  pinMode(VALVE_PIN_ROT_CLOSE, OUTPUT);

  while (!Serial); //Comment or remove when not tethering to PC.
  Serial.begin(9600);

  delay(1000);

  Serial.println("---- BEGIN SETUP----");
  mySDI12.begin(); //Init SDI12 object.
  delay(2000);

  trig_vals = trigger_flash_store.read();

  if (trig_vals.valid == false) {
    Serial.println("No stored values, setting defaults.");

    int mode;
    float vwc_low;
    float vwc_high;
    unsigned long start;
    unsigned long dur;

    //DEBUG
    trig_vals.mode = 1;
    trig_vals.vwc_low = 10.15;
    trig_vals.vwc_high = 25.53;
    trig_vals.start = 2000;
    trig_vals.dur = 5000;
    trig_vals.valid = true;

    trigger_flash_store.write(trig_vals);

  } else {
    Serial.println("Got stored values -");
    Serial.print("Mode: "); Serial.print(trig_vals.mode); Serial.print(" ");
    Serial.print("VWC Lower: "); Serial.print(trig_vals.vwc_low); Serial.print(" ");
    Serial.print("VWC Upper: "); Serial.print(trig_vals.vwc_high); Serial.print(" ");
    Serial.print("Start Time: "); Serial.print(trig_vals.start); Serial.print(" ");
    Serial.print("Duration: "); Serial.println(trig_vals.dur);
  }

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

  Serial.print("Num retries: "); Serial.print(manager.retries()); Serial.print(" for ");
  Serial.print(HALF_SEC); Serial.println("ms per try.");
  last_batt_val = check_batt();
  Serial.println("---- END SETUP---- \n\n");
}

//Setup for sensor read.
String sdiResponse = "";
String resp_buf = "";
String myCommand = "";
char delimit[] = "+-"; // Delimiters for response parsing.
float VWC, temp;
int elec;

int count = 0; //Testing loop counter.

OSCBundle bndl;

void loop() {

  Serial.println("----- BEGIN RUN -----");
  Serial.print("Run: #"); Serial.println(++count);

  last_batt_val = check_batt();

  sdiResponse = "";
  resp_buf = "";
  myCommand = "";

  //-----------------------------
  //----- Begin sensor read -----
  //-----------------------------

  //Take a measurement
  myCommand = String(SENSOR_ADDRESS) + "M!";
  Serial.print(myCommand); Serial.print(": "); //DEBUG echo command to terminal

  mySDI12.sendCommand(myCommand);
  delay(30); // wait a while for a response

  // build response string
  while (mySDI12.available()) {
    char c = mySDI12.read();
    if ((c != '\n') && (c != '\r')) {
      sdiResponse += c;
      delay(5);
    }
  }
  if (sdiResponse.length() > 1)
    Serial.println(sdiResponse); //write the response to the screen
  mySDI12.clearBuffer();


  delay(1000); // delay between taking reading and requesting data

  //Clear bufs
  sdiResponse = ""; // clear the response string
  resp_buf = "";

  //Set garbage values.
  VWC = 99999.9;
  temp = 99999.9;
  elec = 99999;

  //Request data from last measurement
  myCommand = String(SENSOR_ADDRESS) + "D0!";
  Serial.print(myCommand); Serial.print(": "); //DEBUG echo command to terminal

  mySDI12.sendCommand(myCommand);
  delay(30); // wait a while for a response

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

  bndl.add(MyIDString "/VWC").add((float)VWC);
  //  Serial.print("Add VWC ");
  bndl.add(MyIDString "/temp").add((float)temp);
  //  Serial.print("Add Temp ");
  bndl.add(MyIDString "/ElecCond").add((int32_t)elec);
  //  Serial.println("Add EC");
  bndl.add(MyIDString "/vbat").add((float)last_batt_val);
  //  Serial.println("Add VBat");

  char message[MSG_SIZE];

  memset(message, '\0', MSG_SIZE);
  get_OSC_string(&bndl, message);

  Serial.println(message);
  Serial.print("Message length: "); Serial.println(strlen(message));
  Serial.print("Max message length: "); Serial.println(RH_RF95_MAX_MESSAGE_LEN);

  //delay(2000);

  Serial.print("Sending...");
  if (manager.sendtoWait((uint8_t*)message, strlen(message), HUB_ADDRESS)) {
    Serial.println("ok, listening for reply instructions.");

    unsigned long lora_timer = millis();
    while (!manager.available() && (millis() - lora_timer < 1000)) {}


    //TODO ADD TYPE CHECKING FOR RECIEVED.
    if (manager.available()) {
      uint8_t len = sizeof(inst_buf);
      //Serial.println(len); //DEBUG
      uint8_t from;
      memset(inst_buf, '\0', RH_RF95_MAX_MESSAGE_LEN);

      if (manager.recvfromAck(inst_buf, &len, &from)) {
        if (from == HUB_ADDRESS) {
          //DEBUG
          Serial.println((char*) inst_buf);

          //OSCBundle inst_bndl;

          //bndl.empty();

          get_OSC_bundle((char*)inst_buf, &bndl);

          //TODO Change from hardcoded to While loop.
          //TODO Check if actual value contained or crashes. Use GetType()?
          Serial.print((int)bndl.getOSCMessage(HubIDString "/mode_inst")->getInt(0)); Serial.print(" ");
          trig_vals.mode = (int)bndl.getOSCMessage(HubIDString "/mode_inst")->getInt(0);

          Serial.print((float)bndl.getOSCMessage(HubIDString "/vwc_low_inst")->getFloat(0)); Serial.print(" ");
          trig_vals.vwc_low = (float)bndl.getOSCMessage(HubIDString "/vwc_low_inst")->getFloat(0);

          Serial.print((float)bndl.getOSCMessage(HubIDString "/vwc_high_inst")->getFloat(0)); Serial.print(" ");
          trig_vals.vwc_high = (float)bndl.getOSCMessage(HubIDString "/vwc_high_inst")->getFloat(0);

          Serial.print((int32_t)bndl.getOSCMessage(HubIDString "/start_inst")->getInt(0)); Serial.print(" ");
          trig_vals.start = ((unsigned long)bndl.getOSCMessage(HubIDString "/start_inst")->getInt(0));

          Serial.println((int32_t)bndl.getOSCMessage(HubIDString "/dur_inst")->getInt(0));
          trig_vals.dur = ((unsigned long)bndl.getOSCMessage(HubIDString "/dur_inst")->getInt(0));

          trigger_flash_store.write(trig_vals);

          //DEBUG Close valve.

          //inst_bndl.empty();
        }
      }
    }
  } else {
    Serial.println("failed");
  }

  //DEBUG
  Serial.print("Stored instructions: ");
  Serial.print(trig_vals.mode); Serial.print(" ");
  Serial.print(trig_vals.vwc_low); Serial.print(" ");
  Serial.print(trig_vals.vwc_high); Serial.print(" ");
  Serial.print(trig_vals.start); Serial.print(" ");
  Serial.println(trig_vals.dur);


  switch (trig_vals.mode) {
    case 1:
      Serial.println("Timer mode.");

      if (trig_vals.start == 0 && trig_vals.dur > 0) {
        //DEBUG
        Serial.print("No time offset, starting watering for "); Serial.print(trig_vals.dur); Serial.println(" seconds.");

        valve_open();
        delay(trig_vals.dur);
        trig_vals.dur = 0;
        trigger_flash_store.write(trig_vals);
        valve_close();

      } else if (trig_vals.start > 0 && trig_vals.dur > 0) {
        Serial.print("Time offset for "); Serial.print(trig_vals.start); Serial.println(" seconds, sleeping.");

        delay(trig_vals.start);

        Serial.print("Awake, starting watering for "); Serial.print(trig_vals.dur); Serial.println(" seconds.");

        valve_open();
        delay(trig_vals.dur);
        trig_vals.dur = 0;
        trigger_flash_store.write(trig_vals);
        valve_close();

      } else {
        //TODO Send warning to user...?
        Serial.println("ERROR: Bad formatting of date and/or timer.");
      }
      break;
    case 2:
      Serial.println("VWC mode.");

      //TODO Take readings and water while low > vwc, and stop when vwc > high.

      break;
    case 3:
      Serial.println("Combined mode.");

      break;
    default:
      Serial.println("Fell off Switch statement. You shouldn't be here.");
      // statements
  }

  //TODO Replace with sleep code.
  Serial.print("Waiting for "); Serial.print(WAIT); Serial.println(" seconds.");
  Serial.println("----- END RUN -----\n\n");
  delay(WAIT * SECOND);


}
