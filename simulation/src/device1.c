#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "device1.h"

// static void internal_master_task(void *pvParameters);

// void device1_init(void) {
//     printf("Creating Master task...\n");
//     // Create master task at highest priority
//     BaseType_t status = xTaskCreate(
//         internal_master_task,
//         "Master",
//         2048,  // Reduced stack size
//         NULL,
//         configMAX_PRIORITIES - 1,
//         NULL
//     );
    
//     if (status != pdPASS) {
//         printf("ERROR: Failed to create Master task!\n");
//     }
// }

// static void internal_master_task(void *pvParameters) {
//     (void)pvParameters;
//     while(1) {
//         printf("Master device operating\n");
//         fflush(stdout);
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }