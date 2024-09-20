#include "mpucalc.h"
void MPU6050CALC::clock()
{
    short rax, ray, raz, rgx, rgy, rgz;
    getMotion6(&rax, &ray, &raz, &rgx, &rgy, &rgz);
    ax = rax / 8192.0 * 9.81;
    ay = ray / 8192.0 * 9.81;
    az = raz / 8192.0 * 9.81;
    gx = rgx / 131.0;
    gy = rgy / 131.0;
    gz = rgz / 131.0;
    vx += ax * deltaT;
    vy += ax * deltaT;
    vz += az * deltaT;
}