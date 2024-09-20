#include <Arduino.h>
//硬件函数
extern void HalWrite24GHz(const byte* buffer, int len);//写2.4GHz模块
extern void HalRead24GHz(byte* buffer, int buffer_size);//读2.4GHz模块
extern byte HalReadNav(byte* buffer, int buffer_size);