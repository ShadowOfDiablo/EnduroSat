#ifndef MAIN_H
#define MAIN_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "device1.h"
#include "device2.h"

#define FAULT_RESET_THRESHOLD 3

struct device {
    unsigned int id;
    SlaveState currentState;
};

extern TaskHandle_t pSlaveTaskHandle;
extern SemaphoreHandle_t xQueueMutex;
extern SemaphoreHandle_t xConsoleMutex;
extern struct device sDevice2;
extern volatile BaseType_t slaveResetRequested;

const char* gl_slaveStateToString(SlaveState state);
const char* gl_masterStateToString(MasterState state);

#endif /* MAIN_H */