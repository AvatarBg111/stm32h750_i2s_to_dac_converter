/*********************************************************************
*                   (c) SEGGER Microcontroller GmbH                  *
*                        The Embedded Experts                        *
*                           www.segger.com                           *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : STM32H750x_Vectors.s
Purpose : Exception and interrupt vectors for STM32H750x devices.
Additional information:
  Preprocessor Definitions
    __NO_EXTERNAL_INTERRUPTS
      If defined,
        the vector table will contain only the internal exceptions
        and interrupts.
    __VECTORS_IN_RAM
      If defined,
        an area of RAM, large enough to store the vector table,
        will be reserved.

    __OPTIMIZATION_SMALL
      If defined,
        all weak definitions of interrupt handlers will share the
        same implementation.
      If not defined,
        all weak definitions of interrupt handlers will be defined
        with their own implementation.
*/

        .syntax unified

/*********************************************************************
*
*       Macros
*
**********************************************************************
*/

//
// Directly place a vector (word) in the vector table
//
.macro VECTOR Name=
        .section .vectors, "ax"
        .code 16
        .word \Name
.endm

//
// Declare an exception handler with a weak definition
//
.macro EXC_HANDLER Name=
        //
        // Insert vector in vector table
        //
        .section .vectors, "ax"
        .word \Name
        //
        // Insert dummy handler in init section
        //
        .section .init.\Name, "ax"
        .thumb_func
        .weak \Name
        .balign 2
\Name:
        1: b 1b   // Endless loop
.endm

//
// Declare an interrupt handler with a weak definition
//
.macro ISR_HANDLER Name=
        //
        // Insert vector in vector table
        //
        .section .vectors, "ax"
        .word \Name
        //
        // Insert dummy handler in init section
        //
#if defined(__OPTIMIZATION_SMALL)
        .section .init, "ax"
        .weak \Name
        .thumb_set \Name,Dummy_Handler
#else
        .section .init.\Name, "ax"
        .thumb_func
        .weak \Name
        .balign 2
\Name:
        1: b 1b   // Endless loop
#endif
.endm

//
// Place a reserved vector in vector table
//
.macro ISR_RESERVED
        .section .vectors, "ax"
        .word 0
.endm

//
// Place a reserved vector in vector table
//
.macro ISR_RESERVED_DUMMY
        .section .vectors, "ax"
        .word Dummy_Handler
.endm

/*********************************************************************
*
*       Externals
*
**********************************************************************
*/
        .extern __stack_end__
        .extern Reset_Handler
        .extern HardFault_Handler

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*  Setup of the vector table and weak definition of interrupt handlers
*
*/
        .section .vectors, "ax"
        .code 16
        .balign 512
        .global _vectors
_vectors:
        //
        // Internal exceptions and interrupts
        //
        VECTOR __stack_end__
        VECTOR Reset_Handler
        EXC_HANDLER NMI_Handler
        VECTOR HardFault_Handler
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        EXC_HANDLER SVC_Handler
        ISR_RESERVED
        ISR_RESERVED
        EXC_HANDLER PendSV_Handler
        EXC_HANDLER SysTick_Handler
        //
        // External interrupts
        //
#ifndef __NO_EXTERNAL_INTERRUPTS
        ISR_HANDLER WWDG1_IRQHandler
        ISR_HANDLER PVD_PVM_IRQHandler
        ISR_HANDLER RTC_TAMP_STAMP_CSS_LSE_IRQHandler
        ISR_HANDLER RTC_WKUP_IRQHandler
        ISR_HANDLER FLASH_IRQHandler
        ISR_HANDLER RCC_IRQHandler
        ISR_HANDLER EXTI0_IRQHandler
        ISR_HANDLER EXTI1_IRQHandler
        ISR_HANDLER EXTI2_IRQHandler
        ISR_HANDLER EXTI3_IRQHandler
        ISR_HANDLER EXTI4_IRQHandler
