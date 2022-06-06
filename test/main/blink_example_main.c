/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define LED_GPIO 27
#define KYE_GPIO 0


static uint8_t s_led_state = 0;

static void blink_led(void)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(LED_GPIO, s_led_state);
}
static void key_scan(void)
{
    uint8_t key_flag = 0;
    while (1)
    {
        vTaskDelay(10);
        if(!gpio_get_level(KYE_GPIO))
        {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            {
                if(!gpio_get_level(KYE_GPIO))
                {
                    if(key_flag == 0)
                    {
                        key_flag = 1;
                        blink_led();
                        /* Toggle the LED state */
                        s_led_state = !s_led_state;
                        ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
                    }
                }
            }
        }
        else
        {
        key_flag = 0;
        }
    }
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to GPIO LED!");
    gpio_reset_pin(LED_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
}
static void configure_key(void)
{
    ESP_LOGI(TAG, "Example configured to GPIO KEY!");
    gpio_reset_pin(KYE_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(KYE_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(KYE_GPIO, GPIO_PULLUP_ONLY);
}
  
void app_main(void)
{

    /* Configure the peripheral according to the LED type */
    configure_led();
    configure_key();
    while (1) {
        key_scan();
    }
}
