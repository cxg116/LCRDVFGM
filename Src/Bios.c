/*******************************************************************************
  @file           : Bios.c
  @brief          : Base IO Operation Subroutine  of the Application
*******************************************************************************/
#include "Bios.h"
#include "CTRL_Drv.c"
#include "I2C_Drv.c"
#include "Fonts.c"
#include "OLED_Drv.c"
#include "RTC_Drv.c"
#include "COMP_Drv.c"
#include "OPAMP_Drv.c"
#include "DAC_Drv.c"
#include "ADC_Drv.c"
#include "ACCEL_Drv.c"
#include "TIM_Drv.c"
#include "TSC_Drv.c"

/*******************************************************************************
 Function Name  : MSR_MSP    Set Main Stack value.
*******************************************************************************/
void SetMainSP(u32 StackAddr)
{
  asm("MSR  MSP, R0 ");
}
/*******************************************************************************
 Dly_uS 定时误差 < 1%
*******************************************************************************/
void Dly_uS(u32 n)
{ 
  while(n--) DELAYt;
}
/*******************************************************************************
  在 HAL_Init()中被调用
*******************************************************************************/
void HAL_MspInit(void)
{
  PWR_PVDTypeDef sConfigPVD = {0};

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  
  sConfigPVD.PVDLevel = PWR_PVDLEVEL_6;
  sConfigPVD.Mode     = PWR_PVD_MODE_NORMAL;
  HAL_PWR_ConfigPVD(&sConfigPVD);
  HAL_PWR_EnablePVD();
  
}
/*******************************************************************************
  在中断 SysTick_Handler() 中被调用                        会改变公共变量 Item
*******************************************************************************/
u32 Pcnt, Mcnt, Last, CntL, CntH;
u32 Wx[20] = {0};
u16 DispUpd = 1, Item = NC, Mode = USERDEF;

