#include "FreeRTOS.h"
#include "task.h"
#include "device2.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include "semphr.h"
#include <windows.h>

TaskHandle_t xSlaveTaskHandle = NULL;
extern boolean device1Turn;
BaseType_t firstRun;

void internal_slave_task(void *pvParameters);

void device2_init(void) {
    firstRun = pdTRUE;
    printf("Creating Slave task (Device 2)...\n");
    BaseType_t status = xTaskCreate(internal_slave_task, "Slave", 1024, NULL, configMAX_PRIORITIES - 1, &xSlaveTaskHandle);
    if (status != pdPASS) {
        printf("ERROR: Failed to create Slave task! (%d)\n", status);
    } else {
        printf("Slave task created\n");
    }
}

void internal_slave_task(void *pvParameters) {
    (void) pvParameters;
    srand((unsigned int)GetTickCount());
    while (TRUE)
    {
        if (firstRun == TRUE && device1Turn == TRUE)
        {
            if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
                device2.currentState = SLAVE_ACTIVE;
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
            currentState = device2.currentState;
            xSemaphoreGive(xQueueMutex);
        }
        SlaveState newState = (rand()%3); //= (rand() % 5 == 0) ? SLAVE_FAULT : SLAVE_ACTIVE;
        switch(newState)
        {
        case 0:
            newState = SLAVE_FAULT;
        break;

        case 1:
            newState = SLAVE_FAULT;
        break;

        case 2:
            newState = SLAVE_FAULT;
        break;

        default:
            newState = SLAVE_FAULT;
        break;
        }
        printf("[Slave]DeviceB State changed to: %s\n", slaveStateToString(newState));
        if(currentState != newState)
        {
            if (xSemaphoreTake(xQueueMutex, portMAX_DELAY) == pdTRUE) {
                device2.currentState = newState;
                xSemaphoreGive(xQueueMutex);
            }
            if (xSemaphoreTake(xConsoleMutex, portMAX_DELAY) == pdTRUE) {
                xSemaphoreGive(xConsoleMutex);
            }
        }

        if (device1Turn == TRUE) {
            device1Turn = FALSE;
        }
        firstRun = pdFALSE;
        Sleep(1000);
    }
}