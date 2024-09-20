#include "hal.h"
int HalInitHardware()
{

}

void HalWrite24GHz(const byte* buffer, int len)
{

}

void HalRead24GHz(byte* buffer, int buffer_size)
{

}

byte HalReadNav(byte* buffer, int buffer_size)
{
    int i = 0;
    while(Serial1.available() > 0)
    {
        byte readbyte = Serial1.read();
        if(readbyte == '\r')
        {
            continue;
        }
        if(readbyte == '\n')
        {
            buffer[i] = ',';
            continue;
        }
       buffer[i] = readbyte;
        i++;
    }
    return i;
}