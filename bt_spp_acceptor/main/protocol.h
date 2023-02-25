#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>

/**
  * @brief  variable monitor，triggered when a variable changes
  * @param  now:variables to be monitored(int)
  * @param  func:the event triggers the callback function(it could be an assignment or something else)
  * @retval 
  */
#define __ValueMonitor(now,func)\
do{\
    static int last = 0;\
    if(last!=(now))func,last=(now);\
}while(0)

#define PROTOCOL_NUM_MAX (sizeof(Frame_Struct_TypeDef)/sizeof(uint8_t))

typedef struct
{
    unsigned char remoteControl;
} Frame_Struct_TypeDef;

typedef union
{
    Frame_Struct_TypeDef dat;
    unsigned char Data[PROTOCOL_NUM_MAX];
} Frame_Union_TypeDef;

enum
{
    FORWARD = 'A',
    RIGHT_FORWARD = 'B',
    RIGHT_ROTATION = 'C',
    RIGHT_BACKWARD = 'D',
    BACKWARD = 'E',
    LEFT_BACKWARD = 'F',
    LEFT_ROTATION = 'G',
    LEFT_FORWARD = 'H',
    BRAKE = 'Z',
    SPEED_UP = 'X',
    SPEED_DOWN = 'Y',
};

extern Frame_Union_TypeDef frame;

void protocol_init(void);
void Protocol_Process(uint8_t data);

#endif

