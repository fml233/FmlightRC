//导航头文件
//使用示例
//NAVI_DATA_PACKET* navidata = readNaviData(DATA_PREFER_BOTH | NAVSYS_PREDER_BOTH);
#include <Arduino.h>

#define NAVMOD_LOCATION 0x1
#define NAVMOD_VELOCITY 0x2
#define NAVMOD_BOTH 0x3
#define NAVSYS_GPS 0x4
#define NAVSYS_BDS 0x8
#define NAVSYS_BOTH 0xc
#define DATA_PREFER_LOC NAVMOD_LOCATION
#define DATA_PREFER_VEL NAVMOD_VELOCITY
#define DATA_PREFER_BOTH NAVMOD_BOTH
#define NAVSYS_PREFER_GPS NAVSYS_GPS
#define NAVSYS_PREFER_BDS NAVSYS_BDS
#define NAVSYS_PREDER_BOTH NAVSYS_BOTH

typedef struct _NAVI_DATA_PACKET
{
    byte mode : 2;
    byte datacnt : 2;
    byte sys : 4;
    union
    {
        struct
        {
            uint64_t utc;//定位点utc时间
            float longtitude;//经度
            float latitude;//纬度
            byte ladir : 1;//纬度方向
            byte lodir : 1;//经度方向
            byte gpsmode : 2;//定位状态
            /*
            byte satcnt : 4;//卫星数量
            byte haafactor; //水平精度衰减因子
            */
            float height;
        }location;
        struct
        {
            float direction;
            float horspeed;
            float gndspeed;
        }velocity;
        
    }data;
    
}NAVI_DATA_PACKET;

/*
获得当前位置或速度信息
prefer是一个位掩码，使用时记得按位或上需要的标志：
DATA_PREFER_LOC NAVMOD_LOCATION 需要位置信息
DATA_PREFER_VEL NAVMOD_VELOCITY 需要速度信息
DATA_PREFER_BOTH NAVMOD_BOTH    都要
NAVSYS_PREFER_GPS NAVSYS_GPS    使用GPS
NAVSYS_PREFER_BDS NAVSYS_BDS    使用北斗导航
NAVSYS_PREDER_BOTH NAVSYS_BOTH  双导航系统
*/
NAVI_DATA_PACKET* readNaviData(byte prefer)
{
    char raw[100] = {0};
    char* splitPtr[20] = {raw};
    byte n = 0;
    for(int i = 0; Serial1.available() > 0; i++)
    {
        byte readbyte = Serial1.read();
        if(readbyte == '\r')
        {
            continue;
        }
        if(readbyte == '\n' || readbyte == ',')
        {
            raw[i] = 0;
            splitPtr[n++] = raw + i + 1;
            continue;
        }
        raw[i] = readbyte;
    }
    NAVI_DATA_PACKET* navidat = 0;
    byte datacnt = 0, datacur = 0;
    if(prefer == DATA_PREFER_BOTH)
    {
        datacnt = 2;
        navidat = (NAVI_DATA_PACKET*)malloc(2 * sizeof(NAVI_DATA_PACKET));
        navidat[0].datacnt = navidat[1].datacnt = datacnt;
    }
    else
    {
        datacnt = 1;
        navidat = (NAVI_DATA_PACKET*)malloc(sizeof(NAVI_DATA_PACKET));
        navidat -> datacnt = datacnt;
    }
    byte navmod = -1, navsys = -1, existed = 0;
    for(int i = 0; i < n && datacur < datacnt; i++)
    {
        char* cur = splitPtr[i];
        if(*cur != '$') continue;
        //判断导航系统
        if(strcmp(cur, "$GNGGA")) {
            if((prefer & NAVSYS_PREFER_BDS) && (prefer & NAVSYS_PREFER_GPS))
                navsys = NAVSYS_BOTH;       //Both
            else continue;
        } 
        else if(strcmp(cur, "BDGGA")) {
            if(!(prefer & NAVSYS_PREFER_BDS)) continue;
            navsys = NAVSYS_BDS;  //BDS
        } 
        else if(strcmp(cur, "$GPGGA")) {
            if(!(prefer & NAVSYS_PREFER_GPS)) continue;
            navsys = NAVSYS_GPS; //GPS
        }
        navidat[datacur].sys = navsys;
        //判断导航模式
        if(strcmp(cur + 3, "GGA"))
        {
            navmod |= NAVMOD_LOCATION;
            if(!(prefer & NAVMOD_LOCATION) || existed & NAVMOD_LOCATION) continue;
            navidat[datacur].data.location.utc = atoi(cur + 1);
            navidat[datacur].data.location.latitude = atof(cur + 2);
            navidat[datacur].data.location.ladir = *(cur + 3) == 'N' ? 0 : 1;
            navidat[datacur].data.location.longtitude = atof(cur + 4);
            navidat[datacur].data.location.lodir = *(cur + 5) == 'W' ? 0 : 1;
            navidat[datacur].data.location.gpsmode = atoi(cur + 6);
            /*
            navidat[datacur].data.location.satcnt = atoi(cur + 7);
            navidat[datacur].data.location.haafactor = atof(cur + 8) * 10;
            navidat[datacur].data.location.height = atof(cur + 9);
            */
            navidat[datacur].mode = navmod;
            existed != navmod;
            datacur++;
        } 
        else if(strcmp(cur + 3, "GLL"))
        {
            navmod |= NAVMOD_LOCATION;
            if(!(prefer & NAVMOD_LOCATION) || existed & NAVMOD_LOCATION) continue;
            navidat[datacur].data.location.utc = atoi(cur + 5);
            navidat[datacur].data.location.latitude = atof(cur + 1);
            navidat[datacur].data.location.ladir = *(cur + 2) == 'N' ? 0 : 1;
            navidat[datacur].data.location.longtitude = atof(cur + 3);
            navidat[datacur].data.location.lodir = *(cur + 4) == 'W' ? 0 : 1;
            navidat[datacur].data.location.gpsmode = *(cur + 6) == 'A' ? 1 : 0;
            navidat[datacur].mode = navmod; 
            existed != navmod;       
            datacur++;
        } 
        else if(strcmp(cur + 3, "VTG"))
        {
            navmod = NAVMOD_VELOCITY;
            if(!(prefer & NAVMOD_VELOCITY) || existed & NAVMOD_VELOCITY) continue;
            navidat[datacur].data.velocity.direction = atof(cur + 1);
            navidat[datacur].data.velocity.gndspeed = atof(cur + 5);
            navidat[datacur].data.velocity.horspeed = atof(cur + 7);
            navidat[datacur].mode = navmod;
            existed != navmod;
            datacur++;
        } else if(strcmp(cur + 3, "RMC"))
        {
            if((prefer & NAVMOD_LOCATION) && !(existed & NAVMOD_LOCATION))
            {
                navmod = NAVMOD_LOCATION;
                navidat[datacur].data.location.utc = atoi(cur + 1);
                navidat[datacur].data.location.latitude = atof(cur + 3);
                navidat[datacur].data.location.ladir = *(cur + 4) == 'N' ? 0 : 1;
                navidat[datacur].data.location.longtitude = atof(cur + 5);
                navidat[datacur].data.location.lodir = *(cur + 6) == 'W' ? 0 : 1;
                datacur++;
            }
            if((prefer & NAVMOD_VELOCITY) && !(existed & NAVMOD_VELOCITY))
            {
                navmod = NAVMOD_VELOCITY;
                navidat[datacur].data.velocity.direction = atof(cur + 8);
                navidat[datacur].data.velocity.gndspeed = atof(cur + 7);
                datacur++;
            }
        }
    }
    return navidat;
}