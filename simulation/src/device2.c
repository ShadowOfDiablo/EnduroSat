#include "FreeRTOS.h"
#include "task.h"
#include "device2.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include "semphr.h"
#include <windows.h>

TaskHandle_t pSlaveTaskHandle = NULL;
extern boolean bDevice1Turn;
BaseType_t firstRun;

void l_internalSlaveTask(void *pParameters);

void gl_device2Init(void) {
    firstRun = pdTRUE;
    printf("Creating Slave task (Device 2)...\n");
    BaseType_t status = xTaskCreate(l_internalSlaveTask, "Slave", 1024, NULL, configMAX_PRIORITIES - 1, &pSlaveTaskHandle);
    if (status != pdPASS) {
        printf("ERROR: Failed to create Slave task! (%d)\n", status);
    } else {
        printf("Slave task created\n");
    }
}

void l_internalSlaveTask(void *pParameters) {
    (void)pParameters;
    srand((unsigned int)GetTickCount());
    while (TRUE)
    {
        if (firstRun == TRUE && bDevice1Turn == TRUE)
        {
            if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
                sDevice2.currentState = SLAVE_ACTIVE;
                xSemaphoreGive(xQueueMutex);
            }
            if (xSemaphoreTake(xConsoleMutex, portMAX_DELAY) == pdTRUE) {
                printf("[Slave]DeviceB is recovering after reset\n");
                xSemaphoreGive(xConsoleMutex);
            }
            firstRun = pdFALSE;
        }
        SlaveState currentState;
        if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
            currentState = sDevice2.currentState;
            xSemaphoreGive(xQueueMutex);
        }
        SlaveState newState = (rand() % 3);
        switch(newState)
        {
            case 0:
                newState = SLAVE_FAULT;
                break;
            case 1:
                newState = SLAVE_ACTIVE;
                break;
            case 2:
                newState = SLAVE_SLEEP;
                break;
            default:
                newState = SLAVE_FAULT;
                break;
        }
        printf("[Slave]DeviceB State changed to: %s\n", gl_slaveStateToString(newState));
        if(currentState != newState)
        {
            if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
                sDevice2.currentState = newState;
                xSemaphoreGive(xQueueMutex);
            }
            if (xSemaphoreTake(xConsoleMutex, portMAX_DELAY) == pdTRUE) {
                xSemaphoreGive(xConsoleMutex);
            }
        }

        if (bDevice1Turn == TRUE) {
            bDevice1Turn = FALSE;
        }
        firstRun = pdFALSE;
        Sleep(1000);
    }
}
