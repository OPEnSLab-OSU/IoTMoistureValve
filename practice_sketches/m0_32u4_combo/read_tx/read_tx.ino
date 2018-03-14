#include <SPI.h>
#include <RH_RF95.h>
#include "SDI12.h"

#define DATAPIN 12         // change to the proper pin
#define SENSOR_ADDRESS "?"

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

SDI12 mySDI12(DATAPIN);

String sdiResponse = "";
String myCommand = "";
char response_buff[30];
char *pntr_for_resp;

float dielec_p = 0;
float temp = 0;
float elec_c = 0;

/* for feather32u4
  #define RFM95_CS 8
  #define RFM95_RST 4
  #define RFM95_INT 7
*/

/* for feather m0 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup()
{
  ///////////////
  //  Setup
  //////////////
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  mySDI12.begin();

  while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Feather LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop()
{
  ///////////
  // Sensor
  ///////////

  do {                           // wait for a response from the serial terminal to do anything
    delay (30);
  }
  while (!Serial.available());
  char nogo = Serial.read();     // simply hit enter in the terminal window or press send and
                                 // the characters get discarded but now the rest of the loop continues

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
//  sdiResponse.toCharArray(response_buff, sizeof(response_buff));
  
  if (sdiResponse.length() > 1) Serial.println(sdiResponse); //write the response to the screen

  Serial.println("Prep LoRa package...");

  Serial.print("Length of response is: "); Serial.println(sizeof(sdiResponse));

  strcpy(response_buff, sdiResponse.c_str());

//  pntr_for_resp = response_buff
//  strtok_r(p, "+", &p);
//  dielec_p = atof(strtok_r(NULL, "+", &p));
//  temp = atof(strtok_r(NULL, "+", &p));
//  elec_c = atof(strtok_r(NULL, "+", &p));
  
  Serial.println("Send LoRa package...");

  Serial.println("Sending..."); delay(10);
  rf95.send((uint8_t *)response_buff, 30);

  Serial.println("Waiting for packet to complete..."); delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t return_buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(return_buf);

  Serial.println("Waiting for reply..."); delay(10);
  if (rf95.waitAvailableTimeout(1000))
  {
    // Should be a reply message for us now
    if (rf95.recv(return_buf, &len))
    {
      Serial.print("Got reply: ");
      Serial.println((char*)return_buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?");
  }
  delay(1000); 
  
  mySDI12.clearBuffer();
  sdiResponse = "";

//now go back to top and wait until user hits enter on terminal window

  /////////////
  // End Sensor
  /////////////
  
  ////////////
  //  LoRa
  ////////////
//  Serial.println("Sending to rf95_server");
//  // Send a message to rf95_server
//
//  char radiopacket[20] = "Hello World #      ";
//  itoa(packetnum++, radiopacket + 13, 10);
//  Serial.print("Sending "); Serial.println(radiopacket);
//  radiopacket[19] = 0;
//
//  Serial.println("Sending..."); delay(10);
//  rf95.send((uint8_t *)radiopacket, 20);
//
//  Serial.println("Waiting for packet to complete..."); delay(10);
//  rf95.waitPacketSent();
//  // Now wait for a reply
//  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
//  uint8_t len = sizeof(buf);
//
//  Serial.println("Waiting for reply..."); delay(10);
//  if (rf95.waitAvailableTimeout(1000))
//  {
//    // Should be a reply message for us now
//    if (rf95.recv(buf, &len))
//    {
//      Serial.print("Got reply: ");
//      Serial.println((char*)buf);
//      Serial.print("RSSI: ");
//      Serial.println(rf95.lastRssi(), DEC);
//    }
//    else
//    {
//      Serial.println("Receive failed");
//    }
//  }
//  else
//  {
//    Serial.println("No reply, is there a listener around?");
//  }
//  delay(1000); 

  /////////////
  // End LoRa
  /////////////



}
