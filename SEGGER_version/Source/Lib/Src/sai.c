// Included files
#include "sai.h"

// Variable definitions
SAI_HandleTypeDef sai1 = {0};
DMA_HandleTypeDef dma1 = {0};

// Function definitions
void Init_SAI(){
#if(0)
    GPIO_InitTypeDef gpio;

    // Enable periphery clock
    __HAL_RCC_SAI1_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    // Setup pins
    //MCLK - PE2
    gpio.Pin = GPIO_PIN_2;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF6_SAI1;
    HAL_GPIO_Init(GPIOE, &gpio);

    //D - PE6
    gpio.Pin = GPIO_PIN_6;
    gpio.Alternate = GPIO_AF2_SAI1;
    HAL_GPIO_Init(GPIOE, &gpio);

    //SCK - PE5
    gpio.Pin = GPIO_PIN_5;
    gpio.Alternate = GPIO_AF6_SAI1;
    HAL_GPIO_Init(GPIOE, &gpio);

    //SD - PC1
    gpio.Pin = GPIO_PIN_1;
    gpio.Alternate = GPIO_AF6_SAI1;
    HAL_GPIO_Init(GPIOC, &gpio);

    // Enable DMA
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
    dma1.Instance = DMA1_Stream0;
    dma1.Init.Request = DMA_REQUEST_SAI1_A;
    dma1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    dma1.Init.PeriphInc = DMA_PINC_DISABLE;
    dma1.Init.MemInc = DMA_MINC_DISABLE;
    dma1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    dma1.Init.Mode = DMA_NORMAL;
    dma1.Init.Priority = DMA_PRIORITY_LOW;
    dma1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    dma1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    dma1.Init.MemBurst = DMA_MBURST_SINGLE;
    dma1.Init.PeriphBurst = DMA_PBURST_SINGLE;
    if(HAL_DMA_Init(&dma1) != HAL_OK)
        Error_Handler();
    __HAL_LINKDMA(&dac, DMA_Handle, dma1);
#endif
}