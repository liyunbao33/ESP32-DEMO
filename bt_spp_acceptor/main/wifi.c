/* LwIP SNTP example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"

#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
// #include "protocol_examples_common.h"
#include "esp_sntp.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "mqtt_client.h"
#include "protocol.h"

static const char *TAG = "example";
// static volatile uint8_t timeflag = 0;
// static volatile uint8_t mqttflag = 0;
// static volatile uint8_t mqttflag2 = 1;
SemaphoreHandle_t sntpSemaphore;
esp_mqtt_client_handle_t mqitt_client;
void sntp_time_get_task(void);

/* mqtt */
static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        // msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
        // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        msg_id = esp_mqtt_client_subscribe(client, "rgb", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://mqtt.eclipseprojects.io",
    };
    // #if CONFIG_BROKER_URL_FROM_STDIN
    //     char line[128];

    //     if (strcmp(mqtt_cfg.uri, "FROM_STDIN") == 0) {
    //         int count = 0;
    //         printf("Please enter url of mqtt broker\n");
    //         while (count < 128) {
    //             int c = fgetc(stdin);
    //             if (c == '\n') {
    //                 line[count] = '\0';
    //                 break;
    //             } else if (c > 0 && c < 127) {
    //                 line[count] = c;
    //                 ++count;
    //             }
    //             vTaskDelay(10 / portTICK_PERIOD_MS);
    //         }
    //         mqtt_cfg.uri = line;
    //         printf("Broker url: %s\n", line);
    //     } else {
    //         ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
    //         abort();
    //     }
    // #endif /* CONFIG_BROKER_URL_FROM_STDIN */

    mqitt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqitt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, mqitt_client);
    esp_mqtt_client_start(mqitt_client);
}

void mqtt_app_stop(void)
{
    if (mqitt_client)
    {
        esp_mqtt_client_stop(mqitt_client);
        // esp_mqtt_client_unregister_event(mqitt_client, ESP_EVENT_ANY_ID, mqtt_event_handler);
        esp_mqtt_client_destroy(mqitt_client);
    }
    mqitt_client = NULL;
}

/* smart config */
/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;

static void smartconfig_example_task(void *parm);

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        xTaskCreate(smartconfig_example_task, "smartconfig_example_task", 4096, NULL, 3, NULL);
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE)
    {
        ESP_LOGI(TAG, "Scan done");
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL)
    {
        ESP_LOGI(TAG, "Found channel");
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD)
    {
        ESP_LOGI(TAG, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;
        uint8_t ssid[33] = {0};
        uint8_t password[65] = {0};
        uint8_t rvd_data[33] = {0};

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true)
        {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", password);
        if (evt->type == SC_TYPE_ESPTOUCH_V2)
        {
            ESP_ERROR_CHECK(esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)));
            ESP_LOGI(TAG, "RVD_DATA:");
            for (int i = 0; i < 33; i++)
            {
                printf("%02x ", rvd_data[i]);
            }
            printf("\n");
        }

        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        esp_wifi_connect();
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE)
    {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

void initialise_wifi(void)
{
    // ESP_ERROR_CHECK(esp_netif_init());
    s_wifi_event_group = xEventGroupCreate();
    // ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void smartconfig_example_task(void *parm)
{
    EventBits_t uxBits;
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));
    while (1)
    {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);
        if (uxBits & CONNECTED_BIT)
        {
            ESP_LOGI(TAG, "WiFi Connected to ap");
            // get_time();
            // timeflag = 1;
            // mqttflag = 1;
        }
        if (uxBits & ESPTOUCH_DONE_BIT)
        {
            ESP_LOGI(TAG, "smartconfig over");
            esp_smartconfig_stop();
            xSemaphoreGive(sntpSemaphore);
            frameA1.dat.wifiStat = 1;
            vTaskDelete(NULL);
        }
    }
}

/* Variable holding number of times ESP32 restarted since first boot.
 * It is placed into RTC memory using RTC_DATA_ATTR and
 * maintains its value when ESP32 wakes from deep sleep.
 */
RTC_DATA_ATTR static int boot_count = 0;

static void obtain_time(void);
static void initialize_sntp(void);

// #ifdef CONFIG_SNTP_TIME_SYNC_METHOD_CUSTOM
// void sntp_sync_time(struct timeval *tv)
// {
//    settimeofday(tv, NULL);
//    ESP_LOGI(TAG, "Time is synchronized from custom code");
//    sntp_set_sync_status(SNTP_SYNC_STATUS_COMPLETED);
// }
// #endif

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

void sntp_time_int(void)
{
    sntpSemaphore = xSemaphoreCreateBinary();
}

