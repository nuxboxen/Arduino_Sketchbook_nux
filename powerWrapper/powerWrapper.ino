//Power Wrapper Controller 
//Brent Kinser
//brentkinser@icloud.com
//August 2017
#include <OneButton.h>
#include <Adafruit_NeoPixel.h>
#define PIN 6

const int ledPin =  7;  // Led on power switch.
int ledState = LOW;
unsigned long previousMillis = 0;
long interval = 1000;

int reverseState = LOW;

// const int keyPin = 2;
OneButton button(2, true);

boolean rotation = false;
boolean dryMode = false;
int potpin = 14;  // analog pin used to connect the potentiometer
const int reversePin = 12;       // White Wire in ClearPath Cable
const int clearPathPin = 10;    // Black Wire in ClearPath Cable 
const int enablePin = 11;       // Blue Wire in ClearPath Cable
int val;    // variable to read the value from the analog pin
int drySpeed=4;
int light = 0;

// http://web-tech.ga-usa.com/2012/05/creating-a-custom-hot-to-cold-temperature-color-gradient-for-use-with-rrdtool/
// 255,14,240    hot pink
// 255,0,112      medium pink
// 255,0,0           red
// 255,120,0       orange
// 255,255,0       yellow
//0,255,16          green
//0,255,244        teal blue
//0,116,255        light blue
//5,0,255            dark blue 

int r[] = {  0,  0,255,255,  0,  5,  0,255,255,255,255,255,255 };
int g[] = {255,255,255,255,116,  0,255,255,120,  0,  0, 14,0};
int b[] = { 16, 16,  0,  0,255,255, 16,  0,  0,  0,112,240,0};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(reversePin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  Serial.begin(9600);
  Serial.print("Serial comms initialized \n\n");
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  //pinMode(keyPin, INPUT); 
  button.attachClick(singleclick);    
  button.attachDoubleClick(doubleclick);   // This function will be called when the button was pressed 2 times in a short timeframe.
  button.attachLongPressStart(longpress);  // This function will be called once, when the button is pressed for a long time.
}

void speedometer(uint32_t c, uint8_t wait) {
        for (int i=0;i<24; i++) {                            // 9 is the number of pixels in the ring
          if (light>=i)  {
            delay(10);
            strip.setPixelColor(i, c);   
            Serial.print("lighting: ");
            Serial.print(i);
            Serial.print("\n");                        
          }
          else {
            strip.setPixelColor(i, 0);

          }
        }
      strip.show();
}

void blink() {
     unsigned long currentMillis = millis();
    if (dryMode == true){
        interval = 330;
    }
    else{
        interval = 1000;
    }
    if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
}

void reverse() {
    if (rotation == false) {
    digitalWrite(reversePin, HIGH);
  } else {
    digitalWrite(reversePin, LOW);
  }
} //reverse

void singleclick() {
  if (drySpeed>23){
    drySpeed=1;
  }
  else{
    drySpeed++;
  }
} // click1

void doubleclick() {
  rotation = !rotation;
} // doubleclick

void longpress() {
dryMode = !dryMode;
} // longpress

void loop() { 
  button.tick();  // keep watching the push button:
  digitalWrite(enablePin,HIGH);
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 1023, 0, 235);     // scale it to use it with the servo (value between 0 and 180)
  if (dryMode == false){
         analogWrite(clearPathPin, val);
         light = map(val, 0, 235, 0, 23);    // determines the number of lights to energize
  }
  else{
         if (val <= 50){
          analogWrite(clearPathPin, drySpeed);   // remember if you change drySpeeds max, then this must be changed
          light=map(drySpeed,1,24,0,23) ;   
         }
         else{
          analogWrite(clearPathPin, 0);
          light=0;
         }
  }
  speedometer(strip.Color(r[light], g[light], b[light]), 5); // Red
  delay(10);                           // waits for the servo to get there
  blink();
  reverse();  




}




