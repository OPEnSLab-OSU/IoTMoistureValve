#define VALVE_PIN_ROT_OPEN  10 //Near relay
#define VALVE_PIN_ROT_CLOSE  11 //Far relay

//Time unit defines
#define SECOND    1000
#define HALF_SEC  500
#define TENTH_SEC 100

void setup() {
  //Setup pins for Valve-control
  pinMode(VALVE_PIN_ROT_OPEN, OUTPUT);
  pinMode(VALVE_PIN_ROT_CLOSE, OUTPUT);
}

int count = 0;

void loop() {
  delay(1000);
  Serial.println("----------------");
  Serial.print("Begin run #"); Serial.println(++count);
  Serial.println("----------------");
    
  Serial.println("Opening");
  delay(500);
  valve_open();

  Serial.println("Watering yard for 5 seconds.");
  delay(500);
  delay(5 * SECOND);

  Serial.println("Closing");
  delay(500);
  valve_close();

  
  Serial.println("----------------");
  Serial.print("End run #"); Serial.println(count);
  Serial.println("Reset in 5 seconds.");
  Serial.println("----------------");
  delay(5 * SECOND);
}
