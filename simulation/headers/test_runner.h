#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include "device2.h" 


typedef enum {
    CMD_SET_STATE_SLEEP,
    CMD_SET_STATE_ACTIVE,
    CMD_SET_STATE_FAULT
} SlaveCommand;

void test_runner_init(void);

#endif // TEST_RUNNER_H