#include <Arduino.h>
//硬件函数
//初始化硬件
extern int HalInitHardware();
//写2.4GHz模块
extern void HalWrite24GHz(const byte* buffer, int len);
//读2.4GHz模块
extern void HalRead24GHz(byte* buffer, int buffer_size);
//读导航模块
extern byte HalReadNav(byte* buffer, int buffer_size);
//硬件常量
//陀螺仪数据读取频率
const int MPU6050_READ_FREQUENCY = 100;