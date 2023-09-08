// Included files
#include "dma.h"
#include "i2s.h"

// Variable definitions
DMA_HandleTypeDef dma_i2s2_rx = {0};
DMA_HandleTypeDef dma_dac_ch1 = {0};
DMA_HandleTypeDef dma_dac_ch2 = {0};

// Function definitions
void Init_DMA_I2S(){
    // Enable DMA controller clock
    __HAL_RCC_DMA1_CLK_ENABLE();

    // Init I2S DMA RX stream
    dma_i2s2_rx.Instance = DMA1_Stream1;
    if(HAL_DMA_DeInit(&dma_i2s2_rx) != HAL_OK){
        Error_Handler();
    }

    dma_i2s2_rx.Init.Request = DMA_REQUEST_SPI2_RX;
    dma_i2s2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    dma_i2s2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_i2s2_rx.Init.MemInc = DMA_MINC_ENABLE;
    dma_i2s2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    dma_i2s2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    dma_i2s2_rx.Init.Mode = DMA_CIRCULAR; //DMA_CIRCULAR
    dma_i2s2_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    dma_i2s2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if(HAL_DMA_Init(&dma_i2s2_rx) != HAL_OK){
        Error_Handler();
    }

    // Enable DMA1
    __HAL_DMA_ENABLE(&dma_i2s2_rx);

    // Init DMA interrupts
    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 3);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

    // Enable DMA interrupts
    __HAL_DMA_ENABLE_IT(&dma_i2s2_rx, DMA_IT_HT);
    __HAL_DMA_ENABLE_IT(&dma_i2s2_rx, DMA_IT_TC);
}

void Init_DMA_DAC_CH1(){
    // Enable DMA controller clock
    __HAL_RCC_DMA1_CLK_ENABLE();

    // Init I2S DMA RX stream
    dma_dac_ch1.Instance = DMA1_Stream2;
    if(HAL_DMA_DeInit(&dma_dac_ch1) != HAL_OK){
        Error_Handler();
    }

    dma_dac_ch1.Init.Request = DMA_REQUEST_DAC1_CH1;
    dma_dac_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    dma_dac_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_dac_ch1.Init.MemInc = DMA_MINC_ENABLE;
    dma_dac_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_dac_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    dma_dac_ch1.Init.Mode = DMA_CIRCULAR; //DMA_CIRCULAR
    dma_dac_ch1.Init.Priority = DMA_PRIORITY_HIGH;
    dma_dac_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if(HAL_DMA_Init(&dma_dac_ch1) != HAL_OK){
        Error_Handler();
    }

    // Init DMA interrupts
    HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 4);
    HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);

    // Enable DMA interrupts
    __HAL_DMA_ENABLE_IT(&dma_dac_ch1, DMA_IT_HT);
    __HAL_DMA_ENABLE_IT(&dma_dac_ch1, DMA_IT_TC);
}

void Init_DMA_DAC_CH2(){
    // Enable DMA controller clock
    __HAL_RCC_DMA1_CLK_ENABLE();

    // Init I2S DMA RX stream
    dma_dac_ch2.Instance = DMA1_Stream3;
    if(HAL_DMA_DeInit(&dma_dac_ch2) != HAL_OK){
        Error_Handler();
    }

    dma_dac_ch2.Init.Request = DMA_REQUEST_DAC1_CH2;
    dma_dac_ch2.Init.Direction = DMA_MEMORY_TO_PERIPH;
    dma_dac_ch2.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_dac_ch2.Init.MemInc = DMA_MINC_ENABLE;
    dma_dac_ch2.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_dac_ch2.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    dma_dac_ch2.Init.Mode = DMA_CIRCULAR;
    dma_dac_ch2.Init.Priority = DMA_PRIORITY_HIGH;
    dma_dac_ch2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if(HAL_DMA_Init(&dma_dac_ch2) != HAL_OK){
        Error_Handler();
    }

    // Init DMA interrupts
    HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 5);
    HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

    // Enable DMA interrupts
    __HAL_DMA_ENABLE_IT(&dma_dac_ch2, DMA_IT_HT);
    __HAL_DMA_ENABLE_IT(&dma_dac_ch2, DMA_IT_TC);
}

void DMA_STR1_IRQHandler(){
    HAL_DMA_IRQHandler(&dma_i2s2_rx);
}

void DMA_STR2_IRQHandler(){
    if(__HAL_DMA_GET_FLAG(&dma_dac_ch1, DMA_FLAG_TEIF1_5)){
        printf("DAC DMA strm1 transfer error!");
    }
    if(__HAL_DMA_GET_FLAG(&dma_dac_ch1, DMA_FLAG_DMEIF1_5)){
        printf("DAC DMA strm1 direct mode error!\n");
    }
    if(__HAL_DMA_GET_FLAG(&dma_dac_ch1, DMA_FLAG_FEIF1_5)){
        printf("DAC DMA strm1 fifo error!\n");
    }

    HAL_DMA_IRQHandler(&dma_dac_ch1);
}

void DMA_STR3_IRQHandler(){
    if(__HAL_DMA_GET_FLAG(&dma_dac_ch2, DMA_FLAG_TEIF2_6)){
        printf("DAC DMA strm2 transfer error!");
    }
    if(__HAL_DMA_GET_FLAG(&dma_dac_ch2, DMA_FLAG_DMEIF2_6)){
        printf("DAC DMA strm2 direct mode error!\n");
    }
    if(__HAL_DMA_GET_FLAG(&dma_dac_ch2, DMA_FLAG_FEIF2_6)){
        printf("DAC DMA strm2 fifo error!\n");
    }

    HAL_DMA_IRQHandler(&dma_dac_ch2);
}