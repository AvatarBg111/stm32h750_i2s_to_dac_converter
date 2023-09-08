// Included files
#include "tim.h"
#include "dac.h"

TIM_HandleTypeDef tim6;

uint8_t pin_state_tim6 = 0;

// Function definitions
void Init_TIM6(){
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    __HAL_RCC_TIM6_CLK_ENABLE();

    tim6.Instance = TIM6;
    if(HAL_TIM_Base_DeInit(&tim6) != HAL_OK){
        Error_Handler();
    }

    tim6.Init.Prescaler = 0;
    tim6.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim6.Init.Period = 1455;  //1455
    tim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim6.Init.RepetitionCounter = 0;
    tim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if(HAL_TIM_Base_Init(&tim6) != HAL_OK){
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if(HAL_TIMEx_MasterConfigSynchronization(&tim6, &sMasterConfig) != HAL_OK){
        Error_Handler();
    }

    if(HAL_TIM_Base_Start(&tim6) != HAL_OK){
        Error_Handler();
    }
}

// Unused
void TIM6_DAC_IRQHandler(){
    if(__HAL_TIM_GET_FLAG(&tim6, TIM_FLAG_UPDATE)){
        HAL_TIM_IRQHandler(&tim6);
    }else if(__HAL_DAC_GET_FLAG(&dac, DAC_FLAG_DMAUDR1) || __HAL_DAC_GET_FLAG(&dac, DAC_FLAG_DMAUDR2)){
        HAL_DAC_IRQHandler(&dac);
    }else{
        Error_Handler();
    }
}