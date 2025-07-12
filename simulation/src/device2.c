#include "FreeRTOS.h"
#include "task.h"
#include "device2.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include "semphr.h"
#include <windows.h>

extern SemaphoreHandle_t xQueueMutex;
extern SemaphoreHandle_t xConsoleMutex;
extern struct device device2;
TaskHandle_t xSlaveTaskHandle = NULL;
volatile BaseType_t slaveResetRequested = pdFALSE;

void internal_slave_task(void *pvParameters);

void device2_init(void)
{
    printf("Creating Slave task (Device 2)...\n");
    BaseType_t status = xTaskCreate(internal_slave_task, "Slave", 1024, NULL, configMAX_PRIORITIES - 1, &xSlaveTaskHandle);
    if (status != pdPASS) {
        printf("ERROR: Failed to create Slave task! (%d)\n", status);
    } else {
        printf("Slave task created\n");
    }
}

void internal_slave_task(void *pvParameters)
{
    (void) pvParameters;
    srand((unsigned int)GetTickCount());

    for (;;)
    {
        Sleep(1000); // 1000 ms

        if (slaveResetRequested == pdTRUE) {
            slaveResetRequested = pdFALSE;
            if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
                device2.currentState = SLAVE_ACTIVE;
                xSemaphoreGive(xQueueMutex);
            }
            if (xSemaphoreTake(xConsoleMutex, portMAX_DELAY) == pdTRUE) {
                printf("[Slave]DeviceB: Reset request received. State is now ACTIVE.\n");
                xSemaphoreGive(xConsoleMutex);
            }
            continue;
        }

        SlaveState currentState;
        if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
            currentState = device2.currentState;
            xSemaphoreGive(xQueueMutex);
        }
        
        SlaveState newState = (rand() % 5 == 0) ? SLAVE_FAULT : SLAVE_ACTIVE;

        if (newState != currentState) {
            if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
                device2.currentState = newState;
                xSemaphoreGive(xQueueMutex);
            }
            if (xSemaphoreTake(xConsoleMutex, portMAX_DELAY) == pdTRUE) {
                printf("[Slave]DeviceB State changed to: %d\n", newState);
                xSemaphoreGive(xConsoleMutex);
            }
        }
    }
}