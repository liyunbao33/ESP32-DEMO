/**
 * @file my_event.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-02-24
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "esp_event.h"
// #include "esp_event_internal.h"
#include "esp_task.h"

void my_event_init(void)
{
    // Create the default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());


}

// void my_event_loop_create(void)
// {

// }

// void my_event_loop(void)
// {

// }

// void my_event_post_to(void)
// {

// }

// void my_event_handler_register_with(void)
// {

// }

// void my_event_handler_unregister_with(void)
// {

// }

// void my_event_loop_delete(void)
// {

// }
