#include <Adafruit_NeoPixel.h>
#define PIN 6


int potpin = 8;  // analog pin used to connect the potentiometer
int clearPathPin = 10;    // LED connected to digital pin 9
int val;    // variable to read the value from the analog pin
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

int r[] = {5,0,0,0,255,255,255,255,255 };
int g[] = {0,116,255,255,255,120,0,0,14};
int b[] = {255,255,244,16,0,0,0,112,240};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  Serial.print("Serial comms initialized \n\n");
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void speedometer(uint32_t c, uint8_t wait) {
        for (int i=0;i<16; i++) {                            // 9 is the number of pixels in the ring
          if (light>=i)  {
            delay(10);
            strip.setPixelColor(i, c);   
            Serial.print("lighting: ");
            Serial.print(i);
            Serial.print("\n");                  
          }
          else {
            strip.setPixelColor(i, 0);
            Serial.print("blanking: ");
            Serial.print(i);
            Serial.print("\n");
          }
        }
      strip.show();
}

void loop() {
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  light = map(val, 0, 1023, 0, 15);    // determines the number of lights to energize
  val = map(val, 0, 1023, 0, 235);     // scale it to use it with the servo (value between 0 and 180)
  analogWrite(clearPathPin, val);
  speedometer(strip.Color(r[light], g[light], b[light]), 5); // Red
  delay(15);                           // waits for the servo to get there


}




