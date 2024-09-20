#include <Arduino.h>
#include "MPU6050.h"
#include "hal.h"
class MPU6050CALC : public MPU6050
{
    int status = 0;
    double ax, ay, az, gx, gy, gz;
    double vx, vy, vz;
    double xx, xy, xz;
    double pitch, roll, yaw;
    hw_timer_t* timer;
    const double deltaT = 1 / MPU6050_READ_FREQUENCY;
    public:
    MPU6050CALC(uint8_t address = MPU6050_DEFAULT_ADDRESS, void (*clockFn)())
    {
        MPU6050(address);
        initialize();
        if(testConnection())
        {
            status = 1;
            timer = timerBegin(0, 80, true);
            timerAttachInterrupt(timer, clockFn, true);
            timerAlarmWrite(timer, readInterval, true);
            timerAlarmEnable(timer); 
            setFullScaleAccelRange(MPU6050_ACCEL_FS_4);
            setFullScaleGyroRange(MPU6050_GYRO_FS_250);
        }
    }
    void clock();
    int getState(){return status;}
};