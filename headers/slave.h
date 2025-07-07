#include "stdio.h"
enum dev2_state {SLEEP,ACTIVE,FAULT};
struct device
{
    unsigned int id;
    enum dev2_state currentState;
};