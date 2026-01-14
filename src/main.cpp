#include <Arduino.h>
#include <Wire.h>
#include "wf100d.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("Radio v4");
  delay(100);
  WF100D_init();
}

void loop()
{

  // Lire pression du wf100d
  float pression = getPression();
  Serial.print("Pression: ");
  Serial.print(pression);
  Serial.println(" bar");
  delay(5000);
}