#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "main.h"
#include "device1.h"
#include "device2.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(ms) usleep((ms) * 1000)
#endif

SemaphoreHandle_t xQueueMutex;
SemaphoreHandle_t xConsoleMutex;
struct device device2;

void vAssertCalled(const char * const pcFileName, unsigned long ulLine)
{
    printf("ASSERT! Line %lu of file %s\n", ulLine, pcFileName);
    taskDISABLE_INTERRUPTS();
    for (;;);
}

int main(void)
{
    printf("Starting FreeRTOS Windows port...\n");

    xQueueMutex = xSemaphoreCreateMutex();
    xConsoleMutex = xSemaphoreCreateMutex();

    device1_init();
    device2_init();

    printf("Free heap before scheduler: %lu bytes\n", xPortGetFreeHeapSize());

    vTaskStartScheduler();
    
    return 0;
}