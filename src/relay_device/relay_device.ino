#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <OSCBundle.h>
#include "SDI12.h"
#include <FlashStorage.h>
#include <RTClibExtended.h>
#include <Wire.h>

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

int      mode_inst;
float    vwc_low_inst;
float    vwc_high_inst;
uint16_t start_inst;
uint16_t dur_inst;
uint16_t sleep_inst;

//Simple enumator for valve state values.
enum class ValveState {CLOSED = 0, OPEN = 1};

//Struct for storage of received instructions.
typedef struct {
  boolean       valid;        // Is flash memory still good?
  boolean       started;      // Irrigation has started.
  ValveState    valve;        // Valve open or closed?
  DateTime      valve_change; // Time to next valve open/close.
  int           mode;         // Mode of operation (1-Timer, 2-VWC, 3-combined)
  float         vwc_low;      // VWC low threshold to open valve in VWC/combined
  float         vwc_high;     // VWC high threshold to close valve in VWC/combined
  uint16_t      start;        // Time offset from inst received to begin watering.
  uint16_t      dur;          // Length of time to water for.
  uint16_t      sleep;        // Data read/upload alarm length.
  unsigned long start_unix;   // Time offset in .unixtime() seconds
  unsigned long dur_unix;     // Length of time to water in .unixtime() seconds
  unsigned long sleep_unix;   // Data read/upload alarm length in .unixtime() seconds.
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

//Bleh
RTC_DS3231 RTC_DS;

//  =================================================================================
//  =====! Alarm type not currently implemented, only interval wake currently. !=====
//  =================================================================================

// Declare/init RTC_DS variables//
volatile bool doWakeRoutine = false; // Flag is set with external pin interrupt by RTC using wake() method in rtc_utils.
volatile int HR = 8;                  // Hours    --- Use this for daily alarm implementation.
volatile int MIN = 0;                 // Minutes  --- Use this for daily alarm implementation.
volatile int WakePeriodMin = 1;       // Period of time to take sample in Min, reset alarm based on this period (Bo - 5 min)
const byte wakeUpPin = 13;

boolean first_run;

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

  InitalizeRTC();

  if (trig_vals.valid == false) {
    Serial.println("No stored values, setting defaults.");

    DateTime temp_time = RTC_DS.now();

    //Get good defaults from client
    trig_vals.mode = 1;
    trig_vals.vwc_low = 10.15;
    trig_vals.vwc_high = 25.53;
    trig_vals.start = 0;
    trig_vals.dur = 2;
    trig_vals.sleep = 1;
    trig_vals.start_unix  = (temp_time.unixtime() + trig_vals.start * 60UL);
    trig_vals.dur_unix    = (temp_time.unixtime() + trig_vals.dur * 60UL);
    trig_vals.sleep_unix  = (temp_time.unixtime() + trig_vals.sleep * 60UL);
    trig_vals.valve = ValveState::CLOSED;
    trig_vals.valid = true;

    trigger_flash_store.write(trig_vals);
  } else {
    Serial.println("Got stored values -");
    Serial.print("Mode: "); Serial.print(trig_vals.mode); Serial.print(" ");
    Serial.print("VWC Lower: "); Serial.print(trig_vals.vwc_low); Serial.print(" ");
    Serial.print("VWC Upper: "); Serial.print(trig_vals.vwc_high); Serial.print(" ");
    Serial.print("Start Time: "); Serial.print(trig_vals.start); Serial.print(" ");
    Serial.print("Duration: "); Serial.print(trig_vals.dur);
    Serial.print("Sleep time: "); Serial.println(trig_vals.sleep);
  }

  Serial.println("RTC defined.");
  pinMode(wakeUpPin, INPUT_PULLUP);

  Serial.println("M0 defined.");
  attachInterrupt(wakeUpPin, wake, FALLING);

  delay(500);

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

  doWakeRoutine = true;
  first_run = true;
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

boolean new_instructions;

void loop() {

  // TODO Sleep here if sleep operates correctly.
  //.goToSleep();

  if (doWakeRoutine) {

    detachInterrupt(digitalPinToInterrupt(wakeUpPin));

    //Serial.println("I woke up.");
    //clearTargetAlarm(1); // Clear RTC Alarm

    DateTime time_now = RTC_DS.now();

    if (first_run) {
      first_run = false; // Remove?
    }

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
      // TODO Change timer to variable value? Not 1000 arbitrary.

      // TODO ADD TYPE CHECKING FOR RECEIVED.
      if (manager.available()) {
        uint8_t len = sizeof(inst_buf);
        //Serial.println(len); //DEBUG
        uint8_t from;
        memset(inst_buf, '\0', MSG_SIZE);

        if (manager.recvfromAck(inst_buf, &len, &from)) {
          if (from == HUB_ADDRESS) {
            //DEBUG
            Serial.println((char*) inst_buf);

            get_OSC_bundle((char*)inst_buf, &bndl);

            //TODO Change from hardcoded to While loop.
            //TODO Check if actual value contained or crashes. Use GetType()?

            //Serial.print((int)bndl.getOSCMessage(HubIDString "/mode_inst")->getInt(0)); Serial.print(" ");
            mode_inst = (int)bndl.getOSCMessage(HubIDString "/mode_inst")->getInt(0);

            //Serial.print((float)bndl.getOSCMessage(HubIDString "/vwc_low_inst")->getFloat(0)); Serial.print(" ");
            vwc_low_inst = (float)bndl.getOSCMessage(HubIDString "/vwc_low_inst")->getFloat(0);

            //Serial.print((float)bndl.getOSCMessage(HubIDString "/vwc_high_inst")->getFloat(0)); Serial.print(" ");
            vwc_high_inst = (float)bndl.getOSCMessage(HubIDString "/vwc_high_inst")->getFloat(0);

            //Serial.print((int32_t)bndl.getOSCMessage(HubIDString "/start_inst")->getInt(0)); Serial.print(" ");
            start_inst = ((unsigned long)bndl.getOSCMessage(HubIDString "/start_inst")->getInt(0));

            //Serial.print((int32_t)bndl.getOSCMessage(HubIDString "/dur_inst")->getInt(0)); Serial.print(" ");
            dur_inst = ((unsigned long)bndl.getOSCMessage(HubIDString "/dur_inst")->getInt(0));

            //Serial.println((int32_t)bndl.getOSCMessage(HubIDString "/sleep_inst")->getInt(0));
            sleep_inst = ((unsigned long)bndl.getOSCMessage(HubIDString "/sleep_inst")->getInt(0));

            delay(250);

            new_instructions = true;
          }
        }
      } else {
        Serial.println("Nothing available on LoRa. Continuing with stored values.");
      }
    } else {
      Serial.println("failed.");
    }

    if (new_instructions) {
      //Serial.print("mode_inst read: "); Serial.println(mode_inst);
      if ((1 <= mode_inst) && (mode_inst <= 3)) {

        trig_vals.mode        = mode_inst;

        trig_vals.vwc_low     = vwc_low_inst;
        trig_vals.vwc_high    = vwc_high_inst;

        trig_vals.start       = start_inst;
        trig_vals.dur         = dur_inst;
        trig_vals.sleep       = sleep_inst;

        trig_vals.start_unix  = (time_now.unixtime() + start_inst * 60UL);
        trig_vals.dur_unix    = (time_now.unixtime() + dur_inst * 60UL);
        trig_vals.sleep_unix  = (time_now.unixtime() + sleep_inst * 60UL);

        Serial.println(trig_vals.start_unix);
        Serial.println(trig_vals.dur_unix);
        Serial.println(trig_vals.sleep_unix);

        trigger_flash_store.write(trig_vals);
      } else {
        Serial.println("ERROR: Received data was badly formatted.");
        //TODO Send notification to user?
      }
    }
    //DEBUG
    Serial.print("Stored instructions: ");
    Serial.print(trig_vals.mode); Serial.print(" ");
    Serial.print(trig_vals.vwc_low); Serial.print(" ");
    Serial.print(trig_vals.vwc_high); Serial.print(" ");
    Serial.print(trig_vals.start); Serial.print(" ");
    Serial.print(trig_vals.dur); Serial.print(" ");
    Serial.print(trig_vals.sleep); Serial.print(" ");
    Serial.print(trig_vals.start_unix); Serial.print(" ");
    Serial.print(trig_vals.dur_unix); Serial.print(" ");
    Serial.println(trig_vals.sleep_unix);

    Serial.println(valveStateCheck());

    if (trig_vals.valve == ValveState::CLOSED) {
      switch (trig_vals.mode) {
        case 1:
          Serial.println("Timer mode.");

          if (trig_vals.start > 0) {
            Serial.println("Waiting...");
            break;
          } else if ((trig_vals.start <= 0) && (trig_vals.dur > 0)) {
            Serial.print("Watering for "); Serial.print(trig_vals.dur); Serial.println(" minutes.");

            valve_open();
            trig_vals.valve = ValveState::OPEN;
            trigger_flash_store.write(trig_vals);

          } else {
            //TODO Send warning to user...?
            Serial.println("ERROR: Timer mode - Bad formatting of date and/or timer.");
          }
          break;
        case 2:
          Serial.println("VWC mode.");
          break;
        case 3:
          Serial.println("Combined mode.");
          break;
        default:
          Serial.println("Fell off Switch statement. You shouldn't be here.");
          // statements
      }
    } else {
      switch (trig_vals.mode) {
        case 1:
          Serial.println("Closing valve");
          valve_close();
          trig_vals.valve = ValveState::CLOSED;
          trigger_flash_store.write(trig_vals);
          break;
        case 2:
          break;
        case 3:
          break;
        default:
          Serial.println("Fell off Switch statement. You shouldn't be here.");
      }
    }

    clearTargetAlarm(1);
    setTargetAlarm(1, trig_vals.sleep); // TODO Remove?
    delay(75);  // delay so serial stuff has time to print out all the way
    doWakeRoutine = false;

    Serial.println("----- END RUN -----\n\n");

    attachInterrupt(digitalPinToInterrupt(wakeUpPin), wake, FALLING);
  }
}

char* valveStateCheck() {
  if (trig_vals.valve == ValveState::OPEN)
    return ("OPEN");
  else
    return ("CLOSED");
}

