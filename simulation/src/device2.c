#include "portmacro.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "device2.h"

void device2_task(void *pvParameters) {
    while(1) {
        printf("Device2 running\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main() {
    printf("Device2 starting...\n");
    
    xTaskCreate(device2_task, "Main", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    
    vTaskStartScheduler();
    return 0;
}