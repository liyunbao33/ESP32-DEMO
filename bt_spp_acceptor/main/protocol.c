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


Frame_Union_TypeDef frame;
uint8_t dataBackup[PROTOCOL_NUM_MAX];
uint8_t txdBuf[5 + PROTOCOL_NUM_MAX];

void protocol_init(void)
{
    memset(dataBackup, 0, PROTOCOL_NUM_MAX);
    memset(frame.Data, 0, PROTOCOL_NUM_MAX);
}

void Protocol_Process(uint8_t data)
{
    frame.dat.remoteControl = data;

    // printf("cmp is %d\n", memcmp(dataBackup, frame.Data, PROTOCOL_NUM_MAX));

    if (memcmp(dataBackup, frame.Data, PROTOCOL_NUM_MAX) != 0)
    {
        txdBuf[0] = 0x3C;
        txdBuf[1] = 0xA1;
        txdBuf[2] = PROTOCOL_NUM_MAX;
        memcpy(&txdBuf[3], frame.Data, PROTOCOL_NUM_MAX);
        txdBuf[3 + PROTOCOL_NUM_MAX] = 0x00;
        for (uint8_t i = 0; i < (3 + PROTOCOL_NUM_MAX); i++) // 计算校验和
        {
            txdBuf[3 + PROTOCOL_NUM_MAX] += txdBuf[i];
        }
        txdBuf[4 + PROTOCOL_NUM_MAX] = 0x0D;

        // NRF24L01_TxPacket(txdBuf, (5 + PROTOCOL_NUM_MAX));
        sendData("TX_SEND", (char *)txdBuf, (5 + PROTOCOL_NUM_MAX));
        memset(frame.Data, 0, PROTOCOL_NUM_MAX);
    }
}
