//Power Wrapper Controller 
//Brent Kinser
//brentkinser@icloud.com
//August 2017
#include <OneButton.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>   // for breathe routine
#define PIN 6

const int ledPin =  9;  // Led on key and power switch.
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;

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
int throttle=0;  //mapped value derived from val
int turboCut=80;  //End of the Pedal span;  point at which turbo kicks in
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

int r[] = {  0,  0, 51,153,255,255,204,204,255,255,153, 51,204,153,102, 51,  0,  0,  0,  0, 51,102,153,204,255};
int g[] = {  0,  0, 51,153,204,153,102,204,255,255,255,255,255,255,255,255,255,204,204,255,255,255,255,255,204};
int b[] = {204,204,255,255,153, 51,  0,  0, 51,153,255,255,204,153,102, 51,  0,  0,204,255,255,255,255,255,259};

int rr[] = {153,153,204,255,255,255,255,255,255,255,255,204,153,102, 51,  0,  0,  0,  0, 51,102,153,204,255,255};
int gg[] = {  0,  0,  0,  0, 51,153,204,255,255,255,255,255,255,255,255,255,204,204,255,255,255,255,255,204,153};
int bb[] = {  0,  0,  0,  0, 51,153,204,204,153,102, 51,204,153,102, 51,  0,  0,204,255,255,255,255,255,259,204};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(reversePin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(ledPin, OUTPUT);
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
          }
          else {
            strip.setPixelColor(i, 0);
          }
        }
      strip.show();
}

void breathe() {            //  LIGHT BREATHING FUNCTION                    
  // set the brightness of pin 9:
  analogWrite(ledPin, brightness);

  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0 || brightness >= 105) {
    fadeAmount = -fadeAmount;
  }
  // wait for 30 milliseconds to see the dimming effect
  strip.setBrightness(brightness);
}


void setRotation(){                                 // HANDLES THE DIFFERENT ROTATION MODES
  
    if (rotation == false) {         //  FORWARED ROTATION DIRECTION
       digitalWrite(reversePin, HIGH);
       speedometer(strip.Color(r[light], g[light], b[light]), 5); //  paint speedometer with FORWARD rotation color set
       analogWrite(ledPin, 255);   // Led Lights OFF
    } 
    else {
       digitalWrite(reversePin, LOW);   //  REVERSE ROTATION DIRECTION
       analogWrite(ledPin, 0);   // Led Lights ON
       speedometer(strip.Color(rr[light], gg[light], bb[light]), 5); //  paint speedometer with REVERSE rotation color set 
    }
}
void singleclick() {
  rotation = !rotation;                                        //   SWITCH THE ROTATION DIRECTION 
} //                    singleclick: reverses rotation

void doubleclick() {
    if (drySpeed>23){
    drySpeed=1;
  }                                                                                                                 
  else{
    drySpeed++;
  }
} //                    doubleclick: increases dry speed

void longpress() {
dryMode = !dryMode;
} //                    longpress: toggles drying mode

void loop() { 
  button.tick();  // keep watching the push button:
  digitalWrite(enablePin,HIGH);
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  throttle = map(val, 0, 1023, 0, turboCut);     // scale it to use it with the servo (value between 0 and 180)
  setRotation();

  //  DRY MODE LOGIC
  if (dryMode == false){             // NORMAL PEDAL DRIVEN WRAPPING MODE IS ACTIVE
         analogWrite(clearPathPin, throttle);
         if (throttle < turboCut - 10);{
            light = map(throttle, 0, turboCut, 0, 23);    // determines the number of lights to energize
         }
         if (throttle > turboCut - 10){
            analogWrite(clearPathPin, 250);
         }
  }
  else{                             //  DRYING MODE IS ACTIVE
         if (throttle <= 50){                       // PEDAL IS NOT TOUCHED;  DRYING MODE IS GOOD TO GO
          analogWrite(clearPathPin, drySpeed);   // remember if you change drySpeeds max, then this must be changed
          light=map(drySpeed,1,24,0,23) ; 
          breathe();
         }      
         else{                                 // PEDAL HAS BEEN STOMPED ON;   STOP DRYING
          analogWrite(clearPathPin, 0);
          light=0;
          analogWrite(ledPin, 0);
         }
  }
  
  delay(10);                           // PROGRAM DELAY


}




