//                                     yun_eighteen_neopixel                           Mon Feb 6, 2017
//                                                                                                      Brent Kinser

// This Arduino sketch controls a vertical string of neopixels, as the temperature 
// rises or falls a higher or lower neopixel is lit


#include <OneWire.h>
#include <DallasTemperature.h>
#include <Console.h> 
#include <Adafruit_NeoPixel.h>
#define PIN 6
#define ONE_WIRE_BUS 10

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

DeviceAddress insideThermometer = { 0x28, 0xFE, 0x19, 0xBE, 0x4, 0x0, 0x0, 0xEA };


float tempF=1.1;
int tempFint=1;
int lightVal=0;

unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 2000;           // interval at which to blink (milliseconds)


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

void setup(void)
{

  Bridge.begin();      // from ConsoleRead library
  Console.begin();     // from ConsoleRead library
  while (!Console);    // from ConsoleRead library
  
  sensors.begin();
  sensors.setResolution(insideThermometer, 10);

   strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}





void printTemperature(DeviceAddress deviceAddress) {
  delay(10);
  float tempC = sensors.getTempC(deviceAddress);
  delay(50);
  if (tempC == -127.00) {
    Console.print("Error getting temperature");
  } else {
    Console.print("Getting temperatures...\n\r");   
    Console.print("Inside temperature is: ");
    Console.print("C: ");
    Console.print(tempC);
    Console.print(" F: ");
    tempF=DallasTemperature::toFahrenheit(tempC);
    Console.print(tempF);
    Console.print("\n");
    tempFint=tempF*100;
    Console.print("tempFint = ");
    Console.print(tempFint);
    Console.print("\n");
    lightVal = map(tempFint, 7350, 7550, 0, 8);
    if (lightVal>8) {
      lightVal=8;
    }
    Console.print("lightVal = ");
    Console.print(lightVal);
    Console.print("\n");
    Console.print("17 - lightVal = ");
    Console.print(17-lightVal);
    Console.print("\n\n");
  }
}

//Lights rise with temperature.
void thermoRise(uint32_t c, uint8_t wait) {
        for (int i=0;i<9; i++) {                            // 9 is the number of pixels in one column
          if (lightVal>=i)  {
            delay(10);
            strip.setPixelColor(i, c);             
            delay(10);
            strip.setPixelColor(17-i,c);
            Console.print("lighting: ");
            Console.print(i);
            Console.print(",");
            Console.print(17-i);
            Console.print("\n");            
          }
          else {
            strip.setPixelColor(i, 0);
            strip.setPixelColor(17-i,0);
            Console.print("blanking: ");
            Console.print(i);
            Console.print(",");
            Console.print(17-i);
            Console.print("\n");
          }
        }
      strip.show();
}

void loop(void)
{ 

    unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sensors.requestTemperatures();
    printTemperature(insideThermometer);
     thermoRise(strip.Color(r[lightVal], g[lightVal], b[lightVal]), 5); // Red
  }
  
}
