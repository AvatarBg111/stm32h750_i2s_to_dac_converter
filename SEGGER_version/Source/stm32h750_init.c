/** INCLUDES **/
#include "stm32h750_init.h"
#include "stm32h7xx.h"
#include <stdint.h>
#include <stdio.h>

/** DEFINES **/
#if(0)
#define PWM_TIM1
#define PWM_TIM2
#endif

/** VARIABLES **/
DAC_HandleTypeDef dac1 = {0};
ADC_HandleTypeDef adc1 = {0};
ADC_HandleTypeDef adc2 = {0};
ADC_HandleTypeDef adc12 = {0};
TIM_HandleTypeDef tim1 = {0};
TIM_HandleTypeDef tim2 = {0};
TIM_HandleTypeDef tim3 = {0};
TIM_HandleTypeDef tim4 = {0};
TIM_HandleTypeDef tim8 = {0};
TIM_OC_InitTypeDef PWM_Config = {0};
TIM_OC_InitTypeDef PWM_Config2 = {0};
UART_HandleTypeDef uart5 = {0};
SPI_HandleTypeDef spi1 = {0};
DMA_HandleTypeDef dma_adc1 = {0};
DMA_HandleTypeDef dma_adc2 = {0};
DMA_HandleTypeDef DMA_InitStructure = {0};
RTC_HandleTypeDef hrtc = {0};

uint8_t uart5_tx_buf[] = "STM32H750VBT6\r\n";
uint8_t uart5_rx_buf[200];
uint8_t uart5_rx_cnt = 0;

/** FUNCTIONS **/
void SystemClock_Config(){
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY));

  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSI);
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK){
    Error_Handler();
  }
}

void Init_DAC12() {
  DAC_ChannelConfTypeDef dac_conf;
  GPIO_InitTypeDef dac_pin;

  __HAL_RCC_GPIOA_CLK_ENABLE();
  dac_pin.Pin = GPIO_PIN_4;
  dac_pin.Mode = GPIO_MODE_ANALOG;
  dac_pin.Pull = GPIO_NOPULL;
  dac_pin.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &dac_pin);

  __HAL_RCC_DAC12_CLK_ENABLE();
  dac1.Instance = DAC1;
  HAL_DAC_Init(&dac1);

  dac_conf.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  dac_conf.DAC_Trigger = DAC_TRIGGER_NONE;
  dac_conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
  dac_conf.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_EXTERNAL;
  dac_conf.DAC_UserTrimming = DAC_TRIMMING_USER;
  dac_conf.DAC_TrimmingValue = 0x01;
  HAL_DAC_ConfigChannel(&dac1, &dac_conf, DAC_CHANNEL_1);

  __HAL_DAC_ENABLE(&dac1, DAC_CHANNEL_1);
  HAL_DAC_Start(&dac1, DAC_CHANNEL_1);
  HAL_DAC_SetValue(&dac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 225);
}

