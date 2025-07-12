#ifndef MAIN_H
#define MAIN_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "device2.h"
struct device {
    unsigned int id;
    SlaveState currentState;
};

extern TaskHandle_t xMasterTaskHandle;
extern SemaphoreHandle_t xQueueMutex;
extern SemaphoreHandle_t xConsoleMutex;
extern struct device device2;

#endif /* MAIN_H */