void sntp_time_get_task(void)
{
    while (1)
    {
        xSemaphoreTake(sntpSemaphore, portMAX_DELAY);

        ++boot_count;
        ESP_LOGI(TAG, "Boot count: %d", boot_count);

        time_t now;
        struct tm timeinfo;
        time(&now);
        localtime_r(&now, &timeinfo);
        // Is time set? If not, tm_year will be (1970 - 1900).
        if (timeinfo.tm_year < (2016 - 1900))
        {
            ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
            obtain_time();
            // update 'now' variable with current time
            time(&now);
        }
        // #ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
        //     else {
        //         // add 500 ms error to the current system time.
        //         // Only to demonstrate a work of adjusting method!
        //         {
        //             ESP_LOGI(TAG, "Add a error for test adjtime");
        //             struct timeval tv_now;
        //             gettimeofday(&tv_now, NULL);
        //             int64_t cpu_time = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
        //             int64_t error_time = cpu_time + 500 * 1000L;
        //             struct timeval tv_error = { .tv_sec = error_time / 1000000L, .tv_usec = error_time % 1000000L };
        //             settimeofday(&tv_error, NULL);
        //         }

        //         ESP_LOGI(TAG, "Time was set, now just adjusting it. Use SMOOTH SYNC method.");
        //         obtain_time();
        //         // update 'now' variable with current time
        //         time(&now);
        //     }
        // #endif

        char strftime_buf[64];

        // Set timezone to Eastern Standard Time and print local time
        // setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
        // tzset();
        // localtime_r(&now, &timeinfo);
        // strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        // ESP_LOGI(TAG, "The current date/time in New York is: %s", strftime_buf);

        // Set timezone to China Standard Time
        setenv("TZ", "CST-8", 1);
        tzset();
        localtime_r(&now, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        ESP_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf);
        frameA1.dat.sec = timeinfo.tm_sec;
        frameA1.dat.min = timeinfo.tm_min;
        frameA1.dat.hour = timeinfo.tm_hour;
        frameA1.dat.day = timeinfo.tm_mday;
        frameA1.dat.mon = timeinfo.tm_mon;
        frameA1.dat.yearL = ((timeinfo.tm_year+1900)&0xFF);
        frameA1.dat.yearH = ((timeinfo.tm_year+1900)>>8);
        frameA1.dat.updateTimeFlag = 1;

        // mqttflag = 1;

        // vTaskDelay(5000 / portTICK_PERIOD_MS);

        // if (sntp_get_sync_mode() == SNTP_SYNC_MODE_SMOOTH) {
        //     struct timeval outdelta;
        //     while (sntp_get_sync_status() == SNTP_SYNC_STATUS_IN_PROGRESS) {
        //         adjtime(NULL, &outdelta);
        //         ESP_LOGI(TAG, "Waiting for adjusting time ... outdelta = %li sec: %li ms: %li us",
        //                     (long)outdelta.tv_sec,
        //                     outdelta.tv_usec/1000,
        //                     outdelta.tv_usec%1000);
        //         vTaskDelay(2000 / portTICK_PERIOD_MS);
        //     }
        // }

        // const int deep_sleep_sec = 10;
        // ESP_LOGI(TAG, "Entering deep sleep for %d seconds", deep_sleep_sec);
        // esp_deep_sleep(1000000LL * deep_sleep_sec);
    }
}

// void app_main(void)
// {
//     ESP_ERROR_CHECK(nvs_flash_init());
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());

//     /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
//      * Read "Establishing Wi-Fi or Ethernet Connection" section in
//      * examples/protocols/README.md for more information about this function.
//      */
//     // ESP_ERROR_CHECK(example_connect());

//     // initialize_sntp();

//     initialise_wifi();

//     while (1)
//     {
//         if (mqttflag && mqttflag2)
//         {
//             mqttflag2 = 0;
//             mqttflag = 0;
//             mqtt_app_start();
//         }

//         if (timeflag)
//         {
//             // timeflag = 0;
//             get_time();
//         }

//         vTaskDelay(5000 / portTICK_PERIOD_MS);
//     }
// }

static void obtain_time(void)
{
    // ESP_ERROR_CHECK(nvs_flash_init());
    // ESP_ERROR_CHECK(esp_netif_init());
    // ESP_ERROR_CHECK(esp_event_loop_create_default());

    // /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
    //  * Read "Establishing Wi-Fi or Ethernet Connection" section in
    //  * examples/protocols/README.md for more information about this function.
    //  */
    // ESP_ERROR_CHECK(example_connect());

    initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
    {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    time(&now);
    localtime_r(&now, &timeinfo);

    // ESP_ERROR_CHECK(example_disconnect());
}

static void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();
}
