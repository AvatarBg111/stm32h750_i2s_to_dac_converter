// Included files
#include "dac.h"
#include "dma.h"

// Variable definitions
DAC_HandleTypeDef dac = {0};
uint32_t DAC_CH1_DATA[DAC_BUFFER_SIZE];
uint32_t DAC_CH2_DATA[DAC_BUFFER_SIZE];
uint8_t DAC_STATUS_REGISTER;

// Function definitions
void clear_dac_ch1_data_buf(){
    for(uint16_t i = 0; i < DAC_BUFFER_SIZE; i++)
        DAC_CH1_DATA[i] = (uint32_t)0x0000;
}

void clear_dac_ch2_data_buf(){
    for(uint16_t i = 0; i < DAC_BUFFER_SIZE; i++)
        DAC_CH1_DATA[i] = (uint32_t)0x0000;
}

void Init_DAC_dual_channel(){
    DAC_ChannelConfTypeDef dac_conf;

    // Enable DAC clock
    __HAL_RCC_DAC12_CLK_ENABLE();

    // Enable and configure DAC periphery
    dac.Instance = DAC1;
    if(HAL_DAC_DeInit(&dac) != HAL_OK){
        Error_Handler();
    }

    dac.DMA_Handle1 = &dma_dac_ch1;
    dac.DMA_Handle2 = &dma_dac_ch2;
    if(HAL_DAC_Init(&dac) != HAL_OK){
        Error_Handler();
    }

    // Configure DAC channels
    dac_conf.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
    dac_conf.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
    dac_conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    dac_conf.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_EXTERNAL;
    dac_conf.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
    if(HAL_DAC_ConfigChannel(&dac, &dac_conf, DAC_CHANNEL_1) != HAL_OK){
        Error_Handler();
    }
    if(HAL_DAC_ConfigChannel(&dac, &dac_conf, DAC_CHANNEL_2) != HAL_OK){
        Error_Handler();
    }

    clear_dac_ch1_data_buf();
    clear_dac_ch2_data_buf();
    DAC_STATUS_REGISTER &= ~(DAC_CH1_HALF1_COMPLETE_FLAG | DAC_CH1_HALF2_COMPLETE_FLAG | \
                            DAC_CH2_HALF1_COMPLETE_FLAG | DAC_CH2_HALF2_COMPLETE_FLAG);
}

void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac){
    GPIO_InitTypeDef dac_pin;

    // Enable pin port
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Setup pin for DAC channel 1
    dac_pin.Pin = DAC_CH1_PIN;
    dac_pin.Mode = GPIO_MODE_ANALOG;
    dac_pin.Pull = GPIO_NOPULL;
    dac_pin.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(DAC_CH1_PORT, &dac_pin);

    // Setup pin for DAC channel 2
    dac_pin.Pin = DAC_CH2_PIN;
    HAL_GPIO_Init(DAC_CH2_PORT, &dac_pin);

    // Init DMA streams
    Init_DMA_DAC_CH1();
    Init_DMA_DAC_CH2();
}

#if(0)
HAL_StatusTypeDef dac_dual_ch_start_conv(){
    // Check if DAC channels are already running
    if(!(DAC_STATUS_REGISTER & DAC_CH1_READY_FLAG)){
        return HAL_BUSY;
    }else if(!(DAC_STATUS_REGISTER & DAC_CH2_READY_FLAG)){
        return HAL_BUSY;
    }

    // Startup DAC channel 1 conversion
    if(HAL_DAC_Start_DMA(&dac, DAC_CHANNEL_1, (uint32_t*)&DAC_CH1_DATA, DAC_BUFFER_SIZE, DAC_ALIGN_12B_R) != HAL_OK){
        return HAL_ERROR;
    }

    // Startup DAC channel 2 conversion
    if(HAL_DAC_Start_DMA(&dac, DAC_CHANNEL_2, (uint32_t*)&DAC_CH2_DATA, DAC_BUFFER_SIZE, DAC_ALIGN_12B_R) != HAL_OK){
        HAL_DAC_Stop_DMA(&dac, DAC_CHANNEL_1);
        return HAL_ERROR;
    }

    // Clear DAC channels ready flags
    DAC_STATUS_REGISTER &= ~(DAC_CH1_READY_FLAG | DAC_CH2_READY_FLAG);

    return HAL_OK;
}
#endif

HAL_StatusTypeDef dac_ch1_start_conversion_buf(uint32_t *buf){
    // Check if DAC channels are already running
    if((DAC_STATUS_REGISTER & DAC_CH1_HALF1_COMPLETE_FLAG) || (DAC_STATUS_REGISTER & DAC_CH1_HALF2_COMPLETE_FLAG)){
        return HAL_BUSY;
    }

    // Startup DAC channel 1 conversion
    //HAL_DAC_Stop_DMA(&dac, DAC_CHANNEL_1);
    if(HAL_DAC_Start_DMA(&dac, DAC_CHANNEL_1, buf, DAC_BUFFER_SIZE, DAC_ALIGN_12B_R) != HAL_OK){
        return HAL_ERROR;
    }

    // Clear DAC channel buffers half 1 complete conversion flag
    DAC_STATUS_REGISTER &= ~(DAC_CH1_HALF1_COMPLETE_FLAG | DAC_CH2_HALF1_COMPLETE_FLAG);

    // Clear DAC channel buffers half 2 complete conversion flag
    DAC_STATUS_REGISTER &= ~(DAC_CH1_HALF2_COMPLETE_FLAG | DAC_CH2_HALF2_COMPLETE_FLAG);

    return HAL_OK;
}

void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac){
    // Clear DAC channel 1 conversion half 2 ready flag
    DAC_STATUS_REGISTER &= ~(DAC_CH1_HALF2_COMPLETE_FLAG);

    // Set DAC channel 1 conversion half 1 ready flag
    DAC_STATUS_REGISTER |= DAC_CH1_HALF1_COMPLETE_FLAG;

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac){
    if(DAC_STATUS_REGISTER & DAC_CH1_HALF1_COMPLETE_FLAG){
        // Clear DAC channel 1 conversion half 1 ready flag
        DAC_STATUS_REGISTER &= ~(DAC_CH1_HALF1_COMPLETE_FLAG);

        // Set DAC channel 1 conversion half 2 ready flag
        DAC_STATUS_REGISTER |= DAC_CH1_HALF2_COMPLETE_FLAG;

        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
    }
}

void HAL_DAC_ConvCpltCallbackCh2(DAC_HandleTypeDef *hdac){
    // Set DAC channel 2 ready flag
    //DAC_STATUS_REGISTER = DAC_STATUS_REGISTER | DAC_CH2_READY_FLAG;
}