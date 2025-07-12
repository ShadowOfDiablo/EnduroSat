#include "FreeRTOS.h"
#include "task.h"
#include "device1.h"
#include "device2.h"
#include "main.h"
#include <stdio.h>
#include "semphr.h"
#include <windows.h>

extern SemaphoreHandle_t xQueueMutex;
extern SemaphoreHandle_t xConsoleMutex;
extern struct device device2;
extern volatile BaseType_t slaveResetRequested;

void internal_master_task(void *pvParameters);

void device1_init(void)
{
    printf("Creating [Master]DeviceA task (Device 1)...\n");
    BaseType_t status = xTaskCreate(internal_master_task, "Master", 1024, NULL, configMAX_PRIORITIES - 2, NULL);
    if (status != pdPASS) {
        printf("ERROR: Failed to create [Master]DeviceA task! (%d)\n", status);
    } else {
        printf("[Master]DeviceA task created\n");
    }
}

void internal_master_task(void *pvParameters)
{
    (void)pvParameters;
    MasterState myState = MASTER_IDLE;
    int faultCounter = 0;

    for (;;)
    {
        Sleep(500); // 500 ms

        SlaveState currentSlaveState;
        if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
            currentSlaveState = device2.currentState;
            xSemaphoreGive(xQueueMutex);
        }

        switch (currentSlaveState) {
            case SLAVE_ACTIVE:
                myState = MASTER_PROCESSING;
                faultCounter = 0;
                break;
            case SLAVE_FAULT:
                myState = MASTER_ERROR;
                faultCounter++;
                break;
            default:
                myState = MASTER_IDLE;
                break;
        }
        
        if (xSemaphoreTake(xConsoleMutex, portMAX_DELAY) == pdTRUE) {
            printf("[Master]DeviceA: My State: %d | Slave is: %d\n", myState, currentSlaveState);
            xSemaphoreGive(xConsoleMutex);
        }

        if (faultCounter >= 3) 
        {
            if (xSemaphoreTake(xConsoleMutex, portMAX_DELAY) == pdTRUE) {
                printf("[Master]DeviceA: DETECTED 3 FAULTS! Requesting Slave reset...\n");
                xSemaphoreGive(xConsoleMutex);
            }
            slaveResetRequested = pdTRUE;
            faultCounter = 0;
        }
    }
}