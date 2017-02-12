//
//    FILE: oneWireSearch.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.02
// PURPOSE: scan for 1-Wire devices + code snippet generator
//    DATE: 2015-june-30
//     URL: http://forum.arduino.cc/index.php?topic=333923
//
// inspired by http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html
//
// Released to the public domain
//
// 0.1.00 initial version
// 0.1.01 first published version
// 0.1.02 small output changes

#include <OneWire.h>
#include <Console.h>    // from ConsoleRead library

void setup()
{
//  Serial.begin(115200);
//  Serial.println("//\n// Start oneWireSearch.ino \n//");

  Bridge.begin();      // from ConsoleRead library
  Console.begin();     // from ConsoleRead library
  while (!Console);    // from ConsoleRead library

  for (uint8_t pin = 1; pin < 13; pin++)  // changed "pin < 13" to "pin <3"
  {
    findDevices(pin);
  }
  Console.println("\n//\n// End oneWireSearch.ino \n//");   //change "Serial.println" to "Console.println"
}

void loop()
{
}

uint8_t findDevices(int pin)
{
  OneWire ow(pin);

  uint8_t address[8];
  uint8_t count = 0;

  if (ow.search(address))
  {
    Console.print("\nuint8_t pin");      // chaned all Serial.print to Console.print
    Console.print(pin, DEC);
    Console.println("[][8] = {");
    {
      count++;
      Console.println("  {");
      for (uint8_t i = 0; i < 8; i++)
      {
        Console.print("0x");
        if (address[i] < 0x10) Serial.print("0");
        Console.print(address[i], HEX);
        if (i < 7) Console.print(", ");
      }
      Console.println("  },");
    } while (ow.search(address));

    Console.println("};");
    Console.print(" devices found: ");
    Console.println(count);
  }

  return count;
}
