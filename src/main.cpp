#include <Arduino.h>
#include "hal.h"
#include "navigation.h"
#include "mpucalc.h"
void doClock();
MPU6050CALC mpu6050(MPU6050_DEFAULT_ADDRESS, doClock);
void setup() {
  // put your setup code here, to run once:
  HalInitHardware();
}

void loop() {
  // put your main code here, to run repeatedly:
  mpu6050.clock();
}

void doClock()
{
  mpu6050.clock();
}