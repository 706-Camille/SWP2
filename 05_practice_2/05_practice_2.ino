#define PIN_LED 7

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  while(!Serial)
  {
    ; // waiting
  }
  delay(1000);
}

void loop() {
  digitalWrite(PIN_LED, 5);
  delay(1000);

  for(int i = 0 ; i < 5; i++)
  {
    digitalWrite(PIN_LED, 0);
    delay(100);
    digitalWrite(PIN_LED, 5);
    delay(100);
  }
  exit(0);
}
