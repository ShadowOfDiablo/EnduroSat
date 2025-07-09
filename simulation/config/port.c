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
    if (xCriticalSection.DebugInfo == NULL) 
    {
        // Critical section not initialized yet
        return;
    }
    EnterCriticalSection(&xCriticalSection);
    uxCriticalNesting++;
}

void vPortExitCritical(void) {
    if (xCriticalSection.DebugInfo == NULL)
    {
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
        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
        {
            if (xTaskIncrementTick() != pdFALSE) {
                portYIELD();
            }
        }
    }
    return 0;
}

BaseType_t xPortStartScheduler( void )
{
    /* 1) Spin up the FreeRTOS tick thread */
    hTimerThread = CreateThread(
        NULL,           /* no security */
        0,              /* default stack */
        prvTimerThread, /* thread fn */
        NULL,           /* no parameter */
        0,              /* run immediately */
        NULL            /* no thread ID needed */
    );
    configASSERT( hTimerThread );

    SetThreadPriority( hTimerThread, THREAD_PRIORITY_TIME_CRITICAL );

    /* 2) Now *block* forever so the scheduler can run tasks */
    WaitForSingleObject( hTimerThread, INFINITE );

    /* 3) If someone calls vPortEndScheduler(), we’ll break out */
    return pdFALSE;
}
void vPortEndScheduler( void )
{
    if( hTimerThread != NULL )
    {
        TerminateThread( hTimerThread, 0 );
        CloseHandle( hTimerThread );
    }
}