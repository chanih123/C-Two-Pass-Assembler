/**
 * @file global.c
 * @brief Definition and initialization of global variables for the assembler.
 *
 * This file allocates and initializes the global memory counters (IC and DC)
 * and the dynamic memory buffers used for the code and data images.
 */

#include "global.h"

/* Real allocation of global memory counters and dynamic buffers */

/** @brief Instruction Counter: tracks the current memory address for instructions. */
int IC = IC_INIT_VALUE;

/** @brief Data Counter: tracks the current memory address for data directives. */
int DC = 0;

/** @brief Dynamic array holding the encoded machine instructions (Code Image). */
BYTE *code_image = NULL;

/** @brief Current allocated capacity of the code_image buffer. */
int code_capacity = 0;

/** @brief Dynamic array holding the encoded data elements (Data Image). */
BYTE *data_image = NULL;

/** @brief Current allocated capacity of the data_image buffer. */
int data_capacity = 0;
