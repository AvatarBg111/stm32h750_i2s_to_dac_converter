#ifndef STM32H750_DAC_H
#define STM32H750_DAC_H

// Included files
#include "main.h"

// Definitions and macros
#define DAC_BUFFER_SIZE 64
#if(0)
#define DAC_CH1_READY_FLAG 0x01
#define DAC_CH2_READY_FLAG 0x10
#else
#define DAC_CH1_HALF1_COMPLETE_FLAG 0x01
#define DAC_CH1_HALF2_COMPLETE_FLAG 0x02
#define DAC_CH2_HALF1_COMPLETE_FLAG 0x10
#define DAC_CH2_HALF2_COMPLETE_FLAG 0x20
#endif

#define DAC_CH1_PIN GPIO_PIN_4
#define DAC_CH2_PIN GPIO_PIN_5

#define DAC_CH1_PORT GPIOA
#define DAC_CH2_PORT GPIOA

// Variable extentions
extern DAC_HandleTypeDef dac;
extern uint32_t DAC_CH1_DATA[DAC_BUFFER_SIZE];
extern uint32_t DAC_CH2_DATA[DAC_BUFFER_SIZE];
extern uint8_t DAC_STATUS_REGISTER;

// Function declarations
void Init_DAC_dual_channel();
#if(0)
HAL_StatusTypeDef dac_dual_ch_start_conv();
#endif
HAL_StatusTypeDef dac_ch1_start_conversion_buf(uint32_t*);

#endif  // STM32H750_DAC_H