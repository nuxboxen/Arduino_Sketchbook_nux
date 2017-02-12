// This Arduino sketch reads DS18B20 "1-Wire" digital
// temperature sensors.
// Tutorial:
// http://www.hacktronics.com/Tutorials/arduino-1-wire-tutorial.html

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Console.h>    // from ConsoleRead library

// Data wire is plugged into pin 3 on the Arduino
#define ONE_WIRE_BUS 10

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

DeviceAddress insideThermometer = { 0x28, 0xFE, 0x19, 0xBE, 0x4, 0x0, 0x0, 0xEA };


float tempF=1.1;
int tempFint=1;

void setup(void)
{

  Bridge.begin();      // from ConsoleRead library
  Console.begin();     // from ConsoleRead library
  while (!Console);    // from ConsoleRead library
  
  sensors.begin();
  sensors.setResolution(insideThermometer, 10);
}





void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
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
    Console.print("\n\n");
  }
}




void loop(void)
{ 
  delay(2000);
  sensors.requestTemperatures();
  printTemperature(insideThermometer);
  
}

