#ifndef __KEY_H__
#define __KEY_H__
#include "stdbool.h"

typedef enum
{
  BSP_KEY_PRESS = 0x00U,
  ONE_KEY_IN_PRESS,
  KEY_UP_PRESS,
  KEY_DOWN_PRESS,
  KEY_CONFIRM_PRESS,

  BSP_KEY_RELEASE,
  ONE_KEY_IN_RELEASE,
  KEY_UP_RELEASE,
  KEY_DOWN_RELEASE,
  KEY_CONFIRM_RELEASE,

  BSP_KEY_LONG,
  ONE_KEY_IN_LONG,
  KEY_UP_LONG,
  KEY_DOWN_LONG,
  KEY_CONFIRM_LONG,

  KEY_NONE,
} KEY_ValueTypeDef;

typedef enum
{
    DOWN = 0x00U,
    LONG_DOWN,
    GROUP_DOWN,
    UP,
} KEY_StatTypeDef;

// extern ring_buffer_t key_ring_buffer;

void KEY_Init(void);
void KEY_Scan(void);
bool KEY_Query(KEY_ValueTypeDef KEY_VALUE);

#endif
