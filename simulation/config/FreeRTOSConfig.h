#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* Essential configuration */
#define configUSE_16_BIT_TICKS              0
#define configUSE_PREEMPTION                1
#define configUSE_TIME_SLICING              1
#define configTICK_RATE_HZ                  ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES                5
#define configTOTAL_HEAP_SIZE               ((size_t)(4096 * 1024)) //4mb
#define configMAX_TASK_NAME_LEN             16
#define configMINIMAL_STACK_SIZE            256 
#define configTIMER_TASK_STACK_DEPTH        4096
#define configCPU_CLOCK_HZ                  ( ( unsigned long ) 100000000 )
#define configBYTE_ALIGNMENT                8

/* Memory allocation */
#define configSUPPORT_DYNAMIC_ALLOCATION    1
#define configSUPPORT_STATIC_ALLOCATION     1

/* Task utilities */
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_xTaskGetSchedulerState      1
#define INCLUDE_vTaskDelayUntil             1

/* Output configuration */
#define configUSE_STDIO                     1
#define configPRINTF( x )                   printf x

/* Error checking */
extern void vAssertCalled( const char * const pcFileName, unsigned long ulLine );
#define configASSERT( x )                   if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )

/* Hooks */
#define configCHECK_FOR_STACK_OVERFLOW      0
#define configUSE_MALLOC_FAILED_HOOK        1

/* Kernel features */
#define configUSE_IDLE_HOOK                 1
#define configUSE_TICK_HOOK                 0
#define configUSE_DAEMON_TASK_STARTUP_HOOK  0

/* Priority management */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0

/* Optional configuration */
#define configUSE_TRACE_FACILITY            0
#define configUSE_APPLICATION_TASK_TAG      0
#define configENABLE_BACKWARD_COMPATIBILITY 0
#define configUSE_TASK_NOTIFICATIONS        1
#define configUSE_TIMERS                    0
#define configIDLE_SHOULD_YIELD             1
#define configTIMER_TASK_PRIORITY           ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH            1000
#define configAPPLICATION_ALLOCATED_HEAP    0
#define configUSE_MUTEXES                   1
#define configUSE_COUNTING_SEMAPHORES       1
#define configUSE_QUEUE_SETS                1

#endif /* FREERTOS_CONFIG_H */