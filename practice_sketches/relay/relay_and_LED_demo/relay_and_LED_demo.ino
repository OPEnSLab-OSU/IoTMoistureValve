//PIN consts
const int LED_PIN = 13;
const int RELAY_PIN = 11;

//Time consts
const int TENTH_SEC = 100;
const int ONE_SECOND = 1000;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
}

void loop() {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    delay(5*ONE_SECOND);
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);    
    delay(5*ONE_SECOND);
}
