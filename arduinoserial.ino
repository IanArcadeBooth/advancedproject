/*
* File: arduinoserial.ino
* Author: Ian Booth
* Date: 2026/03/29
* Description: Reads button and potentiometer inputs from the Arduino
* and sends the data over serial in CSV format. This data is used by
* the Raspberry Pi to control the lunar lander simulation.
*/

#define BUTTPIN1 2 //start
#define BUTTPIN2 3 //left
#define BUTTPIN3 4 //right
#define POTPIN1 A0 //thrust
#define POTPIN2 A1 //landing zone

#define SMLDLAY 50

/*
* Description: Initializes serial communication and configures pins
* @param none
* @return: none
* side effects: sets pin modes and starts serial communication
*/
void setup() {
  Serial.begin(9600);
  pinMode(BUTTPIN1, INPUT_PULLUP);
  pinMode(BUTTPIN2, INPUT_PULLUP);
  pinMode(BUTTPIN3, INPUT_PULLUP);
}

/*
* Description: Reads inputs and sends them over serial continuously
* @param none
* @return: none
* side effects: outputs data over serial connection
*/
void loop() {
  int b1 = digitalRead(BUTTPIN1);
  int b2 = digitalRead(BUTTPIN2);
  int b3 = digitalRead(BUTTPIN3);
  int p1 = analogRead(POTPIN1);
  int p2 = analogRead(POTPIN2);

  //sends data as CSV line
  Serial.print(b1);
  Serial.print(",");
  Serial.print(b2);
  Serial.print(",");
  Serial.print(b3);
  Serial.print(",");
  Serial.print(p1);
  Serial.print(",");
  Serial.println(p2);

  delay(SMLDLAY); //wait between lines
}

