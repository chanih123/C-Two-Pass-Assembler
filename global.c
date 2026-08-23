#include "global.h"

/* Real allocation of global memory counters and dynamic buffers */
int IC = IC_INIT_VALUE;
int DC = 0;

BYTE *code_image = NULL;
int code_capacity = 0;

BYTE *data_image = NULL;
int data_capacity = 0;
