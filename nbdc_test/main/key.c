#include "key.h"
#include <stdio.h>	

#define KEY_TIME_BASE 10 /* unit of ms */
#define TWO_SEC (2000/10) /* unit of ms */

static uint8_t IsBSPKeyPress(void)
{
	// return (!(PAin(8)))?true:false; 
	return 0;
}

static uint8_t IsKeyUpPress(void)
{
	// return (!(PCin(1)))?true:false;
	return 0;
}

static uint8_t IsKeyDownPress(void)
{
	// return (!(PCin(3)))?true:false;
	return 0;
}

static uint8_t IsKeyConfirmPress(void)
{
	// return (!(PCin(2)))?true:false;
	return 0;
}

typedef struct
{
	uint8_t (*IsKeyPressFunc)(void);/* 按键判断函数 */
	uint8_t filrVal; /* 滤波值 */
	uint16_t count;  /* 按键计数器 */
	uint16_t shortTimeVal; /* 按键短按值 */
	uint16_t logTimeVal; /* 按键长按值 */
	KEY_StatTypeDef stat; /* 按键状态值 */
	uint16_t repeatPeriod;/* 连续按键周期 */
	uint16_t repeatCount;/* 连续按键计数器 */
} KEY_TypeDef;

KEY_TypeDef key[] = 
{
	{IsBSPKeyPress, 3, 0, 100, 80, UP, 0, 0},
	{IsKeyUpPress, 3, 0, 50, 0, UP, 20, 0},
	{IsKeyDownPress, 3, 0, 50, 0, UP, 20, 0},
	{IsKeyConfirmPress, 3, 0, 50, 100, UP, 0, 0},

};
KEY_ValueTypeDef key_value = KEY_NONE;
uint16_t KEY_NUM = sizeof(key)/sizeof(key[0]);


void KEY_Init(void) {
	
	// __HAL_RCC_GPIOA_CLK_ENABLE();
	// __HAL_RCC_GPIOC_CLK_ENABLE();
    
    // GPIO_InitTypeDef KEY_GPIO_Struct = {0};
    
	// /* BSP_KEY_GPIO_Init */
    // KEY_GPIO_Struct.Mode = GPIO_MODE_INPUT;
    // KEY_GPIO_Struct.Pin = GPIO_PIN_8;    
    // KEY_GPIO_Struct.Pull = GPIO_PULLUP;                  
    // KEY_GPIO_Struct.Speed = GPIO_SPEED_FREQ_LOW;
    // HAL_GPIO_Init(GPIOA,&KEY_GPIO_Struct);

	// KEY_GPIO_Struct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;  
	// HAL_GPIO_Init(GPIOC,&KEY_GPIO_Struct);
	// // ring_buffer_init(&key_ring_buffer);
	// One_Key_Init();
}


static void KEY_Detect(uint8_t i)
{
	if(key[i].IsKeyPressFunc())
	{
		if(++key[i].count <= 0) --key[i].count;
		if(key[i].filrVal <= key[i].count && key[i].count <= key[i].shortTimeVal)
		{
			if(key[i].stat == UP)
			{
				key[i].stat = DOWN;
				key_value = i;
				// ring_buffer_put(&key_ring_buffer, (KEY_ValueTypeDef)i);
			}
		}
		else
		{
			if(key[i].count >= key[i].logTimeVal && key[i].logTimeVal != 0)
			{
				if(key[i].stat != LONG_DOWN)
				{
					// ring_buffer_put(&key_ring_buffer, (KEY_ValueTypeDef)(i + KEY_NUM*2));
					key_value = (i + KEY_NUM*2);
					key[i].stat = LONG_DOWN;
				}
			}
			else if(++key[i].repeatCount >= key[i].repeatPeriod && key[i].repeatPeriod != 0)
			{
				// ring_buffer_put(&key_ring_buffer, (KEY_ValueTypeDef)i);
				if(key[i].stat == DOWN)
				{
					key_value = i;
				}
				key[i].repeatCount = 0;
				key[i].stat = DOWN;
			}
			
		}
	}
	else
	{
		if(key[i].stat == DOWN)
		{
			key_value = (i + KEY_NUM*1);
			// ring_buffer_put(&key_ring_buffer, (KEY_ValueTypeDef)(i + KEY_NUM*1));
		}
		key[i].count = 0;
		key[i].repeatCount = 0;
		key[i].stat = UP;
	}
}


void KEY_Scan(void) 
{
	for(uint8_t i = 0; i < KEY_NUM; i++)
	{
		KEY_Detect(i);
	}

	// /* 组合按键处理 */
	// if (key[KEY1_DOWN].count >= TWO_SEC && key[KEY1_DOWN].stat == DOWN
	// && key[KEY2_DOWN].count >= TWO_SEC && key[KEY2_DOWN].stat == DOWN)
	// {
	// 	key[KEY1_DOWN].stat = GROUP_DOWN;
	// 	key[KEY2_DOWN].stat = GROUP_DOWN;
	// 	ring_buffer_put(&key_ring_buffer, KEY1_KEY2);
	// }
}

/**
 * @brief Queries whether the current key value is the specified key
 * 
 * @param KEY_VALUE  specified key
 * @return true 
 * @return false 
 */
bool KEY_Query(KEY_ValueTypeDef KEY_VALUE)
{
	if(key_value == KEY_VALUE)
	{
		key_value = KEY_NONE;
		return true;
	}
	else
		return false;
}


