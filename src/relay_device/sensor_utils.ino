  void preReadClean(){
    sdiResponse = "";
    resp_buf = "";
    myCommand = "";
    memset(message, '\0', MSG_SIZE);
  }
  
  void sensorRead() {
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
  }

  void packageData(){
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

    get_OSC_string(&bndl, message);

    Serial.println(message);
    Serial.print("Message length: "); Serial.println(strlen(message));
    Serial.print("Max message length: "); Serial.println(RH_RF95_MAX_MESSAGE_LEN);
  }
