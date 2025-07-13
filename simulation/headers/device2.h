#ifndef DEVICE2_H
#define DEVICE2_H

typedef enum {
    SLAVE_SLEEP,
    SLAVE_ACTIVE,
    SLAVE_FAULT
} SlaveState;

void gl_device2Init(void);

#endif // DEVICE2_H