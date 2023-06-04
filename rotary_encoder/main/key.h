#ifndef __KEY_H__
#define __KEY_H__
#include "stdbool.h"

typedef enum
{
  KEY_UP_PRESS = 0x00,
  KEY_DOWN_PRESS,

  KEY_UP_RELEASE,
  KEY_DOWN_RELEASE,

  KEY_UP_LONG,
  KEY_DOWN_LONG,

  KEY_NONE,
} KEY_ValueTypeDef;

typedef enum
{
    DOWN = 0x00U,
    LONG_DOWN,
    GROUP_DOWN,
    UP,
} KEY_StatTypeDef;

void KEY_Init(void);
void KEY_Scan(void);
bool KEY_Query(KEY_ValueTypeDef KEY_VALUE);

#endif
