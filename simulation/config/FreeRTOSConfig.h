#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H
/* Modified version of Amazon's config */
/* Essential configuration */
#define configUSE_16_BIT_TICKS          0
#define configUSE_PREEMPTION            1
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0
#define configTICK_RATE_HZ              1000
#define configMAX_PRIORITIES            5
#define configTOTAL_HEAP_SIZE           (size_t)(36*1024)
#define configMAX_TASK_NAME_LEN         16

/* Memory allocation */
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configSUPPORT_STATIC_ALLOCATION  0

/* Optional configuration */
#define configUSE_TRACE_FACILITY        0
#define configUSE_APPLICATION_TASK_TAG  0
#define configENABLE_BACKWARD_COMPATIBILITY 0
#define configUSE_TASK_NOTIFICATIONS    1
#define configUSE_TIMERS                1
#define configTIMER_TASK_PRIORITY       (configMAX_PRIORITIES-1)
#define configTIMER_QUEUE_LENGTH        10
#define configTIMER_TASK_STACK_DEPTH    configMINIMAL_STACK_SIZE

/* Hardware specifics */
#define configCPU_CLOCK_HZ              (unsigned long)100000000
#define configMINIMAL_STACK_SIZE        ((uint16_t)768)  // Increased to 768

/* Error checking */
#include "fake_assert.h"  // Include for vFakeAssert declaration
#define configASSERT(x) if((x) == 0) vFakeAssert(false, __FILE__, __LINE__)

/* Stack overflow hook */
#define configCHECK_FOR_STACK_OVERFLOW  1

/* Priority management */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0

/* Needed for the original function*/
#define INCLUDE_vTaskDelay             1

#endif /* FREERTOS_CONFIG_H */