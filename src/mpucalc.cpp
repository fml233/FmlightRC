#include "mpucalc.h"
void MPU6050CALC::clock()
{
    float g = 9.81;
    short rax, ray, raz, rgx, rgy, rgz;
    getMotion6(&rax, &ray, &raz, &rgx, &rgy, &rgz);

    acc_x = rax / 8192.0 * g;
    acc_y = ray / 8192.0 * g;
    acc_z = raz / 8192.0 * g;
    forward_vel = acc_y * deltaT;

    ang_x += rgx / 131.0 * deltaT;
    ang_y += rgy / 131.0 * deltaT;
    ang_z += rgz / 131.0 * deltaT;

    double R[3][3];
    R[0][0] = cos(ang_y) * cos(ang_z);
    R[0][1] = cos(ang_z) * sin(ang_x) * sin(ang_y) - cos(ang_x) * sin(ang_z);
    R[0][2] = sin(ang_x) * sin(ang_z) + cos(ang_x) * cos(ang_z) * sin(ang_y);
    R[1][0] = cos(ang_y) * sin(ang_z);
    R[1][1] = cos(ang_x) * cos(ang_z) + sin(ang_x) * sin(ang_y) * sin(ang_z);
    R[1][2] = cos(ang_x) * sin(ang_y) * sin(ang_z) - cos(ang_z) * sin(ang_x);
    R[2][0] = -sin(ang_y);
    R[2][1] = cos(ang_y) * sin(ang_x);
    R[2][2] = cos(ang_x) * cos(ang_y);

    double global_accel_x = R[0][0] * acc_x + R[0][1] * acc_y + R[0][2] * acc_z;
    double global_accel_y = R[1][0] * acc_x + R[1][1] * acc_y + R[1][2] * acc_z;
    double global_accel_z = R[2][0] * acc_x + R[2][1] * acc_y + R[2][2] * acc_z;

    vel_x += global_accel_x * deltaT;
    vel_y += global_accel_y * deltaT;
    vel_z += global_accel_z * deltaT;

    x += vel_x * deltaT;
    y += vel_y * deltaT;
    z += vel_z * deltaT;
}
void MPU6050CALC::getAcc(double* x, double* y, double* z)
{
    *x = acc_x;
    *y = acc_y;
    *z = acc_z;
}
void MPU6050CALC::getVel(double* x, double* y, double* z)
{
    *x = vel_x;
    *y = vel_y;
    *z = vel_z;
}
void MPU6050CALC::getAng(double* x, double* y, double* z)
{
    *x = ang_x;
    *y = ang_y;
    *z = ang_z;
}
void MPU6050CALC::getForwardVelocity(double* v)
{
    *v = forward_vel;
}