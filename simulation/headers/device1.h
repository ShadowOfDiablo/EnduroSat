#ifndef DEVICE1_H
#define DEVICE1_H

typedef enum {
    MASTER_IDLE,
    MASTER_PROCESSING,
    MASTER_ERROR
} MasterState;

void device1_init(void);

#endif // DEVICE1_H