void Init_ADC1() {
  //ADC_MultiModeTypeDef multimode;
  ADC_ChannelConfTypeDef adc_ch_conf;
  GPIO_InitTypeDef adc_pin;

  /** ADC1 PIN CHANNEL 5**/
  adc_pin.Pin = GPIO_PIN_1;
  adc_pin.Mode = GPIO_MODE_ANALOG;
  adc_pin.Pull = GPIO_NOPULL;
  adc_pin.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &adc_pin);

  adc1.Instance = ADC1;
  adc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1; /** DON'T MAKE STRUCT FOR ARGUMENT IN STRUCT, JUST USE THE INITIAL STRUCT **/
  adc1.Init.Resolution = ADC_RESOLUTION_10B;           /**   THERE IS NO TRIGGER EDGE WHEN SOFTWARE START IS CHOSEN FOR TRIGGER  **/
  adc1.Init.ScanConvMode = ADC_SCAN_DISABLE;           /**    DON'T MAKE THE STUPID MISTAKE TO PUT "ENABLE" ON OVERSAMPLING MODE **/
  adc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;        /**  YOU CAN ADD MULTIMODE CONFIGURATION, BUT YOU CAN GO WITHOUT IT TOO   **/
  adc1.Init.LowPowerAutoWait = DISABLE;                /**       YOU DON'T NEED TO CONFIGURE RIGHT SHIFT, YOU DON'T NEED IT      **/
  adc1.Init.ContinuousConvMode = ENABLE;               /**  FOR THE FUTURE, DON'T FORGET TO CHECK WHAT STATUS FUNCTIONS RETURN   **/
  adc1.Init.DiscontinuousConvMode = DISABLE;
  adc1.Init.NbrOfConversion = 0x1;
  adc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  adc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  adc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  adc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  adc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  adc1.Init.OversamplingMode = DISABLE;

  if (HAL_ADC_Init(&adc1) != HAL_OK)
    Error_Handler();
  /*multimode.Mode = ADC_DUALMODE_REGSIMULT;
    multimode.DualModeData = ADC_DUALMODEDATAFORMAT_32_10_BITS;
    multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_9CYCLES;
    if(HAL_ADCEx_MultiModeConfigChannel(&adc1, &multimode) != HAL_OK)    Error_Handler();*/

  adc_ch_conf.Channel = ADC_CHANNEL_5;
  adc_ch_conf.Rank = ADC_REGULAR_RANK_1;
  adc_ch_conf.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  adc_ch_conf.SingleDiff = ADC_SINGLE_ENDED;
  adc_ch_conf.OffsetNumber = ADC_OFFSET_NONE;
  adc_ch_conf.Offset = 0x0;
  adc_ch_conf.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&adc1, &adc_ch_conf) != HAL_OK)
    Error_Handler();

  //    adc1.Instance = ADC1;
  //    adc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;    /** DON'T MAKE STRUCT FOR ARGUMENT IN STRUCT, JUST USE THE INITIAL STRUCT **/
  //    adc1.Init.Resolution = ADC_RESOLUTION_16B;              /**   THERE IS NO TRIGGER EDGE WHEN SOFTWARE START IS CHOSEN FOR TRIGGER  **/
  //    adc1.Init.ScanConvMode = ADC_SCAN_DISABLE;              /**    DON'T MAKE THE STUPID MISTAKE TO PUT "ENABLE" ON OVERSAMPLING MODE **/
  //    adc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;           /**  YOU CAN ADD MULTIMODE CONFIGURATION, BUT YOU CAN GO WITHOUT IT TOO   **/
  //    adc1.Init.LowPowerAutoWait = DISABLE;                   /**       YOU DON'T NEED TO CONFIGURE RIGHT SHIFT, YOU DON'T NEED IT      **/
  //    adc1.Init.ContinuousConvMode = ENABLE;                  /**  FOR THE FUTURE, DON'T FORGET TO CHECK WHAT STATUS FUNCTIONS RETURN   **/
  //    adc1.Init.DiscontinuousConvMode = DISABLE;
  //    adc1.Init.NbrOfConversion = 0x1;
  //    adc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  //    adc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  //    adc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  //    adc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  //    adc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  //    adc1.Init.OversamplingMode = DISABLE;

  /*    adc_ch_conf.Channel = ADC_CHANNEL_5;
    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    adc_ch_conf.SingleDiff = ADC_SINGLE_ENDED;
    adc_ch_conf.OffsetNumber = ADC_OFFSET_NONE;
    adc_ch_conf.Offset = 0x0;
    adc_ch_conf.OffsetSignedSaturation = DISABLE;
    if(HAL_ADC_ConfigChannel(&adc1, &adc_ch_conf) != HAL_OK)    Error_Handler();

    if(HAL_ADCEx_Calibration_Start(&adc1, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED) != HAL_OK)    Error_Handler();
    HAL_NVIC_SetPriority(ADC_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);*/
}

