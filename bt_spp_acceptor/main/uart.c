/**
 * @file uart.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-02-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "spp_task.h"
#include "uart.h"
#include "protocol.h"

static const int RX_BUF_SIZE = 1024;
// static const int TX_BUF_SIZE = 1024;

static QueueHandle_t uart2_queue;

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)

void uart_init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_2, RX_BUF_SIZE * 2, 0, 20, &uart2_queue, 0);
    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

int sendData(const char *logName, const char *data, uint8_t len)
{
    // const int len = __Sizeof(data);
    const int txBytes = uart_write_bytes(UART_NUM_2, data, len);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}

void uart_tx_task(void *arg)
{
    char rec_data;

    // esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    while (1)
    {
        xQueueReceive(spp_receive_queue, &rec_data, (portTickType)portMAX_DELAY);
        Protocol_Process(rec_data);
        // sendData(TX_TASK_TAG, &rec_data);
    }
}

void uart_rx_task(void *arg)
{
    uart_event_t event;

    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE + 1);
    while (1)
    {

        int rxBytes;
        if (xQueueReceive(uart2_queue, (void *)&event, (TickType_t)portMAX_DELAY))
        {
            switch (event.type)
            {
            // Event of UART receving data
            /*We'd better handler data event fast, there would be much more data events than
            other types of events. If we take too much time on data event, the queue might
            be full.*/
            case UART_DATA:
                rxBytes = uart_read_bytes(UART_NUM_2, data, event.size, portMAX_DELAY);
                if (rxBytes > 0)
                {
                    data[rxBytes] = 0;
                    ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
                    ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
                }
                break;
            default:
                break;
            }
        }

        // const int rxBytes = uart_read_bytes(UART_NUM_2, data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        // if (rxBytes > 0)
        // {
        //     data[rxBytes] = 0;
        //     ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
        //     ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
        // }
    }
    free(data);
}
