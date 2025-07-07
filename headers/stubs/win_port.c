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

StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters) {
    return pxTopOfStack;
}

BaseType_t xPortStartScheduler(void) {
    return pdTRUE;
}

void vPortEndScheduler(void) {
    exit(0);
}