#include "portmacro.h"

UBaseType_t uxCriticalNesting = 0;

void vPortEnterCritical(void) {
    uxCriticalNesting++;
}

void vPortExitCritical(void) {
    if (uxCriticalNesting > 0) {
        uxCriticalNesting--;
    }
}