/**
 * @file protocol.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-02-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "protocol.h"
#include <string.h>
#include "uart.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "main.h"
#include <stdio.h>

FrameA1_Union_TypeDef frameA1;
FrameA2_Union_TypeDef frameA2;
SemaphoreHandle_t protocol_semaphore;

// uint8_t frameA1Backup[PROTOCOLA1_NUM_MAX];
// uint8_t txdBuf[5 + PROTOCOLA1_NUM_MAX];

// void protocol_init(void)
// {
//     memset(frameA1Backup, 0, PROTOCOLA1_NUM_MAX);
//     memset(frameA1.Data, 0, PROTOCOLA1_NUM_MAX);
// }

void Protocol_Init(void)
{
    protocol_semaphore = xSemaphoreCreateBinary();
}

void Protocol_Send(uint8_t funCode, uint8_t *data)
{
    uint8_t txdBuf[5 + PROTOCOLA1_NUM_MAX];

    // printf("cmp is %d\n", memcmp(frameA1Backup, frameA1.Data, PROTOCOLA1_NUM_MAX));

    // if (memcmp(frameA1Backup, frameA1.Data, PROTOCOLA1_NUM_MAX) != 0)
    // {
    switch (funCode)
    {
    case 0xA1:
        txdBuf[0] = 0x3C;
        txdBuf[1] = 0xA1;
        txdBuf[2] = PROTOCOLA1_NUM_MAX;
        memcpy(&txdBuf[3], data, PROTOCOLA1_NUM_MAX);
        txdBuf[3 + PROTOCOLA1_NUM_MAX] = 0x00;
        for (uint8_t i = 0; i < (3 + PROTOCOLA1_NUM_MAX); i++) // 计算校验和
        {
            txdBuf[3 + PROTOCOLA1_NUM_MAX] += txdBuf[i];
        }
        txdBuf[4 + PROTOCOLA1_NUM_MAX] = 0x0D;
        sendData("TX_SEND", (char *)txdBuf, (5 + PROTOCOLA1_NUM_MAX));
        break;
    case 0xA2:
        txdBuf[0] = 0x3C;
        txdBuf[1] = 0xA2;
        txdBuf[2] = PROTOCOLA2_NUM_MAX;
        memcpy(&txdBuf[3], data, PROTOCOLA2_NUM_MAX);
        txdBuf[3 + PROTOCOLA2_NUM_MAX] = 0x00;
        for (uint8_t i = 0; i < (3 + PROTOCOLA2_NUM_MAX); i++) // 计算校验和
        {
            txdBuf[3 + PROTOCOLA2_NUM_MAX] += txdBuf[i];
        }
        txdBuf[4 + PROTOCOLA2_NUM_MAX] = 0x0D;
        sendData("TX_SEND", (char *)txdBuf, (5 + PROTOCOLA2_NUM_MAX));
        break;
    default:
        break;
    }
    // memset(frameA1.Data, 0, PROTOCOLA1_NUM_MAX);
    // }
}

void Protocol_Receive(uint8_t *data)
{
    uint8_t temp_sum = 0;

    temp_sum = (data[0] + data[1] + data[2]);

    for (uint8_t i = 0; i < data[2]; i++)
    {
        temp_sum += data[3 + i];
    }
    if (data[0] == 0x3C && temp_sum == data[3 + data[2]] && data[4 + data[2]] == 0x0D)
    {
        switch (data[1])
        {
        case 0xA1:
            for (uint8_t i = 0; i < data[2]; i++)
            {
                frameA1.Data[i] = data[3 + i];
            }
            break;
        case 0xA2:
            for (uint8_t i = 0; i < data[2]; i++)
            {
                frameA2.Data[i] = data[3 + i];
            }
            __ValueMonitor(frameA2.data.internetSelect, xSemaphoreGive(protocol_semaphore));
            break;

        default:
            break;
        }
    }
}

void protocol_task(void *arg)
{
    while (1)
    {
        xSemaphoreTake(protocol_semaphore, portMAX_DELAY);
        switch (frameA2.data.internetSelect)
        {
        case BLUETOOH:
            bt_spp_on();
            printf("bt_spp_on\n");
            break;
        default:
            break;
        }
    }
}