void Init_ADC2() {
  ADC_ChannelConfTypeDef adc_ch_conf;
  GPIO_InitTypeDef adc_pin;

  /** ADC2 PIN CHANNEL 7**/
  adc_pin.Pin = GPIO_PIN_7;
  adc_pin.Mode = GPIO_MODE_ANALOG;
  adc_pin.Pull = GPIO_NOPULL;
  adc_pin.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &adc_pin);

  //__HAL_RCC_ADC12_CLK_ENABLE();
  adc2.Instance = ADC2;
  adc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1; /** DON'T MAKE STRUCT FOR ARGUMENT IN STRUCT, JUST USE THE INITIAL STRUCT **/
  adc2.Init.Resolution = ADC_RESOLUTION_10B;           /**   THERE IS NO TRIGGER EDGE WHEN SOFTWARE START IS CHOSEN FOR TRIGGER  **/
  adc2.Init.ScanConvMode = ADC_SCAN_DISABLE;           /**    DON'T MAKE THE STUPID MISTAKE TO PUT "ENABLE" ON OVERSAMPLING MODE **/
  adc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;        /**  YOU CAN ADD MULTIMODE CONFIGURATION, BUT YOU CAN GO WITHOUT IT TOO   **/
  adc2.Init.LowPowerAutoWait = DISABLE;                /**       YOU DON'T NEED TO CONFIGURE RIGHT SHIFT, YOU DON'T NEED IT      **/
  adc2.Init.ContinuousConvMode = ENABLE;               /**  FOR THE FUTURE, DON'T FORGET TO CHECK WHAT STATUS FUNCTIONS RETURN   **/
  adc2.Init.DiscontinuousConvMode = DISABLE;
  adc2.Init.NbrOfConversion = 0x1;
  adc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  adc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  adc2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  adc2.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  adc2.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  adc2.Init.OversamplingMode = DISABLE;

  adc_ch_conf.Channel = ADC_CHANNEL_7;
  adc_ch_conf.Rank = ADC_REGULAR_RANK_1;
  adc_ch_conf.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  adc_ch_conf.SingleDiff = ADC_SINGLE_ENDED;
  adc_ch_conf.OffsetNumber = ADC_OFFSET_NONE;
  adc_ch_conf.Offset = 0x0;
  adc_ch_conf.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&adc2, &adc_ch_conf) != HAL_OK)
    Error_Handler();
}

void Init_ADC12() {
  ADC_MultiModeTypeDef multimode;
  ADC_ChannelConfTypeDef adc_ch_conf;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  GPIO_InitTypeDef adc_pin;

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInitStruct.PLL2.PLL2M = 4;
  PeriphClkInitStruct.PLL2.PLL2N = 9;
  PeriphClkInitStruct.PLL2.PLL2P = 4;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 3072;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    Error_Handler();

  /** ADC1/2 PIN **/
  adc_pin.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  adc_pin.Mode = GPIO_MODE_ANALOG;
  adc_pin.Pull = GPIO_PULLUP;
  adc_pin.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &adc_pin);

  __HAL_RCC_ADC12_CLK_ENABLE();
  adc12.Instance = ADC1;
  adc12.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1; /** DON'T MAKE STRUCT FOR ARGUMENT IN STRUCT, JUST USE THE INITIAL STRUCT **/
  adc12.Init.Resolution = ADC_RESOLUTION_16B;       /**   THERE IS NO TRIGGER EDGE WHEN SOFTWARE START IS CHOSEN FOR TRIGGER  **/
  adc12.Init.ScanConvMode = ADC_SCAN_DISABLE;       /**    DON'T MAKE THE STUPID MISTAKE TO PUT "ENABLE" ON OVERSAMPLING MODE **/
  adc12.Init.EOCSelection = ADC_EOC_SINGLE_CONV;    /**  YOU CAN ADD MULTIMODE CONFIGURATION, BUT YOU CAN GO WITHOUT IT TOO   **/
  adc12.Init.LowPowerAutoWait = DISABLE;            /**       YOU DON'T NEED TO CONFIGURE RIGHT SHIFT, YOU DON'T NEED IT      **/
  adc12.Init.ContinuousConvMode = DISABLE;          /**  FOR THE FUTURE, DON'T FORGET TO CHECK WHAT STATUS FUNCTIONS RETURN   **/
  adc12.Init.NbrOfConversion = 0x1;
  adc12.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T3_TRGO;
  adc12.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  adc12.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_ONESHOT;
  adc12.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  adc12.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  adc12.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&adc12) != HAL_OK)
    Error_Handler();
  adc12.Instance = ADC2;
  if (HAL_ADC_Init(&adc12) != HAL_OK)
    Error_Handler();
  adc12.Instance = ADC1;

  adc_ch_conf.Channel = ADC_CHANNEL_5;
  adc_ch_conf.Rank = ADC_REGULAR_RANK_1;
  adc_ch_conf.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  adc_ch_conf.SingleDiff = ADC_SINGLE_ENDED;
  adc_ch_conf.OffsetNumber = ADC_OFFSET_NONE;
  adc_ch_conf.Offset = 0x0;
  adc_ch_conf.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&adc12, &adc_ch_conf) != HAL_OK)
    Error_Handler();

  adc12.Instance = ADC2;
  adc_ch_conf.Channel = ADC_CHANNEL_9;
  if (HAL_ADC_ConfigChannel(&adc12, &adc_ch_conf) != HAL_OK)
    Error_Handler();

  adc12.Instance = ADC1;
  multimode.Mode = ADC_DUALMODE_INTERL;
  //multimode.Mode = ADC_DUALMODE_REGSIMULT;
  multimode.DualModeData = ADC_DUALMODEDATAFORMAT_32_10_BITS;
  multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_1CYCLE;
  if (HAL_ADCEx_MultiModeConfigChannel(&adc12, &multimode) != HAL_OK)
    Error_Handler();

