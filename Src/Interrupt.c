/*******************************************************************************
  @file   Interrupt.c
  @brief  Interrupt Service Routines
*******************************************************************************/
#include "Bios.h"
#include "Interrupt.h"

extern PCD_HandleTypeDef hpcd_USB_FS;

/*******************************************************************************
             Cortex-M4 Processor Interruption and Exception Handlers 
*******************************************************************************/
void HardFault_Handler(void)
{
  while(1) {}
}

void MemManage_Handler(void)
{
  while(1) {}
}

void BusFault_Handler(void)
{
  while(1) {}
}

void UsageFault_Handler(void)
{
  while(1) {}
}

void NMI_Handler(void) {}

void SVC_Handler(void) {}

void DebugMon_Handler(void) {}

void PendSV_Handler(void) {}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

/*******************************************************************************
  STM32L4xx Peripheral Interrupt Handlers
*******************************************************************************/
void TIM2_IRQHandler(void)
{
  CaptureTimerISP();
}

void TIM1_UP_TIM16_IRQHandler(void)
{
  Calbr_CntF_TimerISP();
}

void TSC_IRQHandler(void)
{
  HAL_TSC_IRQHandler(&hTsc);
}

void USB_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
}

//void EXTI15_10_IRQHandler(void)
//{
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
//  HAL_ResumeTick();
//}

/******************************** END OF FILE *********************************/
