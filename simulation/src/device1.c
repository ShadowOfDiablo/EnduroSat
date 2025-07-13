#include "FreeRTOS.h"
#include "task.h"
#include "device1.h"
#include "device2.h"
#include "main.h"
#include <stdio.h>
#include "semphr.h"
#include <windows.h>

boolean device1Turn = FALSE;
extern TaskHandle_t xSlaveTaskHandle;
void internal_master_task(void *pvParameters);
void gl_resetDevice2(TaskHandle_t xTaskHandle);

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
    while (TRUE) {
        Sleep(500);

        SlaveState currentSlaveState;
        if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
            currentSlaveState = device2.currentState;
            xSemaphoreGive(xQueueMutex);
        }

        switch (currentSlaveState) 
        {
            case SLAVE_ACTIVE:
                myState = MASTER_PROCESSING;
                faultCounter = 0;
                break;
            case SLAVE_FAULT:
                myState = MASTER_ERROR;
                break;
            case SLAVE_SLEEP:
                myState = MASTER_IDLE;
                break;
            default:
                myState = MASTER_IDLE;
                faultCounter = 0;
                break;
        }

        if (currentSlaveState == SLAVE_FAULT && lastKnownSlaveState != SLAVE_FAULT) {
            faultCounter++;
        } else if (currentSlaveState == SLAVE_ACTIVE) {
            faultCounter = 0;
        } else if (currentSlaveState == SLAVE_SLEEP)
        {
            faultCounter = 0;
        } else {
            if(currentSlaveState == SLAVE_FAULT && device1Turn != TRUE)
            {
                faultCounter++;
            }
        }
        
        lastKnownSlaveState = currentSlaveState;
        
        if (xSemaphoreTake(xConsoleMutex, portMAX_DELAY) == pdTRUE) {
            printf("[Master]DeviceA My State: %-12s | Slave(DeviceB) is: %-8s | Consecutive Faults: %d\n", 
                   masterStateToString(myState), 
                   slaveStateToString(currentSlaveState), 
                   faultCounter);
            xSemaphoreGive(xConsoleMutex);
        }

        if (faultCounter >= FAULT_RESET_THRESHOLD) 
        {
            if (xSemaphoreTake(xConsoleMutex, portMAX_DELAY) == pdTRUE) {
                printf("[Master]DeviceA DETECTED %d FAULTS! Sending Slave(DeviceB) to SLEEP...\n", FAULT_RESET_THRESHOLD);
                xSemaphoreGive(xConsoleMutex);
            }

            gl_resetDevice2(xSlaveTaskHandle);
            vTaskDelay(pdMS_TO_TICKS(50));
            if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
                device2.currentState = SLAVE_SLEEP;
                xSemaphoreGive(xQueueMutex);
            }

            faultCounter = 0;
            device1Turn = TRUE;
        }
        device1Turn = TRUE;
        Sleep(100);
    }
}

void gl_resetDevice2(TaskHandle_t xTaskHandle)
{
    if (xTaskHandle != NULL)
    {
        vTaskDelete(xTaskHandle);
    }
    extern BaseType_t firstRun;
    firstRun = pdTRUE;
    printf("[Master] Resetting Slave(DeviceB)...\n");
    Sleep(500);
    device2_init();
}