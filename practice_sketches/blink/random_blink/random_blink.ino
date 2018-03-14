const int LED_PIN = 13;

void setup() {
  pinMode(LED_PIN, OUTPUT);

  randomSeed(analogRead(0));
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(random(200,800));
  digitalWrite(LED_PIN, LOW):
  delay(random(200,800);
}
