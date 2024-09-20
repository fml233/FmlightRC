#include <Arduino.h>
//硬件函数
extern void write24GHz(const byte* buffer, int len);//写2.4GHz模块
extern void read24GHz(byte* buffer, size_t buffer_size);//读2.4GHz模块