void HAL_IncTick(void) 
{
  if(++Mcnt == 1000) Mcnt = 0;
  if(Mcnt%100 == 0)                        // 100mS 周期
  {
    u32 Hcnt = FH_CNT_VAL;
    u32 Lcnt = CNT_VAL+(CntHigh << 16);
    if(Mode == MEASU_F)                    // 频率测量档
    {
      if(Mcnt == 100) 
      {
        Wx[FX] = 10*(Lcnt-CntL);           // 读取 FX 测频结果 @Hz
        if(Wx[FX] <= 10) Wx[FX] = 0;
        if(CptActv == 0) Wx[FX+6] = 0;     // 无新捕捉则清除捕捉结果
        Item = FH;                         // 设置下个测量项目
        MeasuSlctUpdt(Item);                    // 重新配置测量控制
      }
      if(Mcnt == 300)
      {
        Wx[FH] = 80*(Hcnt-CntH);           // 读取 FH 测频结果 @ Hz
        Item = FX;                         // 设置下个测量项目
        MeasuSlctUpdt(Item);                    // 重新配置测量控制
        Mcnt = 0;
      }
    }
    if(Mode == MEASU_V)                    // 电压测量档
    {
      Wx[Item] = AdcVlue[VADC];
      if(Mcnt == 100) Item = PV;
      if(Mcnt == 200) Item = LV;
      if(Mcnt == 300) 
      {
        Item = NV;
        Mcnt = 0;
      }
      MeasuSlctUpdt(Item);
    }
    if(Mode == MEASU_R || Mode == MEASU_D) // 电阻及二极管测量档
    {
      Wx[Item] = AdcVlue[VADC];
      if(Mcnt == 100) Item = PV;
      if(Mcnt == 200) Item = NV;
      if(Mcnt == 300) Item = RH;
      if(Mcnt == 400) Item = RM;
      if(Mcnt == 500) 
      {
        Item = RL;
        Mcnt = 0;
      }
      MeasuSlctUpdt(Item);
    }
    if(Mode == MEASU_C)                    // 电容测量档
    {
      if(Mcnt <= 300) Wx[Item] = AdcVlue[VADC];
      if(Mcnt == 100) Item = PV;
      if(Mcnt == 200) Item = RH;
      if(Mcnt == 300) Item = CX;
      if(Mcnt == 500 || Mcnt == 700)
      {
        Wx[Item] = 10*(Lcnt-CntL);
        if(Wx[Item] <= 10) Wx[Item] = 0;
        if(CptActv == 0) Wx[Item+6] = 0;   // 无新捕捉则清除捕捉结果
        if(Item == CX) Item = FX;
        else           Item = NV;
      }
      if(Mcnt == 700) Mcnt = 0;
      if(Mcnt != 400 && Mcnt != 600) MeasuSlctUpdt(Item);
    }
    if(Mode == MEASU_L)                    // 电感测量档
    {
      if(Mcnt <= 300) Wx[Item] = AdcVlue[VADC];
      if(Mcnt == 100) Item = PV;
      if(Mcnt == 200) Item = RL;
      if(Mcnt == 300) Item = LX;
      if(Mcnt == 500 || Mcnt == 700)
      {
        Wx[Item] = 10*(Lcnt-CntL);
        if(Wx[Item] <= 10) Wx[Item] = 0;
        if(CptActv == 0) Wx[Item+6] = 0;   // 无新捕捉则清除捕捉结果
        if(Item == LX) Item = LR;
        else           Item = NV;
      }
      if(Mcnt == 700) Mcnt = 0;
      if(Mcnt != 400 && Mcnt != 600) MeasuSlctUpdt(Item);
    }
    CntL = Lcnt;
    CntH = Hcnt;
    CptActv = 0;                           // 清除捕捉有效状态标志
    DispUpd = 1;
  }
  Pcnt++;
  uwTick++;
  CalbrSumT = CalbrSumT-CalbrSumT/128+CalbrFrqn;
}
/*******************************************************************************
  System Clock Configuration
*******************************************************************************/
void SysClkCnfg(u16 Mode)
{
  RCC_OscInitTypeDef       RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef       RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit     = {0};

  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  
  if(Mode == DFU)
  {
    RCC_OscInitStruct.OscillatorType    = RCC_OSCILLATORTYPE_HSI48;
    RCC_OscInitStruct.HSI48State        = RCC_HSI48_ON;
    PeriphClkInit.PeriphClockSelection  = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection     = RCC_USBCLKSOURCE_HSI48;
  }
  
  RCC_OscInitStruct.OscillatorType     |= RCC_OSCILLATORTYPE_LSI   |
//                                          RCC_OSCILLATORTYPE_HSI48 |
                                          RCC_OSCILLATORTYPE_LSE   |
                                          RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState            = RCC_LSE_ON;
//  RCC_OscInitStruct.HSI48State          = RCC_HSI48_OFF;//RCC_HSI48_ON;//
  RCC_OscInitStruct.LSIState            = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState            = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange       = RCC_MSIRANGE_8;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) ErrLoop();
   
  RCC_ClkInitStruct.ClockType           = RCC_CLOCKTYPE_HCLK   |
                                          RCC_CLOCKTYPE_SYSCLK |
                                          RCC_CLOCKTYPE_PCLK1  |
                                          RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource        = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider       = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider      = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider      = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) ErrLoop();
   
  PeriphClkInit.PeriphClockSelection   |= RCC_PERIPHCLK_RTC  |
                                          RCC_PERIPHCLK_I2C1 |
//                                          RCC_PERIPHCLK_USB  |
                                          RCC_PERIPHCLK_ADC;
  PeriphClkInit.I2c1ClockSelection      = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection       = RCC_ADCCLKSOURCE_SYSCLK;
  PeriphClkInit.RTCClockSelection       = RCC_RTCCLKSOURCE_LSE;
//  PeriphClkInit.UsbClockSelection       = RCC_USBCLKSOURCE_HSI48;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) ErrLoop();
   
  if(HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) ErrLoop();
  
  HAL_RCCEx_EnableMSIPLLMode();
}

/*******************************************************************************
  This function is executed in case of error occurrence.
*******************************************************************************/
void ErrLoop(void) {}

/******************************** END OF FILE *********************************/
