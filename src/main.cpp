#include <Arduino.h>
#include <Wire.h>
#include "wf100d.h"
#include "radio.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("Radio v4");
  delay(100);
  WF100D_init();
  radio_init();
  radio_transmit("Hello World");
}

void loop()
{

  // Lire pression du wf100d
  float pression = getPression();
  Serial.print("Pression: ");
  Serial.print(pression);
  Serial.println(" bar");
  delay(5000);

  if (radio_available()) {
      uint8_t buffer[256];
      size_t len = 256;
      if (radio_receive(buffer, &len)) {
          // Process received data
      }
  }
}