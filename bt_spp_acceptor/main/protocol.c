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


Frame_Union_TypeDef frame;
uint8_t dataBackup[PROTOCOL_NUM_MAX];
uint8_t nrfTxdBuf[5 + PROTOCOL_NUM_MAX];

void protocol_init(void)
{
    memset(dataBackup, 0, PROTOCOL_NUM_MAX);
    memset(frame.Data, 0, PROTOCOL_NUM_MAX);
}

void Protocol_Process(void)
{
    // if (KEY_Query(KEY_GROUP_UP))
    // {
    //     frame.dat.joyStick = FORWARD;
    // }
    // else if (KEY_Query(KEY_UP_RIGHT_DOWN))
    // {
    //     frame.dat.joyStick = RIGHT_FORWARD;
    // }
    // else if (KEY_Query(KEY_DOWN_RIGHT_DOWN))
    // {
    //     frame.dat.joyStick = RIGHT_BACKWARD;
    // }
    // else if (KEY_Query(KEY_UP_LEFT_DOWN))
    // {
    //     frame.dat.joyStick = LEFT_FORWARD;
    // }
    // else if (KEY_Query(KEY_DOWN_LEFT_DOWN))
    // {
    //     frame.dat.joyStick = LEFT_BACKWARD;
    // }
    // else if (KEY_Query(KEY_GROUP_DOWN))
    // {
    //     frame.dat.joyStick = BACKWARD;
    // }
    // else if (KEY_Query(KEY_GROUP_LEFT))
    // {
    //     frame.dat.joyStick = LEFT_ROTATION;
    // }
    // else if (KEY_Query(KEY_GROUP_RIGHT))
    // {
    //     frame.dat.joyStick = RIGHT_ROTATION;
    // }

    // if (KEY_Query(KEY_SPEED_UP_DOWN))
    // {
    //     frame.dat.speed = SPEED_UP;
    // }
    // else if (KEY_Query(KEY_SPEED_DOWN_DOWN))
    // {
    //     frame.dat.speed = SPEED_DOWN;
    // }

    // if (KEY_Query(KEY_BRAKE))
    // {
    //     frame.dat.brake = BRAKE;
    // }

    // printf("cmp is %bd\n", memcmp(dataBackup, frame.Data, PROTOCOL_NUM_MAX));

    if (memcmp(dataBackup, frame.Data, PROTOCOL_NUM_MAX) != 0)
    {
        nrfTxdBuf[0] = 0x3C;
        nrfTxdBuf[1] = 0xA1;
        nrfTxdBuf[2] = PROTOCOL_NUM_MAX;
        memcpy(&nrfTxdBuf[3], frame.Data, PROTOCOL_NUM_MAX);
        nrfTxdBuf[3 + PROTOCOL_NUM_MAX] = 0x00;
        for (uint8_t i = 0; i < (3 + PROTOCOL_NUM_MAX); i++) // 计算校验和
        {
            nrfTxdBuf[3 + PROTOCOL_NUM_MAX] += nrfTxdBuf[i];
        }
        nrfTxdBuf[4 + PROTOCOL_NUM_MAX] = 0x0D;

        // __ValueMonitor(nrfTxdBuf[3 + PROTOCOL_NUM_MAX], Blink(2, 5, 5));

        // NRF24L01_Transmit(nrfTxdBuf, 5, )5;/
        // NRF24L01_TxPacket(nrfTxdBuf, (5 + PROTOCOL_NUM_MAX));
        memset(frame.Data, 0, PROTOCOL_NUM_MAX);
    }
}
