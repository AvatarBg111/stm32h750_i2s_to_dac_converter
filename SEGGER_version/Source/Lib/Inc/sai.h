#ifndef STM32H750_SAI_H
#define STM32H750_SAI_H

// Included files
#include "main.h"

// Definitions and macros

// Variable extentions
extern SAI_HandleTypeDef sai1;
extern DMA_HandleTypeDef dma1;

// Function declarations
void Init_SAI();

#endif  // STM32H750_SAI_H