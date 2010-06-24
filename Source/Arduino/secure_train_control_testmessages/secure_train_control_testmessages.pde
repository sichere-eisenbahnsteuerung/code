// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

#include <Wire.h>

void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
}

void loop()
{
  Wire.beginTransmission(4); // transmit to device #4
  //Test Message  
  Wire.send(B00000000);    //Nachricht von Leitzentrale                 Byte 0
  Wire.send(B00110010);    //Weichenstellung unbekannt / Loknummer 1    Byte 1
  Wire.send(B00000000);    //Zustand Lok#1 fahrend                      Byte 2
  Wire.send(B00000001);    //Zustand Lok#2 wartend                      Byte 3
  Wire.send(B00000000);    //Position Lok#1                             Byte 4
  Wire.send(B00000000);    //Position Lok#2                             Byte 5
  Wire.send(B00000000);    //Fahrbefehl                                 Byte 6
   //Test Message  
  Wire.send(B00000000);    //Nachricht von Leitzentrale                 Byte 0
  Wire.send(B00010010);    //Weichenstellung unbekannt / Loknummer 1    Byte 1
  Wire.send(B00000000);    //Zustand Lok#1 fahrend                      Byte 2
  Wire.send(B00000001);    //Zustand Lok#2 wartend                      Byte 3
  Wire.send(B00000000);    //Position Lok#1                             Byte 4
  Wire.send(B00000000);    //Position Lok#2                             Byte 5
  Wire.send(B00000000);    //Fahrbefehl                                 Byte 6
    //Test Message  
  Wire.send(B00000000);    //Nachricht von Leitzentrale                 Byte 0
  Wire.send(B00110010);    //Weichenstellung unbekannt / Loknummer 1    Byte 1
  Wire.send(B00000000);    //Zustand Lok#1 fahrend                      Byte 2
  Wire.send(B00000001);    //Zustand Lok#2 wartend                      Byte 3
  Wire.send(B00000000);    //Position Lok#1                             Byte 4
  Wire.send(B00000000);    //Position Lok#2                             Byte 5
  Wire.send(B00000000);    //Fahrbefehl                                 Byte 6
   //Test Message  
  Wire.send(B00000000);    //Nachricht von Leitzentrale                 Byte 0
  Wire.send(B00010010);    //Weichenstellung unbekannt / Loknummer 1    Byte 1
  Wire.send(B00000000);    //Zustand Lok#1 fahrend                      Byte 2
  Wire.send(B00000001);    //Zustand Lok#2 wartend                      Byte 3
  Wire.send(B00000000);    //Position Lok#1                             Byte 4
  Wire.send(B00000000);    //Position Lok#2                             Byte 5
  Wire.send(B00000000);    //Fahrbefehl                                 Byte 6
     //Test Message  
  Wire.send(B00000000);    //Nachricht von Leitzentrale                 Byte 0
  Wire.send(B00010010);    //Weichenstellung unbekannt / Loknummer 1    Byte 1
  Wire.send(B00000000);    //Zustand Lok#1 fahrend                      Byte 2
  Wire.send(B00000001);    //Zustand Lok#2 wartend                      Byte 3
  Wire.send(B00000000);    //Position Lok#1                             Byte 4
  Wire.send(B00000000);    //Position Lok#2                             Byte 5
  Wire.send(B00000000);    //Fahrbefehl                                 Byte 6
  Wire.endTransmission();    // stop transmitting

  delay(4500);
}


