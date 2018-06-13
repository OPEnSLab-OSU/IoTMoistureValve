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

//Time incrementsdur
#define SECOND    1000
#define HALF_SEC  500
#define TENTH_SEC 100

//Delay (spoof wake-up) time.
#define WAIT 20

//Size of message for LoRa
#define MSG_SIZE RH_RF95_MAX_MESSAGE_LEN

//IDString constructor
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x) // To concatenate a predefined number to a string literal, use STR(x)

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
  boolean       recur;        // Should timer be recurring?
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
volatile bool doWakeRoutine = false;  // Flag is set with external pin interrupt by RTC using wake() method in rtc_utils.
volatile int HR = 8;                  // Hours    --- Use this for daily alarm implementation.
volatile int MIN = 0;                 // Minutes  --- Use this for daily alarm implementation.
volatile int WakePeriodMin = 1;       // Period of time to take sample in Min, reset alarm based on this period (Bo - 5 min)
const byte wakeUpPin = 13;

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

    //Rough estimate default values given by client.
    trig_vals.mode = 1;
    trig_vals.vwc_low = 10.15;
    trig_vals.vwc_high = 20.02;
    trig_vals.start = 0;
    trig_vals.dur = 2;
    trig_vals.sleep = 1;
    trig_vals.start_unix  = ((temp_time.unixtime() - (temp_time.unixtime()%60UL)) + trig_vals.start * 60UL);
    trig_vals.dur_unix    = ((temp_time.unixtime() - (temp_time.unixtime()%60UL)) + trig_vals.dur   * 60UL);
    trig_vals.sleep_unix  = ((temp_time.unixtime() - (temp_time.unixtime()%60UL)) + trig_vals.sleep * 60UL);
    trig_vals.recur = true;
    trig_vals.valid = true;

    //Close valve to put it into good state if Flash storage values are lost.
    valve_close();
    trig_vals.valve = ValveState::CLOSED; //Write new values.
    delay(1000);

    trigger_flash_store.write(trig_vals);
  } else {
    //Begin longest print-block ever...
    Serial.println("Got stored values -");
    Serial.print("Mode: "); Serial.print(trig_vals.mode); Serial.print(" ");
    Serial.print("VWC Lower: "); Serial.print(trig_vals.vwc_low); Serial.print(" ");
    Serial.print("VWC Upper: "); Serial.print(trig_vals.vwc_high); Serial.print(" ");
    Serial.print("Start Time: "); Serial.print(trig_vals.start); Serial.print(" ");
    Serial.print("Duration: "); Serial.print(trig_vals.dur);
    Serial.print("Sleep interval: "); Serial.println(trig_vals.sleep);
    Serial.print("Start time (Unix): "); Serial.println(trig_vals.start_unix);
    Serial.print("Duration time (Unix): "); Serial.println(trig_vals.dur_unix);
    Serial.print("Sleep interval (Unix): "); Serial.println(trig_vals.sleep_unix);
    Serial.print("Valve state: "); Serial.println(valveStateCheck());
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
}

