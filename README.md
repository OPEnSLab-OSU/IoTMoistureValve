# IoTMoistureValve
Repo for IoT Moisture Sensing Valve

Since this project was completed for the Openly Published Environmental Sensing Lab, all of the code we produced is open source. 
The OPEnS lab uses GitHub as a source code repository, so our entire project was uploaded there.
Our code base can easily be viewed and downloaded from the OPEnS Lab GitHub page. 

1. As a beginning pre-step, at the time of this writing, our project requires specific hardware to function correctly. This hardware is outlined in the design document. While not advised currently, if the user desires to use hardware not outline in our document, it may be possible, but the included code would need to be modified heavily; many of the libraries we have included are limited in the devices that they serve.
2. After verifying that you have matching hardware, you'll need to setup your Arduino environment. The Arduino IDE can be retrieved from the Arduino homepage (https://www.arduino.cc/en/Main/Software) free of charge. It is available for Windows, Mac and Linux. If looking to install the software for Linux, pre-packaged versions exist on the various package repositories if desired.
3. After installing the Arduino environment, there are a few setup steps that are required. First off, as the Adafruit devices are not affiliated with Arduino directly, an additional repository for the device cores needs to be added. Open the Preference section of the Arduino IDE (File -> Preferences) and look for the text field titled "Additional Board Manager URLs". In that box, add the address: https://adafruit.github.io/arduino-board-index/package_adafruit_index.json . Further explanation and any potential updates by Adafruit can be found on the Adafruit home-page (https://learn.adafruit.com/add-boards-arduino-v164/setup) if desired. After this, close the Arduino IDE software and re-open it. This should refresh the list of available device support in the Boards Manager.
4. From here, under the the Boards Manager (Tools -> Boards -> Boards Manager), type into the search bar "M0". There will be a couple listings; the ones you'll need to obtain are the Arduino SAMD Boards by Arduino, and the Adafruit SAMD Boards by Adafruit. This will allow you to compile and upload the code targeted at these devices.
5. From here, select the "Adafruit Feather M0" board from the list of boards in the Boards menu (Tools -> Boards -> Adafruit Feather M0). The Arduino IDE can now compile and upload the code to the M0 devices correctly. To run the code, just hit the "upload" (right-pointing arrow) button in the IDE tool bar.  Once it is successfully uploaded and plugged into power, the program will automatically begin running. You will need to repeat this process for the hub and the valve-controller device. The boards will continually run the uploaded software in their main loop.  If debugging text is desired, the Arduino serial monitor can be opened to view the debug output (Tools -> Serial Monitor).

For each of the three sections of the project there is a corresponding ReadMe file with full documentation and instructions, that should stay up to date with the project.

Links are provided here:

* [Valve Controller](src/relay_device)


* [Hub](src/hub/Hub_V0.1)


* [UI](/src/user%20interface/UI)