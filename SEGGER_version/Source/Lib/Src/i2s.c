// Included files
#include "i2s.h"
#include "dma.h"
#include "dac.h"
#include "systick_timer.h"

// Definitions and macros
#define MEM_BUF_TRANSFER_ATTEMPT_MAX_CNT 0xFFFF

// Variable definitions
I2S_HandleTypeDef i2s2 = {0};
int16_t I2S2_DATA[I2S_BUFFER_SIZE];
uint8_t I2S2_STATUS_REGISTER;

uint32_t DAC_CH1_BUFFER[DAC_BUFFER_SIZE];
//uint32_t DAC_CH2_BUFFER[DAC_BUFFER_SIZE];

// Function definitions
void clear_i2s2_buf(){
    for(uint32_t i = 0; i < I2S_BUFFER_SIZE; i++)
        I2S2_DATA[i] = 0x0000;
}

void clear_dac_buffers(){
    for(uint32_t i = 0; i < DAC_BUFFER_SIZE; i++){
        DAC_CH1_BUFFER[i] = 0x0000;
        //DAC_CH2_BUFFER[i] = 0x0000;
    }
}

void Init_I2S2(){
    RCC_PeriphCLKInitTypeDef i2s_clock_src = {0};

    // Enable and setup SPI/I2S clock
    i2s_clock_src.PeriphClockSelection = RCC_PERIPHCLK_SPI2;
    i2s_clock_src.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
    if(HAL_RCCEx_PeriphCLKConfig(&i2s_clock_src) != HAL_OK){
        Error_Handler();
    }

    __HAL_RCC_SPI2_CLK_ENABLE();

    // Init I2S periphery
    i2s2.Instance = SPI2;
    if(HAL_I2S_DeInit(&i2s2) != HAL_OK){
        Error_Handler();
    }

    i2s2.Init.Mode = I2S_MODE_SLAVE_RX; //I2S_MODE_MASTER_RX
    i2s2.Init.Standard = I2S_STANDARD_MSB;  //I2S_STANDARD_PHILIPS
    i2s2.Init.DataFormat = I2S_DATAFORMAT_16B;
    i2s2.Init.AudioFreq = I2S_AUDIOFREQ_44K;
    i2s2.Init.CPOL = I2S_CPOL_LOW;
    i2s2.Init.FirstBit = I2S_FIRSTBIT_MSB;
    i2s2.Init.MasterKeepIOState = I2S_MASTER_KEEP_IO_STATE_DISABLE;
    i2s2.Init.Data24BitAlignment = I2S_DATA_24BIT_ALIGNMENT_RIGHT;
    i2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
    i2s2.Instance->CFG1 |= SPI_CFG1_RXDMAEN;
    if(HAL_I2S_Init(&i2s2) != HAL_OK){
        Error_Handler();
    }

    // Enable I2S
    __HAL_I2S_ENABLE(&i2s2);

    // Link DMA and I2S
    __HAL_LINKDMA(&i2s2, hdmarx, dma_i2s2_rx);

    // Clear I2S buffer
    clear_i2s2_buf();

    // Clear DAC channel buffers
    clear_dac_buffers();

    // Clear data receiption register
    I2S2_STATUS_REGISTER &= ~(I2S2_HALF1_BUFFER_READY_FLAG | I2S2_HALF2_BUFFER_READY_FLAG);
}

void HAL_I2S_MspInit(I2S_HandleTypeDef *hi2s){
    GPIO_InitTypeDef gpio = {0};

    // Enable port clock
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Setup pins
    //WS, SCK, SDI
    //gpio.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
    gpio.Pin = I2S2_WS_PIN;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(I2S2_WS_PORT, &gpio);

    gpio.Pin = I2S2_CK_PIN;
    HAL_GPIO_Init(I2S2_CK_PORT, &gpio);

    gpio.Pin = I2S2_SDI_PIN;
    HAL_GPIO_Init(I2S2_SDI_PORT, &gpio);

    gpio.Pin = I2S2_SDO_PIN;
    HAL_GPIO_Init(I2S2_SDO_PORT, &gpio);

    // Init DMA stream
    Init_DMA_I2S();
}

