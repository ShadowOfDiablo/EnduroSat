#include "FreeRTOS.h"
#include "task.h"
#include "device1.h"
#include "device2.h"
#include "main.h"
#include <stdio.h>
#include "semphr.h"
#include <windows.h>

void internal_master_task(void *pvParameters);

void device1_init(void) {
    printf("Creating [Master]DeviceA task (Device 1)...\n");
    BaseType_t status = xTaskCreate(internal_master_task, "Master", 1024, NULL, configMAX_PRIORITIES - 2, NULL);
    if (status != pdPASS) {
        printf("ERROR: Failed to create [Master]DeviceA task! (%d)\n", status);
    } else {
        printf("[Master]DeviceA task created\n");
    }
}

void internal_master_task(void *pvParameters) {
    (void)pvParameters;
    MasterState myState = MASTER_IDLE;
    int faultCounter = 0;
    
    SlaveState lastKnownSlaveState = SLAVE_SLEEP;

    for (;;) {
        Sleep(500);

        SlaveState currentSlaveState;
        if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
            currentSlaveState = device2.currentState;
            xSemaphoreGive(xQueueMutex);
        }

        switch (currentSlaveState) {
            case SLAVE_ACTIVE:
                myState = MASTER_PROCESSING;
                break;
            case SLAVE_FAULT:
                myState = MASTER_ERROR;
                break;
            default:
                myState = MASTER_IDLE;
                break;
        }

        if (currentSlaveState == SLAVE_FAULT && lastKnownSlaveState != SLAVE_FAULT) {
            faultCounter++;
        } else if (currentSlaveState == SLAVE_ACTIVE) {
            faultCounter = 0;
        }
        
        lastKnownSlaveState = currentSlaveState;
        
        if (xSemaphoreTake(xConsoleMutex, portMAX_DELAY) == pdTRUE) {
            printf("[Master]DeviceA My State: %-12s | Slave(DeviceB) is: %-8s | Consecutive Faults: %d\n", 
                   masterStateToString(myState), 
                   slaveStateToString(currentSlaveState), 
                   faultCounter);
            xSemaphoreGive(xConsoleMutex);
        }

        if (faultCounter >= FAULT_RESET_THRESHOLD) {
            if (xSemaphoreTake(xConsoleMutex, portMAX_DELAY) == pdTRUE) {
                printf("[Master]DeviceA DETECTED %d FAULTS! Requesting Slave(DeviceB) reset...\n", FAULT_RESET_THRESHOLD);
                xSemaphoreGive(xConsoleMutex);
            }
            slaveResetRequested = pdTRUE;
            faultCounter = 0;
        }
    }
}