#if (0)
  __HAL_RCC_DMA1_CLK_ENABLE();
  dma_adc1.Instance = DMA1_Stream0;
  dma_adc1.Init.Request = DMA_REQUEST_ADC1;
  dma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
  dma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
  dma_adc1.Init.MemInc = DMA_MINC_ENABLE;
  dma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  dma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  dma_adc1.Init.Mode = DMA_NORMAL;
  dma_adc1.Init.Priority = DMA_PRIORITY_LOW;
  dma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  dma_adc1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
  dma_adc1.Init.MemBurst = DMA_MBURST_SINGLE;
  dma_adc1.Init.PeriphBurst = DMA_PBURST_SINGLE;
  if (HAL_DMA_Init(&dma_adc1) != HAL_OK)
    Error_Handler();
  __HAL_LINKDMA(&adc12, DMA_Handle, dma_adc1);

  adc12.Instance = ADC2;
  dma_adc2.Instance = DMA1_Stream2;
  dma_adc2.Init.Request = DMA_REQUEST_ADC2;
  dma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
  dma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
  dma_adc2.Init.MemInc = DMA_MINC_ENABLE;
  dma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  dma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  dma_adc2.Init.Mode = DMA_NORMAL;
  dma_adc2.Init.Priority = DMA_PRIORITY_LOW;
  dma_adc2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  dma_adc2.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
  dma_adc2.Init.MemBurst = DMA_MBURST_SINGLE;
  dma_adc2.Init.PeriphBurst = DMA_PBURST_SINGLE;
  if (HAL_DMA_Init(&dma_adc2) != HAL_OK)
    Error_Handler();
  __HAL_LINKDMA(&adc12, DMA_Handle, dma_adc2);

  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);

  HAL_NVIC_SetPriority(ADC_IRQn, 1, 2);
  HAL_NVIC_EnableIRQ(ADC_IRQn);
#endif
}

