#include "portmacro.h"
#include "FreeRTOS.h"
#include "task.h"
#include <windows.h>
#include <stdio.h>

static UBaseType_t uxCriticalNesting = 0;
static CRITICAL_SECTION xCriticalSection;
static HANDLE hTimerThread = NULL;

#if ( configAPPLICATION_ALLOCATED_HEAP == 1 )
    __attribute__((aligned(8))) uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#else
    static __attribute__((aligned(8))) uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#endif

void vPortYield(void) {
    SwitchToThread();
}

void vPortEnterCritical(void) {
    if (xCriticalSection.DebugInfo == NULL) {
        // Critical section not initialized yet
        return;
    }
    EnterCriticalSection(&xCriticalSection);
    uxCriticalNesting++;
}

void vPortExitCritical(void) {
    if (xCriticalSection.DebugInfo == NULL) {
        // Critical section not initialized yet
        return;
    }
    uxCriticalNesting--;
    if (uxCriticalNesting == 0) {
        LeaveCriticalSection(&xCriticalSection);
    }
}

void vPortInitializeCriticalSection(void) {
    InitializeCriticalSection(&xCriticalSection);
}

StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters) {
    pxTopOfStack--;
    
    *pxTopOfStack = (StackType_t)pxCode;        // Instruction pointer
    pxTopOfStack--;
    *pxTopOfStack = 0;                          // Flags register
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t)pvParameters;   // Parameter
    pxTopOfStack--;
    
    // Simulate saved registers
    for(int i = 0; i < 15; i++) {
        *pxTopOfStack = 0;
        pxTopOfStack--;
    }
    
    return pxTopOfStack;
}

static DWORD WINAPI prvTimerThread(LPVOID lpParam) {
    const DWORD xDelay = 1000 / configTICK_RATE_HZ;
    TickType_t xLastTick = xTaskGetTickCount();
    
    for (;;) {
        Sleep(xDelay);
        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
            // Proper tick increment method
            if (xTaskIncrementTick() != pdFALSE) {
                portYIELD();
            }
        }
    }
    return 0;
}

BaseType_t xPortStartScheduler(void) {
    hTimerThread = CreateThread(NULL, 0, prvTimerThread, NULL, 0, NULL);
    if (hTimerThread == NULL) {
        return pdFALSE;
    }

    SetThreadPriority(hTimerThread, THREAD_PRIORITY_HIGHEST);

    // Start the scheduler - this should never return
    vTaskStartScheduler();

    // Only reach here if scheduler failed
    return pdFALSE;
}

void vPortEndScheduler(void) {
    if (hTimerThread) {
        TerminateThread(hTimerThread, 0);
        CloseHandle(hTimerThread);
    }
    exit(0);
}