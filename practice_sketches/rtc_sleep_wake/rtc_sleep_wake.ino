#include "LowPower.h"
#include <Wire.h>

//  Original library from:          https://github.com/FabioCuomo/FabioCuomo-DS3231
//  NOTE: Must include the following line in the RTClibExtended.h file to use with M0:
//  #define _BV(bit) (1 << (bit))
//  Modified file can be found at:  https://github.com/OPEnSLab-OSU/RTC/tree/master/RTClibExtended%20(1)/RTClibExtended
#include <RTClibExtended.h>

#ifdef __SAMD21G18A__
  #define is_M0
#endif

#define DEBUG 1 // Comment/uncomment if DEBUG functionality is desired (e.g. Serial prints, etc.)

#ifndef DEBUG
  #define DEBUG 0
#endif

// ===== RTC Initializations =====
#define RTC3231 //  Comment/uncomment if using RTC/Wake-up
#ifdef RTC3231
  RTC_DS3231 RTC_DS;

  //  =================================================================================
  //  =====! Alarm type not currently implemented, only interval wake currently. !=====
  //  =================================================================================
  
  // Declare/init RTC_DS variables//
  volatile bool TakeSampleFlag = false; // Flag is set with external pin interrupt by RTC using wake() method in rtc_utils.
  volatile int HR = 8;                  // Hours    --- Use this for daily alarm implementation.
  volatile int MIN = 0;                 // Minutes  --- Use this for daily alarm implementation.
  volatile int WakePeriodMin = 1;       // Period of time to take sample in Min, reset alarm based on this period (Bo - 5 min)
  const byte wakeUpPin = 13;            // Set to desired GPIO for interrupt.
#endif

//===== Setup =====
void setup() {
  Serial.begin(9600);

  #if DEBUG == 1
  
    // =====================================================================================
    // =====! Forces code to wait for input from Serial monitor before beginning.     !=====
    // =====! Code will "hang" here until Serial input is recieved, if DEBUG is set.  !=====
    // =====================================================================================
    while(!Serial.available());
    
  #endif
  
  delay(2000); //TODO Remove?
   
  Serial.println("Beginning setup");
  
  // RTC Setup
  #ifdef RTC3231 // ! Ensure RTC3231 #define is set if desired. !
    Serial.println("RTC defined.");
    pinMode(wakeUpPin, INPUT_PULLUP);
    #ifdef is_M0
      Serial.println("M0 defined.");
      attachInterrupt(wakeUpPin, wake, FALLING);
    #endif // is_M0
  
    InitalizeRTC();
    
    #if DEBUG == 1
        Serial.print("Alarm set to go off every "); Serial.print(WakePeriodMin); Serial.println("min from program time");
    #endif // DEBUG
    delay(10000);
  #endif // RTC3231
}

void loop() {
  
#ifdef RTC3231
  #ifdef is_M0
    attachInterrupt(digitalPinToInterrupt(wakeUpPin), wake, FALLING);
    LowPower.idle(IDLE_2);
    Serial.println("This should not print.");
  #endif //is_M0
#endif //RTC3231

#ifdef RTC3231
  if(TakeSampleFlag)
  { 
  #ifdef is_M0
    detachInterrupt(digitalPinToInterrupt(wakeUpPin));
  #endif //is_M0

  Serial.println("I woke up.");

  clearAlarmFunction(); // Clear RTC Alarm
  setAlarmFunction();
  delay(75);  // delay so serial stuff has time to print out all the way
  TakeSampleFlag = false; // Clear Sample Flag
  }
#endif //RTC3231
delay(2000);
}

