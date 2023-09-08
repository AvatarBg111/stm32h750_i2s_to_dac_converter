#ifndef STM32H750_I2S_H
#define STM32H750_I2S_H

// Included files
#include "main.h"
#include "dac.h"

// Definitions and macros
#define I2S_BUFFER_SIZE 128
#define I2S2_HALF1_BUFFER_READY_FLAG 0x01
#define I2S2_HALF2_BUFFER_READY_FLAG 0x10

#define I2S2_WS_PIN GPIO_PIN_12
#define I2S2_CK_PIN GPIO_PIN_13
#define I2S2_SDI_PIN GPIO_PIN_14
#define I2S2_SDO_PIN GPIO_PIN_15

#define I2S2_WS_PORT GPIOB
#define I2S2_CK_PORT GPIOB
#define I2S2_SDI_PORT GPIOB
#define I2S2_SDO_PORT GPIOB

// Variable extentions
extern I2S_HandleTypeDef i2s2;
extern int16_t I2S2_DATA[I2S_BUFFER_SIZE];
extern uint8_t I2S2_STATUS_REGISTER;

extern uint32_t DAC_CH1_BUFFER[DAC_BUFFER_SIZE];

// Function declarations
void clear_i2s2_buf();
void Init_I2S2();
HAL_StatusTypeDef i2s2_start_receiption();
HAL_StatusTypeDef i2s2_pause_receiption();
HAL_StatusTypeDef i2s2_resume_receiption();

#endif  // STM32H750_I2S_H