#ifndef ___DEVICE1_H___
#define ___DEVICE1_H___

#define DEVICE_ID   0x01
#define MAX_SLAVES  1

void device1_init(void);
void device1_main_task(void *pvParameters);

#endif // ___DEVICE1_H___