void Init_TIM1() {
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
#ifdef NORMAL_TIM1
  __HAL_RCC_TIM1_CLK_ENABLE();
  tim1.Instance = TIM1;
  tim1.Init.Prescaler = 0;
  tim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim1.Init.Period = 200;
  tim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
  tim1.Init.RepetitionCounter = 0;
  tim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&tim1) != HAL_OK)
    Error_Handler();

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&tim1, &sClockSourceConfig) != HAL_OK)
    Error_Handler();

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&tim1, &sMasterConfig) != HAL_OK)
    Error_Handler();

  HAL_NVIC_SetPriority(TIM1_UP_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
#endif

#ifdef PWM_TIM1
  GPIO_InitTypeDef tim1_pin;

  /** TIM1 PWM PIN **/
  __HAL_RCC_GPIOE_CLK_ENABLE();
  tim1_pin.Pin = GPIO_PIN_14;
  tim1_pin.Mode = GPIO_MODE_AF_PP;
  tim1_pin.Pull = GPIO_PULLUP;
  tim1_pin.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  tim1_pin.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(GPIOE, &tim1_pin);

  __HAL_RCC_TIM1_CLK_ENABLE();
  tim1.Instance = TIM1;
  tim1.Init.Prescaler = 355;
  tim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim1.Init.Period = 3600;
  tim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; /** SYSTEM FREQUENCY / 1 = 64MHz **/
  tim1.Init.RepetitionCounter = 0;
  tim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&tim1) != HAL_OK)
    Error_Handler();

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&tim1, &sClockSourceConfig) != HAL_OK)
    Error_Handler();

  PWM_Config.OCMode = TIM_OCMODE_PWM1;
  PWM_Config.Pulse = 110;
  PWM_Config.OCPolarity = TIM_OCPOLARITY_HIGH;
  PWM_Config.OCFastMode = TIM_OCFAST_DISABLE;
  PWM_Config.OCIdleState = TIM_OCIDLESTATE_RESET;
  PWM_Config.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&tim1, &PWM_Config, TIM_CHANNEL_4) != HAL_OK)
    Error_Handler();

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&tim1, &sMasterConfig) != HAL_OK)
    Error_Handler();
#endif
}

void Init_TIM2() {
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  GPIO_InitTypeDef tim2_pin;

  /** TIM2 PWM PIN **/
  tim2_pin.Pin = GPIO_PIN_11;
  tim2_pin.Mode = GPIO_MODE_AF_PP;
  tim2_pin.Pull = GPIO_PULLUP;
  tim2_pin.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  tim2_pin.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(GPIOB, &tim2_pin);

  __HAL_RCC_TIM2_CLK_ENABLE();
  tim2.Instance = TIM2;
  tim2.Init.Prescaler = 355;
  tim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim2.Init.Period = 3600;
  tim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; /** SYSTEM FREQUENCY / 1 = 64MHz **/
  tim2.Init.RepetitionCounter = 0;
  tim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&tim2) != HAL_OK)
    Error_Handler();

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&tim2, &sClockSourceConfig) != HAL_OK)
    Error_Handler();

  PWM_Config2.OCMode = TIM_OCMODE_PWM1;
  PWM_Config2.Pulse = 110;
  PWM_Config2.OCPolarity = TIM_OCPOLARITY_HIGH;
  PWM_Config2.OCFastMode = TIM_OCFAST_DISABLE;
  PWM_Config2.OCIdleState = TIM_OCIDLESTATE_RESET;
  PWM_Config2.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&tim2, &PWM_Config2, TIM_CHANNEL_4) != HAL_OK)
    Error_Handler();

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&tim2, &sMasterConfig) != HAL_OK)
    Error_Handler();
}

void Init_TIM3() {
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  __HAL_RCC_TIM3_CLK_ENABLE();
  tim3.Instance = TIM3;
  tim3.Init.Prescaler = 0;
  tim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim3.Init.Period = 2000;
  tim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
  tim3.Init.RepetitionCounter = 0;
  tim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&tim3) != HAL_OK)
    Error_Handler();

  if (HAL_TIM_GenerateEvent(&tim3, TIM_EVENTSOURCE_UPDATE) != HAL_OK)
    Error_Handler();

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  if (HAL_TIMEx_MasterConfigSynchronization(&tim3, &sMasterConfig) != HAL_OK)
    Error_Handler();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
  HAL_NVIC_SetPriority(TIM3_IRQn, 1, 3);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

