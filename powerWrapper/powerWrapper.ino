


int potpin = 8;  // analog pin used to connect the potentiometer
int clearPathPin = 10;    // LED connected to digital pin 9
int val;    // variable to read the value from the analog pin

void setup() {
}

void loop() {
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 1023, 0, 235);     // scale it to use it with the servo (value between 0 and 180)
  analogWrite(clearPathPin, val);
  delay(15);                           // waits for the servo to get there
}

