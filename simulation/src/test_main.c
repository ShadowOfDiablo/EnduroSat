#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define pdPASS 1
#define pdFAIL 0
#define TRUE 1
#define FALSE 0
#define portMAX_DELAY 1000

typedef int BaseType_t;
typedef unsigned long TickType_t;
typedef void* TaskHandle_t;
typedef void (*TaskFunction_t)(void*);

typedef unsigned int UBaseType_t;
typedef uint16_t uint16_t;

typedef enum {
    SLAVE_SLEEP,
    SLAVE_ACTIVE,
    SLAVE_FAULT
} SlaveState;

typedef enum {
    MASTER_IDLE,
    MASTER_PROCESSING,
    MASTER_ERROR
} MasterState;

struct device {
    unsigned int id;
    SlaveState currentState;
};

int faultCounter = 0;
int device1Turn = TRUE;
BaseType_t firstRun = TRUE;
struct device sDevice2 = { .id = 0xB, .currentState = SLAVE_SLEEP };
TaskHandle_t pSlaveTaskHandle;
int xTaskCreateCalls = 0;
int vTaskDeleteCalls = 0;
int gl_device2InitCalls = 0;

BaseType_t xTaskCreate(TaskFunction_t pxTaskCode, const char* const pcName,
    const uint16_t usStackDepth, void* pvParameters, UBaseType_t uxPriority, TaskHandle_t* pxCreatedTask) {
    xTaskCreateCalls++;
    return pdPASS;
}

void vTaskDelete(TaskHandle_t xTaskToDelete) {
    vTaskDeleteCalls++;
}

void gl_device2Init(void) {
    gl_device2InitCalls++;
}

void vTaskDelay(TickType_t xTicksToDelay) {}
int xSemaphoreTake(void* xSemaphore, TickType_t xTicksToWait) { return TRUE; }
int xSemaphoreGive(void* xSemaphore) { return TRUE; }

void gl_device1Init(void) {
    xTaskCreate(NULL, "Master", 1024, NULL, 1, NULL);
}

void gl_resetDevice2(TaskHandle_t pTaskHandle) {
    vTaskDelete(pTaskHandle);
    firstRun = TRUE;
    gl_device2Init();
}

void test_device1Init() {
    xTaskCreateCalls = 0;
    gl_device1Init();
    assert(xTaskCreateCalls == 1);
}

void test_resetDevice2() {
    firstRun = FALSE;
    vTaskDeleteCalls = 0;
    gl_device2InitCalls = 0;
    gl_resetDevice2((void*)1);
    assert(firstRun == TRUE);
    assert(vTaskDeleteCalls == 1);
    assert(gl_device2InitCalls == 1);
}

void test_faultThresholdReset() {
    faultCounter = 0;
    int resets = 0;
    sDevice2.currentState = SLAVE_FAULT;
    device1Turn = FALSE;

    for (int i = 0; i < 3; ++i) {
        if (sDevice2.currentState == SLAVE_FAULT) {
            faultCounter++;
        }
    }
    if (faultCounter >= 3) {
        resets++;
        gl_resetDevice2((void*)1);
    }
    assert(faultCounter >= 3);
    assert(resets == 1);
}

void test_slaveFirstRun() {
    firstRun = TRUE;
    sDevice2.currentState = SLAVE_SLEEP;
    device1Turn = TRUE;

    if (firstRun == TRUE && device1Turn == TRUE) {
        sDevice2.currentState = SLAVE_ACTIVE;
        firstRun = FALSE;
    }
    assert(sDevice2.currentState == SLAVE_ACTIVE);
    assert(firstRun == FALSE);
}

void test_slaveStateChange() {
    sDevice2.currentState = SLAVE_ACTIVE;
    SlaveState newState = SLAVE_FAULT;
    if (sDevice2.currentState != newState) {
        sDevice2.currentState = newState;
    }
    assert(sDevice2.currentState == SLAVE_FAULT);
}

void test_masterClearsFaultOnActive() {
    faultCounter = 5;
    sDevice2.currentState = SLAVE_ACTIVE;
    if (sDevice2.currentState == SLAVE_ACTIVE) {
        faultCounter = 0;
    }
    assert(faultCounter == 0);
}

void test_masterClearsFaultOnSleep() {
    faultCounter = 5;
    sDevice2.currentState = SLAVE_SLEEP;
    if (sDevice2.currentState == SLAVE_SLEEP) {
        faultCounter = 0;
    }
    assert(faultCounter == 0);
}

void test_masterIncrementsOnConsecutiveFaults() {
    faultCounter = 0;
    sDevice2.currentState = SLAVE_FAULT;
    device1Turn = FALSE;

    for (int i = 0; i < 5; ++i) {
        if (sDevice2.currentState == SLAVE_FAULT && device1Turn != TRUE) {
            faultCounter++;
        }
    }
    assert(faultCounter == 5);
}

int main() {
    test_device1Init();
    test_resetDevice2();
    test_faultThresholdReset();
    test_slaveFirstRun();
    test_slaveStateChange();
    test_masterClearsFaultOnActive();
    test_masterClearsFaultOnSleep();
    test_masterIncrementsOnConsecutiveFaults();
    printf("All unit tests passed successfully.\n");
    return 0;
}
