//导航头文件
//使用示例
//NAVI_DATA_PACKET* navidata = readNaviData(DATA_PREFER_BOTH | NAVSYS_PREDER_BOTH);
#include "hal.h"

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
    _NAVI_DATA_PACKET* next;
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
    
}NAVI_DATA_PACKET, *PNAVI_DATA_PACKET;

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
NAVI_DATA_PACKET* readNaviData()
{
    char raw[100] = {0};
    char* splitPtr[20] = {raw};
    byte n = 0;
    byte rawLen = HalReadNav((byte*)raw, sizeof(raw));
    for(int i = 0; i < rawLen; i++)
    {
        if(raw[i] == ',')
        {
            raw[i] = 0;
            splitPtr[++n] = raw + i + 1;
        }
    }
    PNAVI_DATA_PACKET head = (PNAVI_DATA_PACKET)malloc(sizeof(NAVI_DATA_PACKET)), prepacket = head, curpacket = 0;
    byte datacnt = 0, datacur = 0;
    byte navmod = -1, navsys = -1, existed = 0;
    for(int i = 0; i < n; i++)
    {
        char* cur = splitPtr[i];
        if(*cur != '$') continue;
        curpacket = (PNAVI_DATA_PACKET)malloc(sizeof(NAVI_DATA_PACKET));
        prepacket->next = curpacket;
        prepacket = curpacket;
        //判断导航系统
        if(strcmp(cur, "$GNGGA")) {
            navsys = NAVSYS_BOTH;       //都有
        } 
        else if(strcmp(cur, "BDGGA")) {
            navsys = NAVSYS_BDS;  //BDS
        } 
        else if(strcmp(cur, "$GPGGA")) {
            navsys = NAVSYS_GPS; //GPS
        }
        curpacket->sys = navsys;
        //判断导航模式
        if(strcmp(cur + 3, "GGA"))
        {
            navmod |= NAVMOD_LOCATION;
            curpacket->data.location.utc = atoi(cur + 1);
            curpacket->data.location.latitude = atof(cur + 2);
            curpacket->data.location.ladir = *(cur + 3) == 'N' ? 0 : 1;
            curpacket->data.location.longtitude = atof(cur + 4);
            curpacket->data.location.lodir = *(cur + 5) == 'W' ? 0 : 1;
            curpacket->data.location.gpsmode = atoi(cur + 6);
            curpacket->mode = navmod;
        } 
        else if(strcmp(cur + 3, "GLL"))
        {
            navmod |= NAVMOD_LOCATION;
            curpacket->data.location.utc = atoi(cur + 5);
            curpacket->data.location.latitude = atof(cur + 1);
            curpacket->data.location.ladir = *(cur + 2) == 'N' ? 0 : 1;
            curpacket->data.location.longtitude = atof(cur + 3);
            curpacket->data.location.lodir = *(cur + 4) == 'W' ? 0 : 1;
            curpacket->data.location.gpsmode = *(cur + 6) == 'A' ? 1 : 0;
            curpacket->mode = navmod;
        } 
        else if(strcmp(cur + 3, "VTG"))
        {
            navmod |= NAVMOD_VELOCITY;
            curpacket->data.velocity.direction = atof(cur + 1);
            curpacket->data.velocity.gndspeed = atof(cur + 5);
            curpacket->data.velocity.horspeed = atof(cur + 7);
            curpacket->mode = navmod;
        } else if(strcmp(cur + 3, "RMC"))
        {
            navmod |= NAVMOD_LOCATION;
            curpacket->data.location.utc = atoi(cur + 1);
            curpacket->data.location.latitude = atof(cur + 3);
            curpacket->data.location.ladir = *(cur + 4) == 'N' ? 0 : 1;
            curpacket->data.location.longtitude = atof(cur + 5);
            curpacket->data.location.lodir = *(cur + 6) == 'W' ? 0 : 1;
            navmod |= NAVMOD_VELOCITY;
            curpacket->data.velocity.direction = atof(cur + 8);
            curpacket->data.velocity.gndspeed = atof(cur + 7);
        }
    }
    return head;
}