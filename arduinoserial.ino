#define BUTTPIN1 2
#define BUTTPIN2 3
#define BUTTPIN3 4
#define POTPIN1 A0
#define POTPIN2 A1

void setup() {
  Serial.begin(9600);
  pinMode(BUTTPIN1, INPUT_PULLUP);
  pinMode(BUTTPIN2, INPUT_PULLUP);
  pinMode(BUTTPIN3, INPUT_PULLUP);
}

void loop() {
  int b1 = digitalRead(BUTTPIN1);
  int b2 = digitalRead(BUTTPIN2);
  int b3 = digitalRead(BUTTPIN3);
  int p1 = analogRead(POTPIN1);
  int p2 = analogRead(POTPIN2);

  Serial.print(b1);
  Serial.print(",");
  Serial.print(b2);
  Serial.print(",");
  Serial.print(b3);
  Serial.print(",");
  Serial.print(p1);
  Serial.print(",");
  Serial.println(p2);

  delay(100);
}

