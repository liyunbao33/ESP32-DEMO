#ifndef __WIFI_H__
#define __WIFI_H__

void initialise_wifi(void);
// void get_time(void);
void sntp_time_int(void);
void sntp_time_get_task(void);
void mqtt_app_start(void);
void mqtt_app_stop(void);
extern SemaphoreHandle_t sntpSemaphore;

#endif
