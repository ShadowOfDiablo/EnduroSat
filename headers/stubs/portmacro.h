#ifndef PORTMACRO_H
#define PORTMACRO_H

#include <stdint.h>

/* Remove duplicate definition */
#undef portSTACK_GROWTH

#define portCHAR        char
#define portFLOAT       float
#define portDOUBLE      double
#define portLONG        long
#define portSHORT       short
#define portSTACK_TYPE  uint32_t
#define portBASE_TYPE   long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#define portENTER_CRITICAL()  
#define portEXIT_CRITICAL()   
#define portDISABLE_INTERRUPTS() 
#define portENABLE_INTERRUPTS()  
#define portBYTE_ALIGNMENT 8

/* Task utilities */
#define portYIELD() 
#define portNOP() 

/* Stack growth direction */
#define portSTACK_GROWTH (-1)

/* Tick count type */
#if configUSE_16_BIT_TICKS == 1
    typedef uint16_t TickType_t;
    #define portMAX_DELAY (TickType_t)0xFFFF
#else
    typedef uint32_t TickType_t;
    #define portMAX_DELAY (TickType_t)0xFFFFFFFF
#endif

/* Task function macros */
#define portTASK_FUNCTION_PROTO(vFunction, pvParameters) void vFunction(void *pvParameters)
#define portTASK_FUNCTION(vFunction, pvParameters) void vFunction(void *pvParameters)

#endif /* PORTMACRO_H */