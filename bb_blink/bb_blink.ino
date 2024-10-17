
#define LED_PIN 13

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_PIN, HIGH); // 3.3V->pin 13
  Serial.println("ON");
  delay(500); // 1000ms = 1s
  digitalWrite(LED_PIN, LOW); // 0V->pin 13
  Serial.println("OFF");
  delay(500); // 1000ms = 1s
}