//Setup for sensor read.
String sdiResponse = "";
String resp_buf = "";
String myCommand = "";
char delimit[] = "+-"; // Delimiters for response parsing.
float VWC, temp;
int elec;
char message[MSG_SIZE];

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

    Serial.println("----- BEGIN RUN -----");
    Serial.print("Run: #"); Serial.println(++count);
    Serial.print("Unix time now is: "); Serial.println(time_now.unixtime());

    last_batt_val = check_batt();

    // In sensor_utils.ino
    // Cleans variables before new read and packaging.
    preReadClean();

    // In sensor_utils.ino
    // Reads data from Decagon device.
    sensorRead();

    // In sensor_utils.ino
    // Packages read data for sending.
    packageData();

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
        trig_vals.dur_unix    = (time_now.unixtime() + (start_inst * 60UL) + (dur_inst   * 60UL));
        trig_vals.sleep_unix  = (time_now.unixtime() + sleep_inst * 60UL);

        //        Serial.println(trig_vals.start_unix);
        //        Serial.println(trig_vals.dur_unix);
        //        Serial.println(trig_vals.sleep_unix);

        trigger_flash_store.write(trig_vals);

        new_instructions = false;
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
    Serial.print(trig_vals.sleep_unix); Serial.print(" ");
    Serial.println(trig_vals.recur);
    

    Serial.print("Valve currently: "); Serial.println(valveStateCheck());

    if (trig_vals.valve == ValveState::CLOSED) {
      switch (trig_vals.mode) {
        case 1:
          Serial.println("INFO: Timer mode.");

          if (trig_vals.start_unix > time_now.unixtime()) {
            Serial.println("INFO: Start time later than now, waiting for next check in.");
            break;
          } else if ( (trig_vals.start_unix <= time_now.unixtime()) &&
                      (trig_vals.dur_unix > time_now.unixtime()) &&
                      (trig_vals.dur > 0))
          {
            Serial.print("INFO: Watering for "); Serial.print(trig_vals.dur); Serial.println(" minutes.");

            valve_open();
            trig_vals.valve = ValveState::OPEN;
            trigger_flash_store.write(trig_vals);

          } else {
            if (trig_vals.recur){
//                Serial.println("INFO: Recurring timer requested, resetting timer values for 24-hours from now.");
//                trig_vals.start_unix  = (time_now.unixtime() + (86400UL - (trig_vals.dur * 60UL)));
//                trig_vals.dur_unix    = (time_now.unixtime() +  86400UL);
                Serial.println("INFO: Recurring timer requested, resetting timer values for 1 minute from now. from now.");
                trig_vals.start_unix  = (time_now.unixtime() + 60UL);
                trig_vals.dur_unix    = (time_now.unixtime() + (60UL) + (trig_vals.dur * 60UL));
                Serial.print("INFO: New start/end time: "); Serial.print(trig_vals.start_unix);
                Serial.print(" / "); Serial.println(trig_vals.dur_unix);
            } else {
              //TODO Send warning to user...?
              Serial.println("WARN: Start and duration timers exceeded. Nothing to do. No recurring timer?");
          }
          break;
        case 2:
          Serial.println("INFO: VWC mode.");
          Serial.print(trig_vals.vwc_low); Serial.print(" "); Serial.println(VWC);
          if (trig_vals.vwc_low > VWC && VWC < trig_vals.vwc_high) {
            Serial.println("INFO: Current VWC low, opening...");
            valve_open();
            trig_vals.valve = ValveState::OPEN;
            trigger_flash_store.write(trig_vals);
            break;
          } else {
            //TODO Send warning to user...?
            Serial.println("INFO: VWC within range, waiting for next check in.");
            break;
          }
        case 3:
          Serial.println("INFO: Combined mode.");
          if (trig_vals.start_unix > time_now.unixtime()) {
            Serial.println("INFO: Start time later than now, waiting for next check in.");
            break;
          } else if ( (trig_vals.start_unix <= time_now.unixtime()) &&
                      (trig_vals.dur_unix > time_now.unixtime()) &&
                      (trig_vals.dur > 0))
          {
            Serial.print("INFO: Within time range for combined mode, checking VWC... ");
            if (trig_vals.vwc_high > VWC) {
              Serial.print("VWC is below "); Serial.print(trig_vals.vwc_high);
              Serial.println("... opening.");

              valve_open();
              trig_vals.valve = ValveState::OPEN;
              trigger_flash_store.write(trig_vals);
            } else {
              Serial.print("VWC is above "); Serial.print(trig_vals.vwc_high);
              Serial.println("... waiting for next check in.");              
            }

          } else {
            if (trig_vals.recur){
//                Serial.println("INFO: Recurring timer requested, resetting timer values for 24-hours from now.");
//                trig_vals.start_unix  = (time_now.unixtime() + (86400UL - (trig_vals.dur * 60UL)));
//                trig_vals.dur_unix    = (time_now.unixtime() +  86400UL);
                Serial.println("INFO: Recurring timer requested, resetting timer values for 1 minute from now. from now.");
                trig_vals.start_unix  = (time_now.unixtime() + 60UL);
                trig_vals.dur_unix    = (time_now.unixtime() + (60UL) + (trig_vals.dur * 60UL));
                Serial.print("INFO: New start/end time: "); Serial.print(trig_vals.start_unix);
                Serial.print(" / "); Serial.println(trig_vals.dur_unix);
            } else {
            //TODO Send warning to user...?
            Serial.println("WARN: Start and duration timers exceeded. Nothing to do. No recurring timer?");
            }
          }
          break;
        default:
          Serial.println("ERROR: ValveState - CLOSED; Fell off Switch statement. You shouldn't be here.");
      }
    } else {
      switch (trig_vals.mode) {
        case 1:
          Serial.println("INFO: Timer mode.");
          if (trig_vals.start_unix > time_now.unixtime()) {
            Serial.println("WARN: Start time in future, closing...");
            delay(50);
            valve_close();
            trig_vals.valve = ValveState::CLOSED;
            trigger_flash_store.write(trig_vals);
          } else {
            if (trig_vals.dur_unix > time_now.unixtime()) {
              Serial.println("INFO: Duration timer greater than now. Continuing to water.");
            } else {
              Serial.println("INFO: Duration timer exceeded. Closing valve.");
              valve_close();
              trig_vals.valve = ValveState::CLOSED;
              if (trig_vals.recur) {
//                Serial.println("INFO: Recurring timer requested, resetting timer values for 24-hours from now.");
//                trig_vals.start_unix  = (time_now.unixtime() + (86400UL - (trig_vals.dur * 60UL)));
//                trig_vals.dur_unix    = (time_now.unixtime() +  86400UL);
                Serial.println("INFO: Recurring timer requested, resetting timer values for 1 minute from now. from now.");
                trig_vals.start_unix  = (time_now.unixtime() + 60UL);
                trig_vals.dur_unix    = (time_now.unixtime() + (60UL) + (trig_vals.dur * 60UL));
                Serial.print("INFO: New start/end time: "); Serial.print(trig_vals.start_unix);
                Serial.print(" / "); Serial.println(trig_vals.dur_unix);
              } else {
                Serial.println("WARN: Recurring timer not requested. Updated need for continued operation.");
              }
              trigger_flash_store.write(trig_vals);
            }
          }
          break;
        case 2:
          Serial.println("INFO: VWC mode.");
          if (trig_vals.vwc_high > VWC) {
            Serial.println("INFO: VWC below defined high value, continuing watering until next check in.");
            break;
          } else {
            Serial.println("INFO: VWC above defined high value, closing...");
            valve_close();
            trig_vals.valve = ValveState::CLOSED;
            trigger_flash_store.write(trig_vals);
            //TODO Send warning to user...?
            break;
          }
        case 3:
          Serial.println("INFO: Combined mode.");
          if (trig_vals.start_unix > time_now.unixtime()) {
            Serial.println("WARN: Start time in future, closing...");
            delay(50);
            valve_close();
            trig_vals.valve = ValveState::CLOSED;
            trigger_flash_store.write(trig_vals);
          } else {
            if (trig_vals.dur_unix > time_now.unixtime()) {
              Serial.print("INFO: Duration time in future, checking VWC...");
              if (trig_vals.vwc_high > VWC) {
                Serial.print("VWC below "); Serial.print(trig_vals.vwc_high);
                Serial.println("... waiting for next check in.");
                break;
              } else {
                Serial.println("INFO: VWC above defined high value, closing...");
                valve_close();
                trig_vals.valve = ValveState::CLOSED;
                trigger_flash_store.write(trig_vals);
                //TODO Send warning to user...?
                break;
              }
            } else {
              Serial.println("INFO: Duration timer has expired, closing...");
              valve_close();
              trig_vals.valve = ValveState::CLOSED;
               if (trig_vals.recur) {
//                  Serial.println("INFO: Recurring timer requested, resetting timer values for 24-hours from now.");
//                  trig_vals.start_unix  = (time_now.unixtime() + (86400UL - (trig_vals.dur * 60UL)));
//                  trig_vals.dur_unix    = (time_now.unixtime() +  86400UL);
                Serial.println("INFO: Recurring timer requested, resetting timer values for 1 minute from now. from now.");
                trig_vals.start_unix  = (time_now.unixtime() + 60UL);
                trig_vals.dur_unix    = (time_now.unixtime() + (60UL) + (trig_vals.dur * 60UL));
                  Serial.print("INFO: New start/end time: "); Serial.print(trig_vals.start_unix); Serial.println(trig_vals.dur_unix);
                } else {
                  Serial.println("WARN: Recurring timer not requested. Updated need for continued operation.");
                }
              trigger_flash_store.write(trig_vals);
              break;
            }
          }
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
