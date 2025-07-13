#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "main.h"
#include "test_runner.h"
#include <stdio.h>
#include <windows.h> // For Sleep()

// --- Unit Test Framework Structures and Macros ---

typedef struct {
    int passes;
    int failures;
} TestStats;

// Core assertion function with a workaround for the faulty FreeRTOS port
static void assert_state(TestStats *stats, SlaveState expected, const char* file, int line) {
    SlaveState actual = SLAVE_SLEEP;
    
    // Give the master task a moment to react to the state change before we verify
    Sleep(750); // This delay must be longer than the master's polling time (500ms)

    // --- CRASH WORKAROUND ---
    // The custom port.c crashes on timed semaphore waits.
    // We use a non-blocking poll instead to avoid the kernel ASSERT.
    int retries = 5;
    while (xSemaphoreTake(xQueueMutex, 0) != pdTRUE && retries > 0) {
        Sleep(20);
        retries--;
    }

    if (retries > 0) { // If we successfully took the semaphore
        actual = device2.currentState;
        xSemaphoreGive(xQueueMutex);
    } else {
        printf("  [WARNING] Test could not acquire state mutex! Verification skipped.\n");
        stats->failures++;
        return;
    }
    
    if (actual == expected) {
        stats->passes++;
        printf("  [PASS] State is correctly %s (at %s:%d)\n", slaveStateToString(expected), file, line);
    } else {
        stats->failures++;
        printf("  [FAIL] Expected state %s, but got %s (at %s:%d)\n",
               slaveStateToString(expected), slaveStateToString(actual), file, line);
    }
}

#define ASSERT_STATE(stats, expected_state) assert_state(stats, expected_state, __FILE__, __LINE__)

#define RUN_TEST(stats, test_func) do { \
    printf("--- RUNNING: %s ---\n", #test_func); \
    test_func(&stats); \
    printf("--- FINISHED: %s ---\n\n", #test_func); \
} while(0)


// --- Helper Function ---
static void send_command_safe(SlaveCommand cmd) {
    xQueueSend(xSlaveStateCommandQueue, &cmd, portMAX_DELAY);
}


// --- Test Case Implementations ---

static void test_case_normal_operation(TestStats *stats) {
    printf("  [Step 1] Forcing slave to ACTIVE. Master should go to PROCESSING.\n");
    send_command_safe(CMD_SET_STATE_ACTIVE);
    ASSERT_STATE(stats, SLAVE_ACTIVE);

    printf("  [Step 2] Forcing slave to SLEEP. Master should return to IDLE.\n");
    send_command_safe(CMD_SET_STATE_SLEEP);
    ASSERT_STATE(stats, SLAVE_SLEEP);
}

static void test_case_single_fault_and_recovery(TestStats *stats) {
    printf("  [Step 1] Setting up: Forcing slave to ACTIVE.\n");
    send_command_safe(CMD_SET_STATE_ACTIVE);
    ASSERT_STATE(stats, SLAVE_ACTIVE);

    printf("  [Step 2] Forcing slave to FAULT. Master should go to ERROR.\n");
    send_command_safe(CMD_SET_STATE_FAULT);
    ASSERT_STATE(stats, SLAVE_FAULT);

    printf("  [Step 3] Forcing slave to ACTIVE to recover. Master should return to PROCESSING.\n");
    send_command_safe(CMD_SET_STATE_ACTIVE);
    ASSERT_STATE(stats, SLAVE_ACTIVE);
}

static void test_case_persistent_fault_and_reset(TestStats *stats) {
    printf("  [Step 1] Forcing slave to FAULT state (Fault 1 detected).\n");
    send_command_safe(CMD_SET_STATE_FAULT);
    Sleep(600); 

    printf("  [Step 2] Forcing slave to FAULT state (Fault 2 detected).\n");
    send_command_safe(CMD_SET_STATE_FAULT);
    Sleep(600);

    printf("  [Step 3] Forcing slave to FAULT state (Fault 3 detected), triggering reset.\n");
    send_command_safe(CMD_SET_STATE_FAULT);
    Sleep(1200); // Longer delay to ensure reset flag is processed by slave

    printf("  [Step 4] Slave should have been reset and now be ACTIVE.\n");
    ASSERT_STATE(stats, SLAVE_ACTIVE);
}

static void test_case_edge_rapid_state_changes(TestStats *stats) {
    printf("  [Step 1] Rapidly toggling FAULT -> ACTIVE -> FAULT.\n");
    send_command_safe(CMD_SET_STATE_FAULT);
    Sleep(200);
    send_command_safe(CMD_SET_STATE_ACTIVE);
    Sleep(200);
    send_command_safe(CMD_SET_STATE_FAULT);
    
    printf("  [Step 2] Observing final state. Master should be in ERROR.\n");
    ASSERT_STATE(stats, SLAVE_FAULT);

    printf("  [Step 3] Cleaning up: returning to SLEEP state.\n");
    send_command_safe(CMD_SET_STATE_SLEEP);
    ASSERT_STATE(stats, SLAVE_SLEEP);
}


// --- Test Runner Task ---
void run_tests_task(void *pvParameters) {
    (void)pvParameters;
    TestStats stats = {0, 0};

    Sleep(1500); 

    printf("\n\n--- STARTING UNIT TEST SUITE ---\n\n");

    RUN_TEST(stats, test_case_normal_operation);
    RUN_TEST(stats, test_case_single_fault_and_recovery);
    RUN_TEST(stats, test_case_persistent_fault_and_reset);
    RUN_TEST(stats, test_case_edge_rapid_state_changes);

    printf("\n--- UNIT TEST SUITE COMPLETE ---\n");
    printf("Result: %d Passes, %d Failures\n", stats.passes, stats.failures);
    printf("--------------------------------\n\n");

    vTaskDelete(NULL);
}

void test_runner_init(void) {
    xTaskCreate(run_tests_task, "TestRunner", 2048, NULL, configMAX_PRIORITIES - 1, NULL);
}