void Init_TIM4() {
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};

  __HAL_RCC_TIM4_CLK_ENABLE();
  tim4.Instance = TIM4;
  tim4.Init.Prescaler = 0;
  tim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim4.Init.Period = 1800;
  tim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
  tim4.Init.RepetitionCounter = 0;
  tim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&tim4) != HAL_OK)
    Error_Handler();

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
  sSlaveConfig.InputTrigger = TIM_TS_ITR2;
  if (HAL_TIM_SlaveConfigSynchro(&tim4, &sSlaveConfig) != HAL_OK)
    Error_Handler();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
  HAL_NVIC_SetPriority(TIM4_IRQn, 2, 2);
  HAL_NVIC_EnableIRQ(TIM4_IRQn);
}

void Init_TIM8() {
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  __HAL_RCC_TIM8_CLK_ENABLE();
  tim8.Instance = TIM8;
  tim8.Init.Prescaler = 0xFFFF;
  tim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim8.Init.Period = 1350;
  tim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim8.Init.RepetitionCounter = 0;
  tim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&tim8) != HAL_OK)
    Error_Handler();

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&tim8, &sClockSourceConfig) != HAL_OK)
    Error_Handler();

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&tim8, &sMasterConfig) != HAL_OK)
    Error_Handler();

  HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 0, 2);
  HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
}

void Init_UART5() {
  GPIO_InitTypeDef uart_pins;
  /** UART5 RX/TX PINS **/
  uart_pins.Pin = GPIO_PIN_12; /** DON'T FORGET TO CHOOSE THE ALTERNATIVE FUNCTION IN THE PIN CONFIGURATION **/
  uart_pins.Mode = GPIO_MODE_AF_PP;
  uart_pins.Pull = GPIO_NOPULL;
  uart_pins.Alternate = GPIO_AF14_UART5;
  HAL_GPIO_Init(GPIOB, &uart_pins);

  uart_pins.Pin = GPIO_PIN_13;
  uart_pins.Mode = GPIO_MODE_AF_PP;
  uart_pins.Pull = GPIO_NOPULL;
  //uart_pins.Alternate = GPIO_AF8_UART5;
  HAL_GPIO_Init(GPIOB, &uart_pins);
  __HAL_RCC_GPIOB_CLK_ENABLE();

  __HAL_RCC_UART5_CLK_ENABLE();
  uart5.Instance = UART5;
  uart5.pTxBuffPtr = (uint8_t *)&uart5_tx_buf[0];
  uart5.pRxBuffPtr = (uint8_t *)&uart5_rx_buf;
  uart5.hdmatx = &DMA_InitStructure;
  uart5.FifoMode = UART_FIFOMODE_DISABLE;
  uart5.Init.BaudRate = 115200;
  uart5.Init.WordLength = UART_WORDLENGTH_8B;
  uart5.Init.StopBits = UART_STOPBITS_1;
  uart5.Init.Parity = UART_PARITY_NONE;
  uart5.Init.Mode = UART_MODE_TX_RX;
  uart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart5.Init.OverSampling = UART_OVERSAMPLING_8;
  uart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  uart5.Init.ClockPrescaler = UART_PRESCALER_DIV16;
  uart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  uart5.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
  uart5.AdvancedInit.DMADisableonRxError = UART_ADVFEATURE_DMA_DISABLEONRXERROR;
  if (HAL_UART_Init(&uart5) != HAL_OK)
    Error_Handler();

  HAL_NVIC_SetPriority(UART5_IRQn, 0, 3);
  HAL_NVIC_EnableIRQ(UART5_IRQn);
  __HAL_UART_ENABLE_IT(&uart5, UART_IT_RXNE);

  /*if(HAL_UARTEx_SetTxFifoThreshold(&uart5, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)    Error_Handler();
    if(HAL_UARTEx_SetRxFifoThreshold(&uart5, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)    Error_Handler();
    if(HAL_UARTEx_DisableFifoMode(&uart5) != HAL_OK)    Error_Handler();*/
}

