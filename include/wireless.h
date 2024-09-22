#include "hal.h"
void send(const byte* buffer, int len)
{
    for(int i = 0; i < len; i++)
    {
        HalWrite24GHz(buffer[i]);
    }
    HalWrite24GHz(0x5A);
}
void recv(byte* buffer, int size)
{
    for(int i = 0; i < size; i++)
    {
        byte b = 0;
        HalRead24GHz(&b);
        if(b == 0x5A) break;
        buffer[i] = b;
    }
}