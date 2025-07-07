#include "portmacro.h"

static UBaseType_t uxCriticalNesting = 0;

void vPortEnterCritical(void) {
    uxCriticalNesting++;
    // In a real port, you'd disable interrupts here
}

void vPortExitCritical(void)
{
    if (uxCriticalNesting > 0) {
        uxCriticalNesting--;
    }
}