HAL_StatusTypeDef i2s2_start_receiption(){
    return HAL_I2S_Receive_DMA(&i2s2, (uint16_t*)&I2S2_DATA, I2S_BUFFER_SIZE);
}

HAL_StatusTypeDef i2s2_pause_receiption(){
    return HAL_I2S_DMAPause(&i2s2);
}

HAL_StatusTypeDef i2s2_resume_receiption(){
    return HAL_I2S_DMAResume(&i2s2);
}

HAL_StatusTypeDef i2s2_transfer_buf_data(){
    uint16_t I2S2_buf_offset;
    uint16_t DAC_buf_offset;

    // For now only channel 1
    if(I2S2_STATUS_REGISTER & I2S2_HALF1_BUFFER_READY_FLAG){
        I2S2_buf_offset = 0x0000;
    }else{
        I2S2_buf_offset = I2S_BUFFER_SIZE / 2;
    }

    // For now only channel 1
    if(DAC_STATUS_REGISTER & DAC_CH1_HALF1_COMPLETE_FLAG){
        DAC_buf_offset = 0x0000;
    }else{
        DAC_buf_offset = DAC_BUFFER_SIZE / 2;
    }

    // Fill DAC channel buffers
    for(uint16_t i = 0; i < I2S_BUFFER_SIZE / 2; i += 2){
        int32_t value1 = I2S2_DATA[i + I2S2_buf_offset] / 0x10;
        //int32_t value2 = I2S2_DATA[i + I2S2_buf_offset + 1] / 0x20;
        //int32_t value = (value1 + value2) / 2;

        //DAC_CH1_BUFFER[i/2 + DAC_buf_offset] = 0x7FF + value;
        DAC_CH1_BUFFER[i/2 + DAC_buf_offset] = 0x7FF + value1;
        if(DAC_CH1_BUFFER[i/2 + DAC_buf_offset] > 0xFFF){
            DAC_CH1_BUFFER[i/2 + DAC_buf_offset] = 0xFFF;
        }
    }

    /*
    if(I2S2_STATUS_REGISTER & I2S2_HALF2_BUFFER_READY_FLAG){
        HAL_DAC_Stop_DMA(&dac, DAC_CHANNEL_1);
        HAL_I2S_DMAStop(&i2s2);

        printf("\n");
        for(int i = 0; i < DAC_BUFFER_SIZE; i++){
            int32_t data1 = I2S2_DATA[i*2] - 0x7FFF;
            int32_t data2 = I2S2_DATA[(i*2) + 1] - 0x7FFF;
            int32_t value1 = data1 / 0x10;
            int32_t value2 = data2 / 0x10;
            int32_t value = (value1 + value2) / 2;

            printf("idx: %d dat1: %d dat2: %d val1: %d val2: %d val: %d res: %d\n", i, data1, data2, value1, value2, value, DAC_CH1_BUFFER[i]);
        }

        while(1);
    }
    */

    return HAL_OK;
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s){
    #if(0)
    if(hi2s->Instance == SPI2){
        // Fill DAC channel buffers
        for(uint16_t i = 0; i < I2S_BUFFER_SIZE / 2; i += 2){
            double perc1 = (double)((double)I2S2_DATA[i] / (double)0xFFFF);
            double perc2 = (double)((double)I2S2_DATA[i + 1] / (double)0xFFFF);
            uint32_t value1 = (uint32_t)((double)perc1 * (double)0xFFF);
            uint32_t value2 = (uint32_t)((double)perc2 * (double)0xFFF);
            uint32_t value = (value1 + value2) / 2;

            DAC_CH1_BUFFER[i/2] = value;
        }

        // Wait for DAC to finish conversion
        while(!(DAC_STATUS_REGISTER & DAC_CH1_READY_FLAG));

        // Stop DAC conversion
        HAL_DAC_Stop_DMA(&dac, DAC_CHANNEL_1);

        // Start new DAC conversion
        if(HAL_DAC_Start_DMA(&dac, DAC_CHANNEL_1, (uint32_t*)&DAC_CH1_BUFFER, DAC_BUFFER_SIZE, DAC_ALIGN_12B_R) != HAL_OK){
            printf("DAC ERR: DMA state = 0x%x\tDMA err code = 0x%x\n", dac.DMA_Handle1->State, dac.DMA_Handle1->ErrorCode);
            Error_Handler();
        }

        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

        // Set I2S half complete flag
        I2S2_STATUS_REGISTER |= I2S2_HALF1_BUFFER_READY_FLAG;
    }
    #else
    if(hi2s->Instance == SPI2){
        // Clear I2S2 buffer half 2 reception flag
        I2S2_STATUS_REGISTER &= ~(I2S2_HALF2_BUFFER_READY_FLAG);

        // Set I2S2 buffer half 1 reception flag
        I2S2_STATUS_REGISTER |= I2S2_HALF1_BUFFER_READY_FLAG;

        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);

        // Transfer I2S2 buffer data to DAC buffer/s
        i2s2_transfer_buf_data();
    }
    #endif
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s){
#if(0)
    if(hi2s->Instance == SPI2){
        if(I2S2_STATUS_REGISTER & I2S2_HALF1_BUFFER_READY_FLAG){
            // Wait for I2S tranfer to pause
            while(1){
                if(i2s2_pause_receiption() != HAL_OK){
                    printf("I2S ERR: cannot pause DMA transfering\n");
                    while(!wait_ms_ch(2, 1));
                }else{
                    break;
                }
            }

            // Clear buffer ready flags from I2S register
            I2S2_STATUS_REGISTER &= ~(I2S2_HALF1_BUFFER_READY_FLAG | I2S2_HALF2_BUFFER_READY_FLAG);

            // Fill DAC channel buffers
            for(uint16_t i = 0; i < I2S_BUFFER_SIZE; i += 2){
                double perc1 = (double)((double)I2S2_DATA[i] / (double)0xFFFF);
                double perc2 = (double)((double)I2S2_DATA[i + 1] / (double)0xFFFF);
                uint32_t value1 = (uint32_t)((double)perc1 * (double)0xFFF);
                uint32_t value2 = (uint32_t)((double)perc2 * (double)0xFFF);
                uint32_t value = (value1 + value2) / 2;

                DAC_CH1_BUFFER[i/2] = value;
            }

            while(!(DAC_STATUS_REGISTER & DAC_CH1_READY_FLAG));
            HAL_DAC_Stop_DMA(&dac, DAC_CHANNEL_1);
            if(HAL_DAC_Start_DMA(&dac, DAC_CHANNEL_1, (uint32_t*)&DAC_CH1_BUFFER, DAC_BUFFER_SIZE, DAC_ALIGN_12B_R) != HAL_OK){
                printf("DAC ERR: DMA state = 0x%x\tDMA err code = 0x%x\n", dac.DMA_Handle1->State, dac.DMA_Handle1->ErrorCode);
                Error_Handler();
            }
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

            // Wait for I2S tranfer to resume
            while(1){
                if(i2s2_resume_receiption() != HAL_OK){
                    printf("I2S ERR: cannot resume DMA transfering\n");
                    while(!wait_ms_ch(2, 1));
                }else{
                    break;
                }
            }
        }
    }
#else
    if(hi2s->Instance == SPI2){
        if(I2S2_STATUS_REGISTER & I2S2_HALF1_BUFFER_READY_FLAG){
            // Clear I2S2 buffer half 1 reception flag
            I2S2_STATUS_REGISTER &= ~(I2S2_HALF1_BUFFER_READY_FLAG);

            // Set I2S2 buffer half 2 reception flag
            I2S2_STATUS_REGISTER |= I2S2_HALF2_BUFFER_READY_FLAG;

            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);

            // Transfer I2S2 buffer data to DAC buffer/s
            i2s2_transfer_buf_data();
        }
    }
#endif
}