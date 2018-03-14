/***********************************
 * Author: Peter Dorich 
 * Based off code from OSC by Kenny Noble 
 * Back-end for send/ recieve data between hub/ valve
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
  
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include "Ethernet2.h"    //Required for ethernet, Ethernet.h will not work

#include <Dns.h>
#include <Dhcp.h>

#define SERVER_ADDRESS 1
#define CLIENT_ADDRESS 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

RHReliableDatagram manager(rf95, SERVER_ADDRESS);
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

Adafruit_MQTT_Publish test2 = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/test2");
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");


void setup() {
  Serial.begin(9600);
  if (!manager.init())
    Serial.println("init failed");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

  rf95.setTxPower(23, false);

  Ethernet.begin(mac);
  //delay(1000);

//  mqtt.subscribe(&onoffbutton);
}

uint32_t x=0;

void loop() {
  // put your main code here, to run repeatedly:
  MQTT_connect();
  delay(5 * 1000);
   Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
    }
  }
  if(! test2.publish(x++)) {
      Serial.println(F("Failed"));
    } else{
         Serial.println(F("OK!"));
  }
  
  if (manager.available()) {
    uint8_t len = sizeof(buf);
    uint8_t from;
    memset(buf, '\0', RH_RF95_MAX_MESSAGE_LEN);
    if (manager.recvfromAck(buf, &len, &from)) {
      OSCBundle bndl;
      get_OSC_bundle((char*)buf, &bndl);
     // Serial.print("Received message: ");
      Serial.println((char*)buf);
      bndl.send(Serial);
      Serial.println("");

      
    }
  }
}
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}
