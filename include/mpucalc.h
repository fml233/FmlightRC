#include <Arduino.h>
#include "MPU6050.h"
#include "hal.h"
class MPU6050CALC : public MPU6050
{
    private:
    int status = 0;
    double acc_x, acc_y, acc_z;
    double vel_x, vel_y, vel_z;
    double ang_x, ang_y, ang_z;
    double forward_vel;
    double x, y, z;
    hw_timer_t* timer;
    const double deltaT = 1 / MPU6050_READ_FREQUENCY;
    volatile bool lock = false;
    void getLock()
    {
        while(lock);
        lock = true;
    }
    void freeLock()
    {
        lock = false;
    }

    public:
    MPU6050CALC(uint8_t address, void (*clockFn)())
    {
        initialize();
        if(testConnection())
        {
            status = 1;
            timer = timerBegin(0, 80, true);
            timerAttachInterrupt(timer, clockFn, true);
            timerAlarmWrite(timer, deltaT, true);
            timerAlarmEnable(timer); 
            setFullScaleAccelRange(MPU6050_ACCEL_FS_4);
            setFullScaleGyroRange(MPU6050_GYRO_FS_250);
        }
    }
    void clock();
    int getState(){return status;}
    //此函数返回以*加速度计*为参照系的三轴加速度
    void getAcc(double* x, double* y, double* z);
    //此函数返回以*陀螺仪*为参照系的三轴角度
    void getAng(double* x, double* y, double* z);
    //此函数返回以*地面*为参照系的三轴速度
    void getVel(double* x, double* y, double* z);
    //此函数返回*向前*的速度
    void getForwardVelocity(double* v);
    void reset()
    {
        getLock();
        acc_x = acc_y = acc_z = 0;
        ang_x = ang_y = ang_z = 0;
        vel_x = vel_y = vel_z = 0;
        forward_vel = 0;
        x = y = z = 0;
        freeLock();
    }
};