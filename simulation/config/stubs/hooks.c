#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "fake_assert.h"

void vFakeAssert(bool x, char* file, int line)
{
    (void)x;
    printf("Assertion failed at %s:%d\n", file, line);
    exit(1);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    printf("Stack overflow in task: %s\n", pcTaskName);
    exit(1);
}

void vApplicationMallocFailedHook(void) {
    printf("Memory allocation failed! Free heap: %zu bytes\n", xPortGetFreeHeapSize());
    exit(1);
}

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
    
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize) {
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];
    
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vConfigureTimerForRunTimeStats(void)
{

}

void portRECORD_READY_PRIORITY(UBaseType_t uxPriority, UBaseType_t uxReadyPriorities)
{
    // Dummy implementation
}

void portRESET_READY_PRIORITY(UBaseType_t uxPriority, UBaseType_t uxReadyPriorities)
{
    // Dummy implementation
}

UBaseType_t portGET_HIGHEST_PRIORITY(UBaseType_t uxTopPriority, UBaseType_t uxReadyPriorities)
{
    return 0; // Dummy implementation
}

void vApplicationIdleHook(void) {
    printf("Idle hook\n");
}

void vApplicationTickHook(void) {}
void vApplicationDaemonTaskStartupHook(void) {}