/***************************************************
  Adafruit MQTT Library Ethernet W/ LoRa

  Written by Peter Dorich
  Derived from the code written by Limor Fried/Ladyada for Adafruit Industries.
  Derived from Alec Moore
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <SPI.h>
#include <RH_RF95.h>
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_IRQ 7
#define LED_PIN 13
#define FREQ 915.0

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include "Ethernet2.h"    //Required for ethernet, Ethernet.h will not work

#include <Dns.h>
#include <Dhcp.h>

/************************* LoRa Radio Setup **********************************/
RH_RF95 rf95(RFM95_CS, RFM95_IRQ);
/************************* Ethernet Client Setup *****************************/
byte mac[] = {0x98, 0x76, 0xB6, 0x10, 0x61, 0xD6};

//Uncomment the following, and set to a valid ip if you don't have dhcp available.
//IPAddress iotIP (128, 193, 56, 138);
//Uncomment the following, and set to your preference if you don't have automatic dns.
//IPAddress dnsIP (8, 8, 8, 8);
//If you uncommented either of the above lines, make sure to change "Ethernet.begin(mac)" to "Ethernet.begin(mac, iotIP)" or "Ethernet.begin(mac, iotIP, dnsIP)"


/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "OPEnS"
#define AIO_KEY         "c3b8ceca3231410ab47418540810c1fe"


/************ Global State (you don't need to change this!) ******************/

//Set up the ethernet client
EthernetClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// You don't need to change anything below this line!
#define halt(s) { Serial.println(F( s )); while(1);  }


/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish test2 = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/test2");
Adafruit_MQTT_Publish test = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/test");


// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

/*************************** Sketch Code ************************************/

void setup() {
  pinMode (RFM95_RST, OUTPUT);
  pinMode (LED_PIN, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  Serial.begin(115200);
  Serial.println(F("Adafruit MQTT demo"));

  // Initialise the Client
  Serial.print(F("\nInit the Client..."));
  Ethernet.begin(mac);
 // delay(1000); //give the ethernet a second to initialize

  Serial.println("Feather LoRa TX Test!");

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
  }

  Serial.println("LoRa Radio init successful!");

  if (!rf95.setFrequency(FREQ)) {
    Serial.println("setFrequency failed");
    while(1);
  }

  Serial.print ("Set Freq to: "); Serial.println(FREQ);
  
  mqtt.subscribe(&onoffbutton);
}

uint32_t x=0;
uint8_t on_off = 0;
void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
 // delay (5 * 1000); //Wait 1 second between transmits, could also sleep here"
  // this is our 'wait for incoming subscription packets' busy subloop
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
    }
  }

  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
     if (rf95.recv(buf, &len))
      {
      digitalWrite(LED_PIN, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
       Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      
      // Send a (data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      test2.publish(rf95.lastRssi(), DEC);
   //   test.publish(x++);
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  /*  if (! test2.publish(x++)) {
     Serial.println(F("Failed"));
     } else {
        Serial.println(F("OK!"));
     }
  */
  // ping the server to keep the mqtt connection alive
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
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
