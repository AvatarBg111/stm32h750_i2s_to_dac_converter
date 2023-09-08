#ifndef STM32H750_DMA_H
#define STM32H750_DMA_H

// Included files
#include "main.h"

// Definitions and macros

// Variable extentions
extern DMA_HandleTypeDef dma_i2s2_rx;
extern DMA_HandleTypeDef dma_dac_ch1;
extern DMA_HandleTypeDef dma_dac_ch2;

// Function declarations
void Init_DMA_I2S();
void Init_DMA_DAC_CH1();
void Init_DMA_DAC_CH2();

#endif  // STM32H750_DMA_H