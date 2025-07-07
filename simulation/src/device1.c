#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "device1.h"
#include "fake_assert.h"

void device1_init() {
    printf("Device1 starting...\n");
    
    xTaskCreate(vMainTask, "Main", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    
    vTaskStartScheduler();
}

void vMainTask(void *pvParameters) {
    (void)pvParameters;
    while(1) {
        printf("Device1 running\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main() {
    device1_init();
    return 0;
}