#ifndef __UART_H__
#define __UART_H__

void uart_init(void); 
void uart_tx_task(void *arg);
void uart_rx_task(void *arg);

#endif