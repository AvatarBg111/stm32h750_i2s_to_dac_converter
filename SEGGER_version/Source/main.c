/**
  ******************************************************************************
  *                                 GreenBo         
  *                           The Embedded Experts
  ******************************************************************************
  * @file    main.c
  * @brief   Generic application start
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 GreenBo.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by GreenBo under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  **/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include "dac.h"
#include "i2s.h"
#include "dma.h"
#include "tim.h"
#include "systick_timer.h"

/* Function declarations -----------------------------------------------------*/
void SystemClock_Config(void);

/* Variable definitions ------------------------------------------------------*/
uint16_t phase = 0xFFF;

void MCU_Init(){
    HAL_Init();
    SystemClock_Config();

    __enable_irq();
    HAL_NVIC_SetPriority(SysTick_IRQn, 0x0, 0x0);
    HAL_NVIC_EnableIRQ(SysTick_IRQn);

    Init_GPIO();
    Init_I2S2();
    Init_TIM6();
    Init_DAC_dual_channel();
}

/* Function definitions ------------------------------------------------------*/
void LED_Blink(uint32_t ms){
    if(wait_ms_ch(0, ms)){
        if(HAL_GPIO_ReadPin(PE3_GPIO_Port, PE3_Pin) == GPIO_PIN_RESET){
            printf("Led on\n");
            HAL_GPIO_WritePin(PE3_GPIO_Port, PE3_Pin, GPIO_PIN_SET);
        }else{
            printf("Led off\n");
            HAL_GPIO_WritePin(PE3_GPIO_Port, PE3_Pin, GPIO_PIN_RESET);
        }
    }
}

/* Main program --------------------------------------------------------------*/
int main(void){
    MCU_Init();

    while(!wait_ms_ch(3, 50));
    /*
    printf("SPI/I2S configuration -> CFGR2: 0x%X\n", i2s2.Instance->CFG2);
    printf("SPI/I2S interrupts -> IER: 0x%X\n", i2s2.Instance->IER);
    printf("SPI/I2S status register -> SR: 0x%X\n", i2s2.Instance->SR);
    printf("SPI/I2S configuration register -> I2SCFGR: 0x%X\n", i2s2.Instance->I2SCFGR);
    */

    while(1){
        if(i2s2_start_receiption() != HAL_OK){
            printf("I2S2 ERR: could not initialize a receive process\n");
            while(wait_ms_ch(1, 2));
        }else{
            break;
        }
    }

    while(1){
        if(dac_ch1_start_conversion_buf((uint32_t*)&DAC_CH1_BUFFER) != HAL_OK){
            printf("DAC ERR: could not initialize a conversion process\n");
            while(wait_ms_ch(1, 2));
        }else{
            break;
        }
    }

    // Infinite loop
    while(1){
        LED_Blink(850);
        /*
        if(I2S2_STATUS_REGISTER & I2S2_HALF2_BUFFER_READY_FLAG){
            I2S2_STATUS_REGISTER &= ~(0x00 | I2S2_HALF2_BUFFER_READY_FLAG);

            printf("DATA:\n\t[\n");
            for(int i = 0; i < I2S_BUFFER_SIZE; i++){
                if(!(i % 16) && i != 0){
                    printf("\n");
                }
                printf("0x%X, ", I2S2_DATA[i]);
            }
            printf("\n\t]\nVAL: %d\n", i2s2.Instance->RXDR);
        }
        */
    }

    return 0;
}

void SystemClock_Config(void){
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Supply configuration update enable
    */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

    /** Configure the main internal regulator output voltage
    */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 9;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOMEDIUM;
    RCC_OscInitStruct.PLL.PLLFRACN = 3072;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;   // RCC_APB1_DIV2
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK){
        Error_Handler();
    }
}

void CPU_CACHE_Enable(void){
    SCB_EnableICache();
    SCB_EnableDCache();
}

void Error_Handler(void){
    while(1){
        LED_Blink(10);
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT GreenBo *****************END OF FILE****/