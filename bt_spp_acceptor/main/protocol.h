#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>

/**
 * @brief  variable monitor，triggered when a variable changes
 * @param  now:variables to be monitored(int)
 * @param  func:the event triggers the callback function(it could be an assignment or something else)
 * @retval
 */
#define __ValueMonitor(now, func) \
    do                            \
    {                             \
        static int last = 0;      \
        if (last != (now))        \
            func, last = (now);   \
    } while (0)

#define PROTOCOLA1_NUM_MAX (sizeof(FrameA1_Struct_TypeDef) / sizeof(uint8_t))
#define PROTOCOLA2_NUM_MAX (sizeof(FrameA2_Struct_TypeDef) / sizeof(uint8_t))

typedef struct
{
    unsigned char remoteControl;
} FrameA1_Struct_TypeDef;

typedef union
{
    FrameA1_Struct_TypeDef dat;
    unsigned char Data[PROTOCOLA1_NUM_MAX];
} FrameA1_Union_TypeDef;

typedef struct
{
    uint8_t internetSelect;
} FrameA2_Struct_TypeDef;

typedef union
{
    FrameA2_Struct_TypeDef data;
    uint8_t unionData[PROTOCOLA2_NUM_MAX];
} FrameA2_Union_TypeDef;

enum
{
    INTERNET_OFF = 0,
    BLUETOOH,
    WIFI,
};

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

extern FrameA1_Union_TypeDef frameA1;
extern FrameA2_Union_TypeDef frameA2;

void protocol_init(void);
void Protocol_Process(uint8_t data);

#endif
