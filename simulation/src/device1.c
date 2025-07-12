#include "FreeRTOS.h"
#include "task.h"
#include "device1.h"
#include "device2.h"
#include "main.h"
#include <stdio.h>
#include "semphr.h"

extern SemaphoreHandle_t xQueueMutex;
extern SemaphoreHandle_t xConsoleMutex;
extern struct device device2;

void internal_master_task(void *pvParameters);

void device1_init(void)
{
    printf("Creating Master task (Device 1)...\n");
    BaseType_t status = xTaskCreate(
        internal_master_task,
        "Master",
        1024, NULL,
        configMAX_PRIORITIES - 2,
        NULL);

    if (status != pdPASS) {
        printf("ERROR: Failed to create Master task! (%d)\n", status);
    } else {
        printf("Master task created\n");
    }
}

void internal_master_task(void *pvParameters)
{
    (void)pvParameters;
    printf("[Master] Running...\n");

    MasterState myState = MASTER_IDLE;
    int faultCounter = 0;

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));

        SlaveState slaveState;

        if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
            slaveState = device2.currentState;
            xSemaphoreGive(xQueueMutex);
        }

        switch (slaveState) {
            case SLAVE_ACTIVE:
                myState = MASTER_PROCESSING;
                break;
            case SLAVE_FAULT:
                myState = MASTER_ERROR;
                faultCounter++;
                if (faultCounter >= 3) {
                    printf("[Master] 3 FAULTS detected, resetting Slave.\n");
                    if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
                        device2.currentState = SLAVE_SLEEP;
                        xSemaphoreGive(xQueueMutex);
                    }
                    faultCounter = 0;
                }
                break;
            case SLAVE_SLEEP:
                if (myState == MASTER_ERROR) {
                    myState = MASTER_IDLE;
                }
                break;
        }

        printf("[Master] Tick: %lu | My State: %d | Slave: %d | Faults: %d\n",
               xTaskGetTickCount(), myState, slaveState, faultCounter);
    }
}
