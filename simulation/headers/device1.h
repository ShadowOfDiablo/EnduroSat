#ifndef DEVICE1_H
#define DEVICE1_H

typedef enum {
    MASTER_IDLE,
    MASTER_PROCESSING,
    MASTER_ERROR
} MasterState;

void gl_device1Init(void);

#endif // DEVICE1_H