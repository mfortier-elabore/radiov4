#ifndef WF100D_H
#define WF100D_H
#include <Wire.h>
#define WF100D_ADDRESS 0x6D
#define WF100D_CONFIG_REG 0x30
#define WF100D_PRESSURE_REG 0x06

#define SDA_PIN A4
#define SCL_PIN A5

void WF100D_init();
float getPression();
void doScanToFindAddress(int *foundAddress);

#endif // WF100D_H