/***********************************
   Author: Peter Dorich
   Based off code from OSC by Kenny Noble
   Back-end for send/ recieve data between hub/ valve
 ************************************/
#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <OSCBundle.h>

/* for feather32u4
  #define RFM95_CS 8
  #define RFM95_RST 4
  #define RFM95_INT 7
*/
/* for M0 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define LED 13


#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include "Ethernet2.h"    //Required for ethernet, Ethernet.h will not work

#include <Dns.h>
#include <Dhcp.h>

#define HUB_ADDRESS 1
#define RELAY_ADDRESS 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

//Size of message for LoRa
#define MSG_SIZE 121

//IDString constructor
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x) //to concatenate a predefined number to a string literal, use STR(x)

#define FAMILY "/LOOM"
#define THIS_DEVICE "/Hub/"
#define RELAY_DEVICE "/Relay/"
#define INSTANCE_NUM 0  // Unique instance number for this device, useful when using more than one of the same device type in same space

//#define IDString FAMILY DEVICE STR(INSTANCE_NUM) // C interprets subsequent string literals as concatenation: "/Loom" "/Ishield" "0" becomes "/Loom/Ishield0"
#define MYIDString FAMILY THIS_DEVICE STR(INSTANCE_NUM) 
#define RelayIDString FAMILY RELAY_DEVICE STR(INSTANCE_NUM) 

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

RHReliableDatagram manager(rf95, HUB_ADDRESS);
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

/********************Ethernet Client Setup******************************/
byte mac[] = {0x98, 0x76, 0xB6, 0x10, 0x61, 0xD6};

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "OPEnS"
#define AIO_KEY         "c3b8ceca3231410ab47418540810c1fe"

EthernetClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// You don't need to change anything below this line!
#define halt(s) { Serial.println(F( s )); while(1);  }

Adafruit_MQTT_Publish Elec_Cond = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/soil-data.elec-cond");
Adafruit_MQTT_Publish Temperature = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/soil-data.temp");
Adafruit_MQTT_Publish VWC = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/soil-data.vwc");
//Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt,  AIO_USERNAME "/feeds/soil-data.on-off");
Adafruit_MQTT_Subscribe txtbox = Adafruit_MQTT_Subscribe(&mqtt,  AIO_USERNAME "/feeds/soil-data.txtbox");

void setup() {
  Serial.begin(9600);
  while (!Serial);

  //manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!manager.init())
    Serial.println("init failed");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

  rf95.setTxPower(23, false);

  Ethernet.begin(mac);
  //delay(1000);

  //mqtt.subscribe(&onoffbutton);
    mqtt.subscribe(&txtbox);
}

uint32_t x = 0;

struct soil_data
{
  float VWC;
  float TEMP;
  uint32_t ELEC_COND;

};

      soil_data s_dat;
      float inst_VWC_low = 0;
      float inst_VWC_high = 0;
      int32_t inst_start = 0;
      int32_t inst_dur = 0;
      int32_t inst_mode = 0;

OSCBundle inst_bndl;

Adafruit_MQTT_Subscribe *subscription;

void loop() {
  //TODO This should (hopefully) do nothing.
  MQTT_connect();

  unsigned long lora_timer = millis();
  int x = 0;
  while (!manager.available() && (millis() - lora_timer < 10000)) {
    x++;
    while ((subscription = mqtt.readSubscription(250))) {
      if (subscription == &txtbox) {
        Serial.print(F("Got: "));
        Serial.println((char *)txtbox.lastread);
    /*    inst_mode = (float)inst_bndl.getOSCMessage(RelayIDString "/mode_inst")->getInt(0);
        inst_dur = (int32_t)inst_bndl.getOSCMessage(RelayIDString "/inst_dur")->getInt(0);
        inst_start = (int32_t)inst_bndl.getOSCMessage(RelayIDString "/inst_start")->getInt(0);
     */ 
      }
    //  if (strcmp((char *)txtbox.lastread, "2000") == 0) {
    //    digitalWrite(LED, HIGH);
    //  }
    //  if (strcmp((char *)txtbox.lastread, "OFF") == 0) {
    //    digitalWrite(LED, LOW);
        
    //  }
      
    }
  }

  if (manager.available()) {
    uint8_t len = sizeof(buf);
    uint8_t from;
    memset(buf, '\0', RH_RF95_MAX_MESSAGE_LEN);
    if (manager.recvfromAck(buf, &len, &from)) {
      OSCBundle bndl;
      get_OSC_bundle((char*)buf, &bndl);
      Serial.println((char*)buf);
      String str;

      //----------------------------------------
      //------ Pack and send instructions ------
      //----------------------------------------
      char inst_mess[MSG_SIZE];
      memset(inst_mess, '\0', MSG_SIZE);

      /* TODO Test values. */
         str = (char*) txtbox.lastread;
         Serial.println(str.toInt());
        // inst_timer = str.toInt();


      inst_bndl.empty();

      // Add desired instructions to bundle. Remember to handle on receiving end. /
      
      inst_mode = 1;
      inst_start = 2000;
      inst_dur = 20000;
      
      inst_bndl.add(MYIDString "/mode_inst").add((int32_t) inst_mode);
      inst_bndl.add(MYIDString "/vwc_low_inst").add((float)inst_VWC_low);
      inst_bndl.add(MYIDString "/vwc_high_inst").add((float)inst_VWC_high);
      inst_bndl.add(MYIDString "/start_inst").add((int32_t) inst_start);
      inst_bndl.add(MYIDString "/dur_inst").add((int32_t) inst_dur);
      
      get_OSC_string(&inst_bndl, inst_mess);
      
  
      if (manager.sendtoWait((uint8_t*)inst_mess, strlen(inst_mess), RELAY_ADDRESS)) {
        Serial.println("Instructions sent.");
      } else {
        Serial.println("Instruction sending failed -- TODO/Resend?");
      }
      //----------------------------------------
      //------ End of instruction passing ------
      //----------------------------------------

      //TODO -- Handle data publish retry on fail???
      Serial.print("[Publishing data] - ");
      //Publish Info to Adafruit.io

      Serial.print("Elec_Cond: ");
      if (! Elec_Cond.publish(s_dat.ELEC_COND)) {
        Serial.print(F("Failed "));
      } else {
        Serial.print(F("OK "));
      }

      Serial.print("Temp: ");
      if (! Temperature.publish((char *) String(s_dat.TEMP).c_str())) {
        Serial.print(F("Failed "));
      } else {
        Serial.print(F("OK "));
      }

      Serial.print("VWC: ");
      if (! VWC.publish((char *) String(s_dat.VWC).c_str())) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK"));
      }

      //DEBUG Newlines between prints.
      Serial.println("\n\n");
    }
  }

}
