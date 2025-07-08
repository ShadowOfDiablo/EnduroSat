#include "portmacro.h"
#include "FreeRTOS.h"
#include "task.h"
#include <windows.h>
#include <stdio.h>

static UBaseType_t uxCriticalNesting = 0;
static HANDLE hTimerThread = NULL;

#if ( configAPPLICATION_ALLOCATED_HEAP == 1 )
    uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#else
    static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#endif

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

// Create a simulated tick thread
static DWORD WINAPI prvTimerThread(LPVOID lpParam) {
    const DWORD xDelay = 1000 / configTICK_RATE_HZ;
    
    for (;;) {
        Sleep(xDelay);
        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
            xTaskIncrementTick();
        }
    }
    return 0;
}

BaseType_t xPortStartScheduler(void) {
    // Create the tick thread
    hTimerThread = CreateThread(NULL, 0, prvTimerThread, NULL, 0, NULL);
    if (hTimerThread == NULL) {
        return pdFALSE;
    }

    SetThreadPriority(hTimerThread, THREAD_PRIORITY_HIGHEST);

    vTaskStartScheduler();

    return pdFALSE;
}

void vPortEndScheduler(void) {
    if (hTimerThread) {
        TerminateThread(hTimerThread, 0);
        CloseHandle(hTimerThread);
    }
    exit(0);
}