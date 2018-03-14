const int LED_PIN = 13;
const int TENTH_SEC = 100;

void setup() {
  pinMode(LED_PIN, OUTPUT);

  //randomSeed(analogRead(0));
}

void loop() {
  for(int i = 10; i > 0; i--){
    digitalWrite(LED_PIN, HIGH);
    delay(i*TENTH_SEC);
    digitalWrite(LED_PIN, LOW);
    delay(TENTH_SEC);
  }
}
