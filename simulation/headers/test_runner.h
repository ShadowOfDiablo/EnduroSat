#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include "device2.h" // For SlaveState

// Commands to be sent to the slave task for testing
typedef enum {
    CMD_SET_STATE_SLEEP,
    CMD_SET_STATE_ACTIVE,
    CMD_SET_STATE_FAULT
} SlaveCommand;

// Initializes and starts the test runner task.
void test_runner_init(void);

#endif // TEST_RUNNER_H