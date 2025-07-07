#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"

static UBaseType_t uxCriticalNesting = 0;

void vPortYield(void) {
    SwitchToThread();
}

void vPortEnterCritical(void) {
    uxCriticalNesting++;
}

void vPortExitCritical(void) {
    if (uxCriticalNesting > 0) {
        uxCriticalNesting--;
    }
}

void vPortGenerateSimulatedInterrupt(uint32_t ulInterruptNumber) {
}

void vPortCloseRunningThread(void *pvTaskToDelete, volatile BaseType_t *pxPendYield) {
}

void vPortDeleteThread(void *pvTaskToDelete) {
}

BaseType_t xPortStartScheduler(void) {
    return pdTRUE;
}