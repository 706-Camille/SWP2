#define PIN_LED 13
unsigned int count, toggle;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  while(!Serial)
  {
    ; // waiting
  }
}


void loop() {
  Serial.println(++count);
  toggle = toggle_state(toggle); // toggle LED value
  digitalWrite(PIN_LED, toggle); // updated LED status
  delay(1000);
}

int toggle_state(int toggle)
{
  return toggle;
}
