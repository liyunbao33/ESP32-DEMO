#ifndef __WIFI_H__
#define __WIFI_H__

void initialise_wifi(void);
// void get_time(void);
void sntp_time_int(void);
void sntp_time_get_task(void);
extern SemaphoreHandle_t sntpSemaphore;

#endif
