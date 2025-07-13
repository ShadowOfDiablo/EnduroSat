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
struct device sDevice2;
volatile BaseType_t slaveResetRequested = pdFALSE;

const char* gl_slaveStateToString(SlaveState state) {
    switch (state) {
        case SLAVE_SLEEP: return "SLEEP";
        case SLAVE_ACTIVE: return "ACTIVE";
        case SLAVE_FAULT: return "FAULT";
        default: return "UNKNOWN";
    }
}

const char* gl_masterStateToString(MasterState state) {
    switch (state) {
        case MASTER_IDLE: return "IDLE";
        case MASTER_PROCESSING: return "PROCESSING";
        case MASTER_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void vAssertCalled(const char * const cpFileName, unsigned long ulLine) {
    printf("ASSERT! Line %lu of file %s\n", ulLine, cpFileName);
    taskDISABLE_INTERRUPTS();
    for (;;);
}

int main(void) {
    printf("Starting FreeRTOS Windows port...\n");

    xQueueMutex = xSemaphoreCreateMutex();
    xConsoleMutex = xSemaphoreCreateMutex();

    sDevice2.id = 0xB;
    sDevice2.currentState = SLAVE_SLEEP;

    gl_device1Init();
    gl_device2Init();

    printf("Free heap before scheduler: %lu bytes\n", xPortGetFreeHeapSize());

    vTaskStartScheduler();

    return 0;
}
