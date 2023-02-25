#ifndef __UART_H__
#define __UART_H__
#include "main.h"

void uart_init(void); 
void uart_tx_task(void *arg);
void uart_rx_task(void *arg);
int sendData(const char *logName, const char *data, uint8_t len);

#endif