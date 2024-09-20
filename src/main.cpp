#include <Arduino.h>
#include "hal.h"
#include "navigation.h"
#include "mpucalc.h"
MPU6050CALC mpu6050;
// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  HalInitHardware();
}

void loop() {
  // put your main code here, to run repeatedly:
  mpu6050.clock();
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

void doClock()
{
  mpu6050.clock();
}