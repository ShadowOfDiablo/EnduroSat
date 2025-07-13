#include "FreeRTOS.h"
#include "task.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <process.h>
    #define Sleep(ms) Sleep(ms)
#else
    #include <pthread.h>
    #include <unistd.h>
    #include <stdlib.h>
    #define Sleep(ms) usleep((ms) * 1000)
#endif

#include <stdio.h>

#define MAX_TASKS 8

typedef struct {
    TaskFunction_t pxCode;
    void *pvParameters;
} FakeTask;

static FakeTask createdTasks[MAX_TASKS];
static int numCreatedTasks = 0;

#if defined(_WIN32) || defined(_WIN64)
    static HANDLE hTimerThread = NULL;
    static CRITICAL_SECTION xCriticalSection;
    static BaseType_t xCriticalSectionInitialised = pdFALSE;
#else
    static pthread_t hTimerThread;
    static pthread_mutex_t xCriticalSection;
    static BaseType_t xCriticalSectionInitialised = pdFALSE;
#endif

extern void internal_master_task(void *);
extern void internal_slave_task(void *);

#if defined(_WIN32) || defined(_WIN64)
unsigned __stdcall prvTimerThread(void *lpParameter);
#else
void* prvTimerThread(void *lpParameter);
#endif

void vPortInitializeCriticalSection(void);

BaseType_t xPortStartScheduler(void) {
    vPortInitializeCriticalSection();

#if defined(_WIN32) || defined(_WIN64)
    hTimerThread = (HANDLE)_beginthreadex(NULL, 0, prvTimerThread, NULL, 0, NULL);
    if (hTimerThread == NULL) {
        printf("FAILED to create timer\n");
        return pdFAIL;
    }
#else
    if (pthread_create(&hTimerThread, NULL, prvTimerThread, NULL) != 0) {
        printf("FAILED to create timer\n");
        return pdFAIL;
    }
#endif

    printf("Timer thread created OK!\n");

    for (int i = 0; i < numCreatedTasks; ++i) {
#if defined(_WIN32) || defined(_WIN64)
        HANDLE h = (HANDLE)_beginthreadex(NULL, 0,
            (unsigned (__stdcall *)(void *)) createdTasks[i].pxCode,
            createdTasks[i].pvParameters, 0, NULL);
        if (h == NULL) printf("FAILED to start task %d\n", i);
        else {
            printf("Task thread started.\n");
            CloseHandle(h);
        }
#else
        pthread_t h;
        if (pthread_create(&h, NULL, (void *(*)(void *)) createdTasks[i].pxCode,
            createdTasks[i].pvParameters) != 0) {
            printf("FAILED to start task %d\n", i);
        } else {
            printf("Task thread started.\n");
        }
#endif
    }

#if defined(_WIN32) || defined(_WIN64)
    WaitForSingleObject(hTimerThread, INFINITE);
#else
    pthread_join(hTimerThread, NULL);
#endif

    return pdPASS;
}

#if defined(_WIN32) || defined(_WIN64)
unsigned __stdcall prvTimerThread(void *lpParameter)
#else
void* prvTimerThread(void *lpParameter)
#endif
{
    printf("!!! Timer thread ENTRY !!!\n");
    const TickType_t xTickPeriodms = 1000 / configTICK_RATE_HZ;
    printf(">> Timer thread STARTED. Tick every %d ms\n", xTickPeriodms);

    while (1) {
        Sleep(xTickPeriodms);

        if (xTaskIncrementTick() != pdFALSE) {
            vTaskSwitchContext();
        }
    }
#if !defined(_WIN32) && !defined(_WIN64)
    return NULL;
#endif
}

void vPortStartFirstTask(void) {
    printf("Starting fake FreeRTOS scheduler...\n");
    for (int i = 0; i < numCreatedTasks; ++i) {
#if defined(_WIN32) || defined(_WIN64)
        HANDLE h = (HANDLE)_beginthreadex(NULL, 0,
            (unsigned (__stdcall *)(void *)) createdTasks[i].pxCode,
            createdTasks[i].pvParameters, 0, NULL);

        if (h == NULL) {
            printf("FAILED: Could not start task %d\n", i);
        } else {
            printf("Task thread %d started.\n", i);
            CloseHandle(h);
        }
#else
        pthread_t h;
        if (pthread_create(&h, NULL, (void *(*)(void *)) createdTasks[i].pxCode,
            createdTasks[i].pvParameters) != 0) {
            printf("FAILED: Could not start task %d\n", i);
        } else {
            printf("Task thread %d started.\n", i);
        }
#endif
    }
}

#if defined(_WIN32) || defined(_WIN64)
unsigned __stdcall portTaskWrapper(void * pvTaskCode)
#else
void* portTaskWrapper(void * pvTaskCode)
#endif
{
    TaskFunction_t pxCode = (TaskFunction_t) pvTaskCode;
    printf("Task thread started.\n");
    pxCode(NULL);
#if !defined(_WIN32) && !defined(_WIN64)
    return NULL;
#endif
}

StackType_t * pxPortInitialiseStack(StackType_t * pxTopOfStack,
                                    TaskFunction_t pxCode,
                                    void * pvParameters)
{
#if defined(_WIN32) || defined(_WIN64)
    HANDLE h = (HANDLE)_beginthreadex(NULL, 0, (unsigned (__stdcall *)(void *)) portTaskWrapper, pxCode, 0, NULL);
    if (h == NULL) {
        printf("FAILED: Could not create task thread!\n");
    } else {
        printf("Task thread created OK!\n");
        CloseHandle(h);
    }
#else
    pthread_t h;
    if (pthread_create(&h, NULL, portTaskWrapper, pxCode) != 0) {
        printf("FAILED: Could not create task thread!\n");
    } else {
        printf("Task thread created OK!\n");
    }
#endif
    return pxTopOfStack;
}

void vPortYield(void) {
#if defined(_WIN32) || defined(_WIN64)
    SwitchToThread();
#else
    sched_yield();
#endif
}

void vPortInitializeCriticalSection(void) {
    if (xCriticalSectionInitialised == pdFALSE) {
#if defined(_WIN32) || defined(_WIN64)
        InitializeCriticalSection(&xCriticalSection);
#else
        pthread_mutex_init(&xCriticalSection, NULL);
#endif
        xCriticalSectionInitialised = pdTRUE;
    }
}

void vPortEnterCritical(void) {
    if (xCriticalSectionInitialised != pdFALSE) {
#if defined(_WIN32) || defined(_WIN64)
        EnterCriticalSection(&xCriticalSection);
#else
        pthread_mutex_lock(&xCriticalSection);
#endif
    }
}

void vPortExitCritical(void) {
    if (xCriticalSectionInitialised != pdFALSE) {
#if defined(_WIN32) || defined(_WIN64)
        LeaveCriticalSection(&xCriticalSection);
#else
        pthread_mutex_unlock(&xCriticalSection);
#endif
    }
}

void vPortEndScheduler(void) {
#if defined(_WIN32) || defined(_WIN64)
    if (hTimerThread != NULL) {
        CloseHandle(hTimerThread);
        hTimerThread = NULL;
    }
#else
    // Nothing required for pthread
#endif
}
