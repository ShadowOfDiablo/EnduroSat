#include "FreeRTOS.h"
#include "task.h"
#include "device1.h"
#include "device2.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(ms) usleep((ms)*1000)
#endif

#define EXPAND_MACRO(x) #x
#define STRINGIFY(x) EXPAND_MACRO(x)
#pragma message("configTOTAL_HEAP_SIZE = " STRINGIFY(configTOTAL_HEAP_SIZE))

extern size_t xPortGetFreeHeapSize(void);
extern void *pvPortMalloc(size_t);
static void internal_master_task(void *pvParameters) 
{
    (void)pvParameters;
    while(1) 
    {
        printf("Master device operating\n");
        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void internal_slave_task(void *pvParameters) 
{
    (void)pvParameters;
    while(1) {
        printf("Slave device operating\n");
        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(1500));
    }
}

int main() 
{
    extern void vPortInitializeCriticalSection(void);
    vPortInitializeCriticalSection();  // Initialize critical section first
    
    extern uint8_t ucHeap[configTOTAL_HEAP_SIZE];
    BaseType_t status;
    printf("Heap starts at %p, size: %zu bytes\n", ucHeap, sizeof(ucHeap));
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("Starting FreeRTOS Windows port...\n");
    
    status = xTaskCreate(internal_master_task, "Master", 4096, NULL, configMAX_PRIORITIES-1, NULL);
        if (status != pdPASS) 
        {
            printf("ERROR: Failed to create Master task! (%d)\n", status);
        } 
        else 
        {
            printf("Master task created\n");
        }
    
    status = xTaskCreate(internal_slave_task, "Slave", 4096, NULL, configMAX_PRIORITIES-2, NULL);
        if (status != pdPASS)
        {
            printf("ERROR: Failed to create Slave task! (%d)\n", status);
        } 
        else 
        {
            printf("Slave task created\n");
        }
    
    printf("Free heap before scheduler: %lu bytes\n", xPortGetFreeHeapSize());

    #ifdef _WIN32
        Sleep(100);
    #endif
    
    printf("Starting FreeRTOS scheduler...\n");

    vTaskStartScheduler();
    
    printf("ERROR: Scheduler exited!\n");
    return 0;
}