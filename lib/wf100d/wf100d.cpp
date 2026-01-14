#include "wf100d.h"

void WF100D_init()
{
    Wire.setSDA(SDA_PIN);
    Wire.setSCL(SCL_PIN);
    Wire.setClock(400000); // 400kHz
    Wire.begin();
}

float getPression()
{
    uint32_t low, med, high;
    uint32_t wert;
    float fadc;
    float druck;

    Serial.print("Using I2C address: 0x");
    Serial.print(WF100D_ADDRESS, HEX);
    Serial.println(", reading sensor data...");
    Wire.beginTransmission(WF100D_ADDRESS);
    Wire.write(WF100D_CONFIG_REG);
    Wire.write(0b00001010); // 00001010 : Trigger one pressure measurement
    Wire.endTransmission();
    delay(10);

    Wire.beginTransmission(WF100D_ADDRESS);
    Wire.write(WF100D_PRESSURE_REG);
    Wire.endTransmission(false);

    Wire.beginTransmission(WF100D_ADDRESS);
    Wire.requestFrom(WF100D_ADDRESS, (uint8_t)3, 1);
    high = Wire.read();
    med = Wire.read();
    low = Wire.read();

    wert = (high << 16) | (med << 8) | low;
    Serial.println(low, HEX);
    Serial.println(med, HEX);
    Serial.println(high, HEX);
    Serial.print("Value read: ");
    Serial.println(wert);

    if (wert & 0x800000)
    {
        fadc = wert - 16777216.0;
    }
    else
    {
        fadc = wert;
    }
    druck = 500 * ((3.3 * wert / 8388608.0) - 0.5) / 200.0;
    Serial.print("Druck [bar]: ");
    Serial.println(druck);

    Wire.endTransmission();
    return druck;
}

void doScanToFindAddress(int *foundAddress)
{
  Serial.println("Scanning I2C bus for devices...");
  int nDevices = 0;
  for (uint8_t address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    Wire.setClock(50000); // 50kHz
    uint8_t error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      nDevices++;
      *foundAddress = address;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}