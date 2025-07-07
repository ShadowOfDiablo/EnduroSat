#ifndef ___DEVICE2_H___
#define ___DEVICE2_H___
#include "slave.h"

#define SLAVE_ID 2
#define MASTER_ID 1

void device2_task(void *pvParameters);
void vUDPSendTask(void *pvParameters);
void vUDPReceiveTask(void *pvParameters);

#endif // ___DEVICE2_H___