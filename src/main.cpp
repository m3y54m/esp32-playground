#include <Arduino.h>

// GPIO Matrix will be used to assign any IO pin
// to the desired UART peripheral
#define MUXED_RXD 36
#define MUXED_TXD 32

void setup()
{
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(9600);
  // Serial1.begin(9600, SERIAL_8N1, MUXED_RXD, MUXED_TXD);
  Serial2.begin(9600, SERIAL_8N1, MUXED_RXD, MUXED_TXD);
  Serial.println("Serial2 Txd is on pin: " + String(MUXED_TXD));
  Serial.println("Serial2 Rxd is on pin: " + String(MUXED_RXD));
}

void loop()
{
  // Create a loopback on Serial2
  while (Serial2.available()) {
    Serial2.print(char(Serial2.read()));
  }
}