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
volatile BaseType_t slaveResetRequested = pdFALSE;

const char* slaveStateToString(SlaveState state) {
    switch (state) {
        case SLAVE_SLEEP: return "SLEEP";
        case SLAVE_ACTIVE: return "ACTIVE";
        case SLAVE_FAULT: return "FAULT";
        default: return "UNKNOWN";
    }
}

const char* masterStateToString(MasterState state) {
    switch (state) {
        case MASTER_IDLE: return "IDLE";
        case MASTER_PROCESSING: return "PROCESSING";
        case MASTER_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void vAssertCalled(const char * const pcFileName, unsigned long ulLine) {
    printf("ASSERT! Line %lu of file %s\n", ulLine, pcFileName);
    taskDISABLE_INTERRUPTS();
    for (;;);
}

int main(void) {
    printf("Starting FreeRTOS Windows port...\n");

    xQueueMutex = xSemaphoreCreateMutex();
    xConsoleMutex = xSemaphoreCreateMutex();

    device2.id = 0xB;
    device2.currentState = SLAVE_SLEEP;

    device1_init();
    device2_init();

    printf("Free heap before scheduler: %lu bytes\n", xPortGetFreeHeapSize());

    vTaskStartScheduler();
    
    return 0;
}