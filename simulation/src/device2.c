#include "FreeRTOS.h"
#include "task.h"
#include "device2.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include "semphr.h"

extern SemaphoreHandle_t xQueueMutex;
extern SemaphoreHandle_t xConsoleMutex;
extern struct device device2;
volatile BaseType_t slaveResetRequested = pdFALSE;
void internal_slave_task(void *pvParameters);

void device2_init(void)
{
    printf("Creating Slave task (Device 2)...\n");

    BaseType_t status = xTaskCreate(
        internal_slave_task,
        "Slave",
        1024, NULL,
        configMAX_PRIORITIES - 1,
        NULL);

    if (status != pdPASS) {
        printf("ERROR: Failed to create Slave task! (%d)\n", status);
    } else {
        printf("Slave task created\n");
    }
}

void internal_slave_task(void *pvParameters)
{
    (void) pvParameters;

    srand((unsigned int)xTaskGetTickCount());
    const TickType_t xDelay = pdMS_TO_TICKS(1000);

    if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
        device2.currentState = SLAVE_ACTIVE;
        xSemaphoreGive(xQueueMutex);
    }
    printf("[Slave] Boot: State is ACTIVE\n");

    for (;;)
    {
        vTaskDelay(1000);
        if (slaveResetRequested) {
            if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
                device2.currentState = SLAVE_ACTIVE;
                xSemaphoreGive(xQueueMutex);
            }
            if (xSemaphoreTake(xConsoleMutex, portMAX_DELAY) == pdTRUE) {
                printf("[Slave] Reset received. Switching to ACTIVE.\n");
                xSemaphoreGive(xConsoleMutex);
            }
            slaveResetRequested = pdFALSE;
        }

        SlaveState newState;

        if (device2.currentState == SLAVE_ACTIVE) {
            newState = (rand() % 10 == 0) ? SLAVE_FAULT : SLAVE_ACTIVE;
        }
        else if (device2.currentState == SLAVE_FAULT) {
            newState = SLAVE_FAULT;  
        }
        else {
            newState = SLAVE_ACTIVE;
        }

        if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
            device2.currentState = newState;
            xSemaphoreGive(xQueueMutex);
        }
        
        printf("[Slave] Tick: %lu | State: %d\n",
               xTaskGetTickCount(), newState);
    }
}