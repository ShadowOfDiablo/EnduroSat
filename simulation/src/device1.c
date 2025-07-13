#include "FreeRTOS.h"
#include "task.h"
#include "device1.h"
#include "device2.h"
#include "main.h"
#include <stdio.h>
#include "semphr.h"
#include <windows.h>

boolean bDevice1Turn = FALSE;
extern TaskHandle_t pSlaveTaskHandle;
void l_internalMasterTask(void *pParameters);
void gl_resetDevice2(TaskHandle_t pTaskHandle);

void gl_device1Init(void) {
    printf("Creating [Master]DeviceA task (Device 1)...\n");
    BaseType_t status = xTaskCreate(l_internalMasterTask, "Master", 1024, NULL, configMAX_PRIORITIES - 2, NULL);
    if (status != pdPASS) {
        printf("ERROR: Failed to create [Master]DeviceA task! (%d)\n", status);
    } else {
        printf("[Master]DeviceA task created\n");
    }
}

void l_internalMasterTask(void *pParameters) {
    (void)pParameters;
    MasterState masterState = MASTER_IDLE;
    int faultCounter = 0;

    SlaveState lastKnownSlaveState = SLAVE_SLEEP;
    while (TRUE) {
        Sleep(500);

        SlaveState currentSlaveState;
        if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
            currentSlaveState = sDevice2.currentState;
            xSemaphoreGive(xQueueMutex);
        }

        switch (currentSlaveState) 
        {
            case SLAVE_ACTIVE:
                masterState = MASTER_PROCESSING;
                faultCounter = 0;
                break;
            case SLAVE_FAULT:
                masterState = MASTER_ERROR;
                break;
            case SLAVE_SLEEP:
                masterState = MASTER_IDLE;
                break;
            default:
                masterState = MASTER_IDLE;
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
            if(currentSlaveState == SLAVE_FAULT && bDevice1Turn != TRUE)
            {
                faultCounter++;
            }
        }
        
        lastKnownSlaveState = currentSlaveState;
        
        if (xSemaphoreTake(xConsoleMutex, portMAX_DELAY) == pdTRUE) {
            printf("[Master]DeviceA My State: %-12s | Slave(DeviceB) is: %-8s | Consecutive Faults: %d\n", 
                   gl_masterStateToString(masterState), 
                   gl_slaveStateToString(currentSlaveState), 
                   faultCounter);
            xSemaphoreGive(xConsoleMutex);
        }

        if (faultCounter >= FAULT_RESET_THRESHOLD) 
        {
            if (xSemaphoreTake(xConsoleMutex, portMAX_DELAY) == pdTRUE) {
                printf("[Master]DeviceA DETECTED %d FAULTS! Sending Slave(DeviceB) to SLEEP...\n", FAULT_RESET_THRESHOLD);
                xSemaphoreGive(xConsoleMutex);
            }

            gl_resetDevice2(pSlaveTaskHandle);
            vTaskDelay(pdMS_TO_TICKS(50));
            if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
                sDevice2.currentState = SLAVE_SLEEP;
                xSemaphoreGive(xQueueMutex);
            }

            faultCounter = 0;
            bDevice1Turn = TRUE;
        }
        bDevice1Turn = TRUE;
        Sleep(100);
    }
}

void gl_resetDevice2(TaskHandle_t pTaskHandle)
{
    if (pTaskHandle != NULL)
    {
        vTaskDelete(pTaskHandle);
    }
    extern BaseType_t firstRun;
    firstRun = pdTRUE;
    printf("[Master] Resetting Slave(DeviceB)...\n");
    Sleep(500);
    gl_device2Init();
}
