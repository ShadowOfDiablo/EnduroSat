#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "device2.h"

// static void internal_slave_task(void *pvParameters);

// void device2_init(void) {
//     printf("Creating Slave task...\n");
//     // Create slave task at medium priority
//     BaseType_t status = xTaskCreate(
//         internal_slave_task,
//         "Slave",
//         2048,  // Reduced stack size
//         NULL,
//         configMAX_PRIORITIES - 2,
//         NULL
//     );
    
//     if (status != pdPASS) {
//         printf("ERROR: Failed to create Slave task!\n");
//     }
// }

// static void internal_slave_task(void *pvParameters) {
//     (void)pvParameters;
//     while(1) {
//         printf("Slave device operating\n");
//         fflush(stdout);
//         vTaskDelay(pdMS_TO_TICKS(1500));
//     }
// }