//        ISR_HANDLER DMA_STR0_IRQHandler DMA1_Stream0_IRQHandler
        ISR_HANDLER DMA1_Stream0_IRQHandler
        ISR_HANDLER DMA_STR1_IRQHandler
        ISR_HANDLER DMA_STR2_IRQHandler
        ISR_HANDLER DMA_STR3_IRQHandler
        ISR_HANDLER DMA_STR4_IRQHandler
        ISR_HANDLER DMA_STR5_IRQHandler
        ISR_HANDLER DMA_STR6_IRQHandler
        ISR_HANDLER ADC1_2_IRQHandler
        ISR_HANDLER FDCAN1_IT0_IRQHandler
        ISR_HANDLER FDCAN2_IT0_IRQHandler
        ISR_HANDLER FDCAN1_IT1_IRQHandler
        ISR_HANDLER FDCAN2_IT1_IRQHandler
        ISR_HANDLER EXTI9_5_IRQHandler
        ISR_HANDLER TIM1_BRK_IRQHandler
        ISR_HANDLER TIM1_UP_IRQHandler
        ISR_HANDLER TIM1_TRG_COM_IRQHandler
        ISR_HANDLER TIM_CC_IRQHandler
        ISR_HANDLER TIM2_IRQHandler
        ISR_HANDLER TIM3_IRQHandler
        ISR_HANDLER TIM4_IRQHandler
        ISR_HANDLER I2C1_EV_IRQHandler
        ISR_HANDLER I2C1_ER_IRQHandler
        ISR_HANDLER I2C2_EV_IRQHandler
        ISR_HANDLER I2C2_ER_IRQHandler
        ISR_HANDLER SPI1_IRQHandler
        ISR_HANDLER SPI2_IRQHandler
        ISR_HANDLER USART1_IRQHandler
        ISR_HANDLER USART2_IRQHandler
        ISR_HANDLER USART3_IRQHandler
        ISR_HANDLER EXTI15_10_IRQHandler
        ISR_HANDLER RTC_ALARM_IRQHandler
        ISR_RESERVED_DUMMY
        ISR_HANDLER TIM8_BRK_TIM12_IRQHandler
        ISR_HANDLER TIM8_UP_TIM13_IRQHandler
        ISR_HANDLER TIM8_TRG_COM_TIM14_IRQHandler
        ISR_HANDLER TIM8_CC_IRQHandler
        ISR_HANDLER DMA1_STR7_IRQHandler
        ISR_HANDLER FMC_IRQHandler
        ISR_HANDLER SDMMC1_IRQHandler
        ISR_HANDLER TIM5_IRQHandler
        ISR_HANDLER SPI3_IRQHandler
        ISR_HANDLER UART4_IRQHandler
        ISR_HANDLER UART5_IRQHandler
        ISR_HANDLER TIM6_DAC_IRQHandler
        ISR_HANDLER TIM7_IRQHandler
        ISR_HANDLER DMA2_STR0_IRQHandler
        ISR_HANDLER DMA2_STR1_IRQHandler
        ISR_HANDLER DMA2_STR2_IRQHandler
        ISR_HANDLER DMA2_STR3_IRQHandler
        ISR_HANDLER DMA2_STR4_IRQHandler
        ISR_HANDLER ETH_IRQHandler
        ISR_HANDLER ETH_WKUP_IRQHandler
        ISR_HANDLER FDCAN_CAL_IRQHandler
        ISR_RESERVED_DUMMY
        ISR_RESERVED_DUMMY
        ISR_RESERVED_DUMMY
        ISR_RESERVED_DUMMY
        ISR_HANDLER DMA2_STR5_IRQHandler
        ISR_HANDLER DMA2_STR6_IRQHandler
        ISR_HANDLER DMA2_STR7_IRQHandler
        ISR_HANDLER USART6_IRQHandler
        ISR_HANDLER I2C3_EV_IRQHandler
        ISR_HANDLER I2C3_ER_IRQHandler
        ISR_HANDLER OTG_HS_EP1_OUT_IRQHandler
        ISR_HANDLER OTG_HS_EP1_IN_IRQHandler
        ISR_HANDLER OTG_HS_WKUP_IRQHandler
        ISR_HANDLER OTG_HS_IRQHandler
        ISR_HANDLER DCMI_IRQHandler
        ISR_HANDLER CRYP_IRQHandler
        ISR_HANDLER HASH_RNG_IRQHandler
        ISR_HANDLER FPU_IRQHandler
        ISR_HANDLER UART7_IRQHandler
        ISR_HANDLER UART8_IRQHandler
        ISR_HANDLER SPI4_IRQHandler
        ISR_HANDLER SPI5_IRQHandler
        ISR_HANDLER SPI6_IRQHandler
        ISR_HANDLER SAI1_IRQHandler
        ISR_HANDLER LTDC_IRQHandler
        ISR_HANDLER LTDC_ER_IRQHandler
        ISR_HANDLER DMA2D_IRQHandler
        ISR_HANDLER SAI2_IRQHandler
        ISR_HANDLER QUADSPI_IRQHandler
        ISR_HANDLER LPTIM1_IRQHandler
        ISR_HANDLER CEC_IRQHandler
        ISR_HANDLER I2C4_EV_IRQHandler
        ISR_HANDLER I2C4_ER_IRQHandler
        ISR_HANDLER SPDIF_IRQHandler
        ISR_HANDLER OTG_FS_EP1_OUT_IRQHandler
        ISR_HANDLER OTG_FS_EP1_IN_IRQHandler
        ISR_HANDLER OTG_FS_WKUP_IRQHandler
        ISR_HANDLER OTG_FS_IRQHandler
        ISR_HANDLER DMAMUX1_OV_IRQHandler
        ISR_HANDLER HRTIM1_MST_IRQHandler
        ISR_HANDLER HRTIM1_TIMA_IRQHandler
        ISR_HANDLER HRTIM_TIMB_IRQHandler
        ISR_HANDLER HRTIM1_TIMC_IRQHandler
        ISR_HANDLER HRTIM1_TIMD_IRQHandler
        ISR_HANDLER HRTIM_TIME_IRQHandler
        ISR_HANDLER HRTIM1_FLT_IRQHandler
        ISR_HANDLER DFSDM1_FLT0_IRQHandler
        ISR_HANDLER DFSDM1_FLT1_IRQHandler
        ISR_HANDLER DFSDM1_FLT2_IRQHandler
        ISR_HANDLER DFSDM1_FLT3_IRQHandler
        ISR_HANDLER SAI3_IRQHandler
        ISR_HANDLER SWPMI1_IRQHandler
        ISR_HANDLER TIM15_IRQHandler
        ISR_HANDLER TIM16_IRQHandler
        ISR_HANDLER TIM17_IRQHandler
        ISR_HANDLER MDIOS_WKUP_IRQHandler
        ISR_HANDLER MDIOS_IRQHandler
        ISR_HANDLER JPEG_IRQHandler
        ISR_HANDLER MDMA_IRQHandler
        ISR_RESERVED_DUMMY
        ISR_HANDLER SDMMC_IRQHandler
        ISR_HANDLER HSEM0_IRQHandler
        ISR_RESERVED_DUMMY
        ISR_HANDLER ADC3_IRQHandler
        ISR_HANDLER DMAMUX2_OVR_IRQHandler
        ISR_HANDLER BDMA_CH1_IRQHandler
        ISR_HANDLER BDMA_CH2_IRQHandler
        ISR_HANDLER BDMA_CH3_IRQHandler
        ISR_HANDLER BDMA_CH4_IRQHandler
        ISR_HANDLER BDMA_CH5_IRQHandler
        ISR_HANDLER BDMA_CH6_IRQHandler
        ISR_HANDLER BDMA_CH7_IRQHandler
        ISR_HANDLER BDMA_CH8_IRQHandler
        ISR_HANDLER COMP_IRQHandler
        ISR_HANDLER LPTIM2_IRQHandler
        ISR_HANDLER LPTIM3_IRQHandler
        ISR_HANDLER LPTIM4_IRQHandler
        ISR_HANDLER LPTIM5_IRQHandler
        ISR_HANDLER LPUART_IRQHandler
        ISR_HANDLER WWDG1_RST_IRQHandler
        ISR_HANDLER CRS_IRQHandler
        ISR_HANDLER ECC_IRQHandler
        ISR_HANDLER SAI4_IRQHandler
        ISR_RESERVED_DUMMY
        ISR_RESERVED_DUMMY
        ISR_HANDLER WKUP_IRQHandler
#endif
        //
        .section .vectors, "ax"
_vectors_end:

#ifdef __VECTORS_IN_RAM
        //
        // Reserve space with the size of the vector table
        // in the designated RAM section.
        //
        .section .vectors_ram, "ax"
        .balign 512
        .global _vectors_ram

_vectors_ram:
        .space _vectors_end - _vectors, 0
#endif

/*********************************************************************
*
*  Dummy handler to be used for reserved interrupt vectors
*  and weak implementation of interrupts.
*
*/
        .section .init.Dummy_Handler, "ax"
        .thumb_func
        .weak Dummy_Handler
        .balign 2
Dummy_Handler:
        1: b 1b   // Endless loop


/*************************** End of file ****************************/