void Init_UART5_DMA() {
  __HAL_RCC_DMA1_CLK_ENABLE();

  DMA_InitStructure.Instance = DMA1_Stream1;
  if (HAL_DMA_DeInit(&DMA_InitStructure) != HAL_OK)
    Error_Handler();
  DMA_InitStructure.DMAmuxChannel = DMAMUX1_Channel1;
  DMA_InitStructure.Init.Request = DMA_REQUEST_UART5_TX;
  DMA_InitStructure.Init.Direction = DMA_MEMORY_TO_PERIPH;
  DMA_InitStructure.Init.PeriphInc = DMA_PINC_DISABLE;
  DMA_InitStructure.Init.MemInc = DMA_MINC_ENABLE;
  DMA_InitStructure.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  DMA_InitStructure.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  DMA_InitStructure.Init.Mode = DMA_NORMAL;
  DMA_InitStructure.Init.Priority = DMA_PRIORITY_MEDIUM;
  DMA_InitStructure.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  DMA_InitStructure.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
  DMA_InitStructure.Init.MemBurst = DMA_MBURST_SINGLE;
  DMA_InitStructure.Init.PeriphBurst = DMA_PBURST_SINGLE;
  if (HAL_DMA_Init(&DMA_InitStructure) != HAL_OK)
    Error_Handler();

  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 1, 4);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
}

void Init_SPI1() {
  GPIO_InitTypeDef spi_pin;

  /** SPI1 PINS **/
  __HAL_RCC_SPI123_CONFIG(RCC_SPI123CLKSOURCE_PLL);
  __HAL_RCC_GPIOA_CLK_ENABLE();

  spi_pin.Pin = GPIO_PIN_5;
  spi_pin.Mode = GPIO_MODE_AF_PP;
  spi_pin.Pull = GPIO_NOPULL;
  spi_pin.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  spi_pin.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &spi_pin);

  spi_pin.Pin = GPIO_PIN_7;
  spi_pin.Mode = GPIO_MODE_AF_PP;
  spi_pin.Pull = GPIO_NOPULL;
  spi_pin.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &spi_pin);

  spi_pin.Pin = GPIO_PIN_6;
  spi_pin.Mode = GPIO_MODE_OUTPUT_PP;
  spi_pin.Pull = GPIO_PULLUP;
  spi_pin.Alternate = NULL;
  HAL_GPIO_Init(GPIOA, &spi_pin);

  __HAL_RCC_SPI1_CLK_ENABLE();
  spi1.Instance = SPI1;
  spi1.Init.Mode = SPI_MODE_MASTER;
  spi1.Init.Direction = SPI_DIRECTION_2LINES_TXONLY;
  spi1.Init.DataSize = SPI_DATASIZE_8BIT;
  spi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  spi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  spi1.Init.NSS = SPI_NSS_SOFT;
  spi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  spi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  spi1.Init.TIMode = SPI_TIMODE_DISABLE;
  spi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  spi1.Init.CRCPolynomial = 0x0;
  spi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLED;
  spi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  if (HAL_SPI_DeInit(&spi1) != HAL_OK)
    Error_Handler();
  if (HAL_SPI_Init(&spi1) != HAL_OK)
    Error_Handler();
}

void Init_RTC() {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  //__HAL_RCC_PWR_CLK_ENABLE();//Enable power clock PWR
  HAL_PWR_EnableBkUpAccess(); //Cancel the write protection of the backup area

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE; //LSE configuration
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON; //RTC uses LSE
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC; //The external setting is RTC
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE; //RTC clock source is LSE
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

  __HAL_RCC_RTC_ENABLE(); //RTC clock enable

  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
    Error_Handler();
  if (HAL_RTC_WaitForSynchro(&hrtc) != HAL_OK)
    Error_Handler();

  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
    Error_Handler();

  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;
  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
    Error_Handler();
}

void print_frequencies(){
  uint32_t sys_clock = HAL_RCC_GetSysClockFreq();
  uint32_t hclk = HAL_RCC_GetHCLKFreq();
  uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
  uint32_t pclk2 = HAL_RCC_GetPCLK2Freq();

  printf("\n\tsys_clock:%d hclk:%d pclk1:%d pclk2:%d", sys_clock, hclk, pclk1, pclk2);
}