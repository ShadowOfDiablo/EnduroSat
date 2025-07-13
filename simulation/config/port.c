#include "FreeRTOS.h"
#include "task.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#else
    #include <unistd.h>
    #define Sleep(ms) usleep((ms) * 1000)
#endif
#include <stdio.h>
#ifdef _WIN32
    #include <process.h>
    // Windows-specific thread creation or process control functions
#else
    #include <pthread.h> // For threading on Linux
    // Linux/POSIX equivalent thread creation or process control functions
#endif
#define MAX_TASKS 8  

typedef struct {
    TaskFunction_t pxCode;
    void *pvParameters;
} FakeTask;

static FakeTask createdTasks[MAX_TASKS];
static int numCreatedTasks = 0;
static HANDLE hTimerThread = NULL;
static CRITICAL_SECTION xCriticalSection;
static BaseType_t xCriticalSectionInitialised = pdFALSE;

extern void internal_master_task(void *);  
extern void internal_slave_task(void *);   

unsigned __stdcall prvTimerThread(void *lpParameter);
void vPortInitializeCriticalSection(void); 

BaseType_t xPortStartScheduler(void) {
    vPortInitializeCriticalSection();

    hTimerThread = (HANDLE)_beginthreadex(NULL, 0, prvTimerThread, NULL, 0, NULL);
    if (hTimerThread == NULL) { printf("FAILED to create timer\n"); return pdFAIL; }

    printf("Timer thread created OK!\n");

    for (int i = 0; i < numCreatedTasks; ++i) {
        HANDLE h = (HANDLE)_beginthreadex(NULL, 0,
          (unsigned (__stdcall *)(void *)) createdTasks[i].pxCode,
          createdTasks[i].pvParameters, 0, NULL);
        if (h == NULL) printf("FAILED to start task %d\n", i);
        else {
          printf("Task thread started.\n");
          CloseHandle(h);
        }
    }

    WaitForSingleObject(hTimerThread, INFINITE);
    return pdPASS;
}
unsigned __stdcall prvTimerThread(void *lpParameter)
{
    printf("!!! prvTimerThread ENTRY !!!\n");
    const TickType_t xTickPeriodms = 1000 / configTICK_RATE_HZ;
    printf(">> Timer thread STARTED. Tick every %d ms\n", xTickPeriodms);

    while (1) {
        Sleep(xTickPeriodms);

        if (xTaskIncrementTick() != pdFALSE) {
            vTaskSwitchContext();
        }
    }
    return 0;
}
void vPortStartFirstTask(void)
{
    printf("Starting fake FreeRTOS scheduler...\n");
    for (int i = 0; i < numCreatedTasks; ++i) {
        HANDLE h = (HANDLE)_beginthreadex(
            NULL, 0,
            (unsigned (__stdcall *)(void *)) createdTasks[i].pxCode,
            createdTasks[i].pvParameters, 0, NULL);

        if (h == NULL) {
            printf("FAILED: Could not start task %d\n", i);
        } else {
            printf("Task thread %d started.\n", i);
            CloseHandle(h);
        }
    }
}
unsigned __stdcall portTaskWrapper(void * pvTaskCode)
{
    TaskFunction_t pxCode = (TaskFunction_t) pvTaskCode;
    printf("Task thread started.\n");
    pxCode(NULL);
    return 0;
}
StackType_t * pxPortInitialiseStack(StackType_t * pxTopOfStack,
                                    TaskFunction_t pxCode,
void * pvParameters)
{
    HANDLE h = (HANDLE)_beginthreadex(
        NULL, 0,
        (unsigned (__stdcall *)(void *)) portTaskWrapper, // 👈 обвивка
        pxCode, 0, NULL);

    if (h == NULL) {
        printf("FAILED: Could not create task thread!\n");
    } else {
        printf("Task thread created OK!\n");
        CloseHandle(h);
    }

    return pxTopOfStack;
}


void vPortYield(void)
{
    SwitchToThread();
}

void vPortInitializeCriticalSection(void)
{
    if (xCriticalSectionInitialised == pdFALSE) {
        InitializeCriticalSection(&xCriticalSection);
        xCriticalSectionInitialised = pdTRUE;
    }
}

void vPortEnterCritical(void)
{
    if (xCriticalSectionInitialised != pdFALSE) {
        EnterCriticalSection(&xCriticalSection);
    }
}

void vPortExitCritical(void)
{
    if (xCriticalSectionInitialised != pdFALSE) {
        LeaveCriticalSection(&xCriticalSection);
    }
}

void vPortEndScheduler(void)
{
    if (hTimerThread != NULL) {
        CloseHandle(hTimerThread);
        hTimerThread = NULL;
    }
}
