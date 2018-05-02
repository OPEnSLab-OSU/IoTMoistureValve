// ==========================================================================================================
// =====! Currently this entire utility requires that the RTC3231 #define is set in rtc_sleep_wake.ino !=====
// ==========================================================================================================
  void InitalizeRTC()
  {
    // RTC Timer settings here
    if (! RTC_DS.begin()) {
        Serial.println("Couldn't find RTC");
      while (1); // Activates Watchdog Function after 15ms.
    }
    
    // This may end up causing a problem in practice - what if RTC loses power in field? Shouldn't happen with coin cell batt backup
    if (RTC_DS.lostPower()) {
        Serial.println("RTC lost power, lets set the time!");
      // following line sets the RTC to the date & time this sketch was compiled
      RTC_DS.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    
    //clear any pending alarms
//    clearTargetAlarm(1);
//    clearTargetAlarm(2);
  
    // Query Time and print
    DateTime now = RTC_DS.now();
    
      Serial.print("RTC Time is: ");
      Serial.print(now.hour(), DEC); Serial.print(':'); Serial.print(now.minute(), DEC); Serial.print(':'); Serial.print(now.second(), DEC); Serial.println();
    
    //Set SQW pin to OFF (in my case it was set by default to 1Hz)
    //The output of the DS3231 INT pin is connected to this pin
    //It must be connected to arduino Interrupt pin for wake-up
    RTC_DS.writeSqwPinMode(DS3231_OFF);
  
    //Set alarm1
//    setAlarmMins(1, 5);
  }
  
  // *********
  // RTC helper function
  // Function to query current RTC time and add the period to set next alarm cycle
  // *********
  void setAlarmMins(byte alarm_num, unsigned int min_offset)
  {

    if(0 > alarm_num || alarm_num > 2) {
      Serial.println("Alarm number must be 1 or 2.");
      return;
    }
    
    DateTime now = RTC_DS.now(); // Check the current time

    // Calculate new time
    MIN = (now.minute() + min_offset) % 60; // wrap-around using modulo every 60 sec
    HR  = (now.hour() + ((now.minute() + min_offset) / 60)) % 24; // quotient of now.min+periodMin added to now.hr, wraparound every 24hrs
    
    Serial.print("Resetting Alarm "); Serial.print(alarm_num); Serial.print(" for: "); Serial.print(HR); Serial.print(":"); Serial.println(MIN);

    // set your wake-up time here      
    if(alarm_num == 1){
      RTC_DS.setAlarm(ALM1_MATCH_HOURS, MIN, HR, 0);
    } else if(alarm_num == 2){
      RTC_DS.setAlarm(ALM2_MATCH_HOURS, MIN, HR, 0);
    }
    
    //Set alarm1
    RTC_DS.alarmInterrupt(alarm_num, true);
  }
  
  //*********
  // RTC helper function
  // Clear specific alarm from the RTC
  //*********
  void clearTargetAlarm(int alarm_num)
  {
    //clear any pending alarms
    RTC_DS.armAlarm(alarm_num, false);
    RTC_DS.clearAlarm(alarm_num);
    RTC_DS.alarmInterrupt(alarm_num, false);
  }

  // Called when on interrupt from INT_PIN
  void wake()
  {
    doWakeRoutine = true;
  }

  unsigned long minsToSecLong(unsigned long mins) {
    return mins*60L;
  }

