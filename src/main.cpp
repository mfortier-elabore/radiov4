#include <Arduino.h>
#include <Wire.h>
#include "wf100d.h"
#include "radio.h"

// Set to true for transmitter, false for receiver
#define IS_TRANSMITTER false

void setup()
{
  Serial.begin(115200);
  Serial.println(IS_TRANSMITTER ? "Radio v4 - TRANSMITTER" : "Radio v4 - RECEIVER");
  delay(100);
  
  WF100D_init();
  radio_init();
}

void loop()
{
  if (IS_TRANSMITTER) {
    // Read sensor and transmit
    float pression = getPression();
    Serial.print("Pression: ");
    Serial.print(pression);
    Serial.println(" bar");
    
    // Convert float to string (snprintf %f doesn't work on STM32)
    char buffer[32];
    int intPart = (int)pression;
    int decPart = (int)((pression - intPart) * 100);
    if (decPart < 0) decPart = -decPart;
    snprintf(buffer, sizeof(buffer), "Pressure: %d.%02d bar", intPart, decPart);
    
    radio_transmit(buffer);
    delay(5000);
    
  } else {
    // Listen for incoming data
    if (radio_available()) {
      uint8_t buffer[256];
      size_t len = 256;
      if (radio_receive(buffer, &len)) {
        Serial.print("Data: ");
        for (size_t i = 0; i < len; i++) {
          Serial.print((char)buffer[i]);
        }
        Serial.println();
      }
    }
    delay(100);
  }
}