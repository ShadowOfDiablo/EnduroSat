#include "FreeRTOS.h"

#if(configAPPLICATION_ALLOCATED_HEAP == 1)
    uint8_t ucHeap[configTOTAL_HEAP_SIZE];
#endif

void vPortDefineHeapRegions(void) {
    extern uint8_t ucHeap[configTOTAL_HEAP_SIZE];
    xHeapRegions[0].pucStartAddress = ucHeap;
    xHeapRegions[0].xSizeInBytes = configTOTAL_HEAP_SIZE;
    xHeapRegions[1].pucStartAddress = NULL;
    xHeapRegions[1].xSizeInBytes = 0;
    
    vPortDefineHeapRegions(xHeapRegions);
}

size_t xPortGetFreeHeapSize(void) {
    return xPortGetFreeHeapSize();
}

size_t xPortGetMinimumEverFreeHeapSize(void) {
    return xPortGetMinimumEverFreeHeapSize();
}

void *pvPortMalloc(size_t xSize) {
    return pvPortMalloc(xSize);
}

void vPortFree(void *pv) {
    vPortFree(pv);
}