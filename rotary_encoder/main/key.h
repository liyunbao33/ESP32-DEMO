#ifndef __KEY_H__
#define __KEY_H__
#include "stdbool.h"

typedef enum
{
  BSP_KEY_PRESS = 0x00,
  // KEY_DOWN_PRESS,

  BSP_KEY_RELEASE,
  // KEY_DOWN_RELEASE,

  BSP_KEY_LONG,
  // KEY_DOWN_LONG,

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
