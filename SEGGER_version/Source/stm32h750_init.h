#ifndef STM32H750_INIT_H
#define STM32H750_INIT_H

/** INCLUDES **/
#include "main.h"

#include "stm32h7xx_hal_adc.h"
#include "stm32h7xx_hal_ltdc.h"
#include "stm32h7xx_hal_uart.h"
#include "stm32h7xx_hal_def.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_ll_rcc.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_dac.h"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_spi.h"
#include "stm32h7xx_hal_dsi.h"
#include "stm32h7xx_hal_jpeg.h"
#include "stm32h7xx_hal_dcmi.h"

#define E3_Pin GPIO_PIN_3
#define E3_GPIO_Port GPIOE
#define KEY_Pin GPIO_PIN_13
#define KEY_GPIO_Port GPIOC
#define LCD_CS_Pin GPIO_PIN_11
#define LCD_CS_GPIO_Port GPIOE
#define LCD_WR_RS_Pin GPIO_PIN_13
#define LCD_WR_RS_GPIO_Port GPIOE

/** FUNCTIONS **/
void SystemClock_Config();
void Init_DAC12();
void Init_ADC1();
void Init_ADC2();
void Init_ADC12();
void Init_TIM1();
void Init_TIM2();
void Init_TIM3();
void Init_TIM4();
void Init_TIM8();
void Init_UART5();
void Init_UART5_DMA();
void Init_SPI1();
void Init_RTC();
void print_frequencies();

void Error_Handler(void);

/** VARIABLES **/
extern DAC_HandleTypeDef dac1;
extern ADC_HandleTypeDef adc1;
extern ADC_HandleTypeDef adc2;
extern ADC_HandleTypeDef adc12;
extern TIM_HandleTypeDef tim1;
extern TIM_HandleTypeDef tim2;
extern TIM_HandleTypeDef tim3;
extern TIM_HandleTypeDef tim4;
extern TIM_HandleTypeDef tim8;
extern TIM_OC_InitTypeDef PWM_Config;
extern TIM_OC_InitTypeDef PWM_Config2;
extern UART_HandleTypeDef uart5;
extern SPI_HandleTypeDef spi1;
extern RTC_HandleTypeDef hrtc;
extern DMA_HandleTypeDef DMA_InitStructure;
extern DMA_HandleTypeDef dma_adc1;
extern DMA_HandleTypeDef dma_adc2;

extern uint8_t uart5_tx_buf[];
extern uint8_t uart5_rx_buf[200];

#endif