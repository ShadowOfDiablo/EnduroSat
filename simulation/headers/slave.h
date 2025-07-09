
#ifndef ___SLAVE_H___
#define ___SLAVE_H___
#include "stdio.h"
enum dev2_state {SLEEP,ACTIVE,FAULT};
struct device
{
    unsigned int id;
    enum dev2_state currentState;
};
#endif //___SLAVE_H___