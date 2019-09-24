/*******************************************************************************
  Brief: 加载在 BIOS 中的 TIM 驱动程序子模块                    Author : bure
*******************************************************************************/

u32  CptActv, CntHigh = 0;
vu32 LastCpt, LastCnt;

/*******************************************************************************
  
*******************************************************************************/
void CntClkCnfg(u16 Filter)
{
  if(Filter == 0) CNT_TIM->SMCR &= ~0xF00;
  else            CNT_TIM->SMCR |=  0x100;
}
/*******************************************************************************
  计数测频窗口 100mS 频率 > 12.5KHz 误差 < 0.08% 
*******************************************************************************/
void CounterTimerInit(void)
{
  TIM_HandleTypeDef        hTimCnt = {0};
  TIM_ClockConfigTypeDef   sClkSrc = {0};
  
  CNT_TIM_RCC_ENBL();
  hTimCnt.Instance                 = CNT_TIM;
  hTimCnt.Init.Prescaler           = 1-1;
  hTimCnt.Init.Period              = 0xFFFF;
  hTimCnt.Init.CounterMode         = TIM_COUNTERMODE_UP;
  hTimCnt.Init.ClockDivision       = TIM_CLOCKDIVISION_DIV1;
  hTimCnt.Init.RepetitionCounter   = 0;
  hTimCnt.Init.AutoReloadPreload   = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&hTimCnt);
  
  sClkSrc.ClockSource              = TIM_CLOCKSOURCE_ETRMODE2;
  sClkSrc.ClockPolarity            = TIM_CLOCKPOLARITY_NONINVERTED;
  sClkSrc.ClockPrescaler           = TIM_CLOCKPRESCALER_DIV1;
  sClkSrc.ClockFilter              = 0;// 0~15
  HAL_TIM_ConfigClockSource(&hTimCnt, &sClkSrc);
    
  HAL_TIMEx_RemapConfig(&hTimCnt, CNT_REMAP);
  HAL_TIM_Base_Start_IT(&hTimCnt);
}
/*******************************************************************************
  
*******************************************************************************/
void Cpt_Fh_TimerCnfg(u16 Slct)
{
  GPIO_InitTypeDef         sGpio   = {0};
  TIM_HandleTypeDef        hTimCpt = {0};
  TIM_ClockConfigTypeDef   sClkSrc = {0};
  TIM_MasterConfigTypeDef  sMaster = {0};
  TIM_IC_InitTypeDef       sCnfgIC = {0};
  
  FH_IO_RCC_EN();
  sGpio.Pin                        = FH_PIN;
  sGpio.Alternate                  = FH_IO_AF;
  sGpio.Pull                       = GPIO_NOPULL;
  sGpio.Mode                       = GPIO_MODE_AF_PP;
  sGpio.Speed                      = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FH_IO_PORT, &sGpio);
  
  u32 Modes = 1 << (Slct & 0x0F);
  
  CPT_TIM_RCC_ENBL();
  hTimCpt.Instance                 = FH_CPT_TIM;
  hTimCpt.Init.Prescaler           = 1-1;
  hTimCpt.Init.Period              = 0xFFFFFFFF;
  hTimCpt.Init.CounterMode         = TIM_COUNTERMODE_UP;
  hTimCpt.Init.ClockDivision       = TIM_CLOCKDIVISION_DIV1;
  hTimCpt.Init.AutoReloadPreload   = TIM_AUTORELOAD_PRELOAD_DISABLE;
  
  sClkSrc.ClockSource              = TIM_CLOCKSOURCE_ETRMODE2;
  sClkSrc.ClockPolarity            = TIM_CLOCKPOLARITY_NONINVERTED;
  sClkSrc.ClockFilter              = 0;// 0~15
  if(Modes & FHM)
  {
    sClkSrc.ClockSource            = TIM_CLOCKSOURCE_ETRMODE2;
    sClkSrc.ClockPrescaler         = TIM_CLOCKPRESCALER_DIV8;
  }
  else            
  {
    sClkSrc.ClockSource            = TIM_CLOCKSOURCE_INTERNAL;   
    sClkSrc.ClockPrescaler         = TIM_CLOCKPRESCALER_DIV1;
  }
  HAL_TIM_ConfigClockSource(&hTimCpt, &sClkSrc);
  
  sMaster.MasterOutputTrigger      = TIM_TRGO_RESET;
  sMaster.MasterSlaveMode          = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&hTimCpt, &sMaster);
  
  CaptureDsbl();
  HAL_NVIC_DisableIRQ(CAPTURE_IRQ);
  __HAL_TIM_DISABLE(&hTimCpt);
  HAL_TIM_IC_DeInit(&hTimCpt);
  if(Modes & FHM)
  {
    HAL_TIM_Base_Init(&hTimCpt);
    HAL_TIM_Base_Start(&hTimCpt); 
  }
  else if(Modes & CXM+FXM+LXM+LRM)
  {
    sCnfgIC.ICPolarity             = TIM_INPUTCHANNELPOLARITY_RISING;
    sCnfgIC.ICSelection            = TIM_ICSELECTION_DIRECTTI;
    sCnfgIC.ICPrescaler            = TIM_ICPSC_DIV1;
    sCnfgIC.ICFilter               = 15;// 0~15
    HAL_TIM_IC_Init(&hTimCpt);
    HAL_TIM_IC_ConfigChannel(&hTimCpt, &sCnfgIC, CAPTURE_CHx);
    HAL_TIMEx_RemapConfig(&hTimCpt, CAPTURE_REMAP);
    HAL_NVIC_SetPriority(CAPTURE_IRQ, 1, 0);
    HAL_NVIC_EnableIRQ(CAPTURE_IRQ);
  }
}
/*******************************************************************************
  
*******************************************************************************/
void Angl_Trig_TimerCnfg(u16 Slct, u16 Psc, u16 Period)
{
  TIM_HandleTypeDef        hTimCpt = {0};
  TIM_ClockConfigTypeDef   sClkSrc = {0};
  TIM_MasterConfigTypeDef  sMaster = {0};
  
  AOT_TIM_RCC_ENBL();
  hTimCpt.Instance                 = AOT_TIM;
  sClkSrc.ClockPolarity            = TIM_CLOCKPOLARITY_NONINVERTED;
  sClkSrc.ClockPrescaler           = TIM_CLOCKPRESCALER_DIV1;
  sClkSrc.ClockSource              = TIM_CLOCKSOURCE_INTERNAL;                    
  HAL_TIM_ConfigClockSource(&hTimCpt, &sClkSrc);
//  HAL_TIM_Base_Stop(&hTimCpt); 
  HAL_TIM_Base_DeInit(&hTimCpt);

  if(Slct == AO)
  {
    hTimCpt.Init.CounterMode       = TIM_COUNTERMODE_UP;
    hTimCpt.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    hTimCpt.Init.Prescaler         = Psc;
    hTimCpt.Init.Period            = Period; 
    hTimCpt.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    sMaster.MasterOutputTrigger    = TIM_TRGO_UPDATE;
    sMaster.MasterSlaveMode        = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&hTimCpt, &sMaster);
    HAL_TIM_Base_Init(&hTimCpt);
    HAL_TIM_Base_Start(&hTimCpt); 
  }
//  else HAL_TIM_Base_Stop(&hTimCpt);
}
/*******************************************************************************
  PSC: 0~7 -> 对应 0~128 8级可选
*******************************************************************************/
void PulseGen_TimerCnfg(u16 Slct, u16 Psc, u16 Period, u16 Pulse)
{
  LPTIM_HandleTypeDef   hLpTim  = {0}; 
  
  if(Slct == PO)
  {
    PO_TIM_RCC_ENBL();
    hLpTim.Instance             = PO_TIM;
    hLpTim.Instance             = LPTIM2;
    hLpTim.Init.Clock.Source    = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
    hLpTim.Init.Clock.Prescaler = (Psc & 0x07) << 9;
    hLpTim.Init.Trigger.Source  = LPTIM_TRIGSOURCE_SOFTWARE;
    hLpTim.Init.OutputPolarity  = LPTIM_OUTPUTPOLARITY_HIGH;
    hLpTim.Init.UpdateMode      = LPTIM_UPDATE_IMMEDIATE;
    hLpTim.Init.CounterSource   = LPTIM_COUNTERSOURCE_INTERNAL;
    hLpTim.Init.Input1Source    = LPTIM_INPUT1SOURCE_GPIO;
    hLpTim.Init.Input2Source    = LPTIM_INPUT2SOURCE_GPIO;
    HAL_LPTIM_Init(&hLpTim);
    HAL_LPTIM_PWM_Start(&hLpTim, Period, Pulse);
  }
  else 
  {
    HAL_LPTIM_PWM_Stop(&hLpTim);
    PO_TIM_RCC_DSBL();
  }
}
/*******************************************************************************
  捕获测周期模式: 频率 < 12.5KHz 误差 < 0.08%  @16MHz
*******************************************************************************/
void CaptureEnbl(void)
{
////  HAL_TIM_IC_Start_IT(&hTimCpt, CAPTURE_CHx);
////  __HAL_TIM_ENABLE_IT(&hTimCpt, TIM_IT_CC4);
  FH_CPT_TIM->DIER |= TIM_DIER_CC4IE;
  FH_CPT_TIM->CCER |= TIM_CCER_CC4E;
  FH_CPT_TIM->CR1  |= TIM_CR1_CEN;
}
void CaptureDsbl(void)
{
////  HAL_TIM_IC_Stop_IT(&hTimCpt, CAPTURE_CHx);
////  __HAL_TIM_DISABLE_IT(&hTimCpt, TIM_IT_CC4);
  FH_CPT_TIM->DIER &= ~TIM_DIER_CC4IE;
  FH_CPT_TIM->CCER &= ~TIM_CCER_CC4E;
  FH_CPT_TIM->CR1  &= ~TIM_CR1_CEN;
}
/*******************************************************************************
  周期测量捕获中断处理程序                      实测信号周期(uS) = MeasuVal/16
*******************************************************************************/
void CaptureTimerISP(void)
{
  if(FH_CPT_TIM->SR & TIM_FLAG_CC4 && FH_CPT_TIM->DIER & TIM_IT_CC4)
  {
    FH_CPT_TIM->SR = ~TIM_IT_CC4;
    u32 CurrVal = CAPTURE_VAL;
    u32 Capture = CurrVal-LastCpt;
    if((1 << Item) & CXM+LXM+LRM+FXM) Wx[Item+6] = 1000*Capture/16; // @ nS
    LastCpt = CurrVal;
    CptActv = 1;       // 有新捕捉结果产生
  }
}
/*******************************************************************************
  32768Hz 精准时钟校正捕获测频          理论频差系数为 16MHz/(32768Hz/8)= 3906
*******************************************************************************/
void CalbrTimerInit(void)
{
  TIM_HandleTypeDef   hTimClb = {0};
  TIM_IC_InitTypeDef  sCnfgIC = {0};
  
  CLBR_TIM_RCC_ENBL();
  hTimClb.Instance               = CLBR_TIM; // 0x4001 4400
  hTimClb.Init.Prescaler         = 0;
  hTimClb.Init.Period            = 0xFFFF;
  hTimClb.Init.CounterMode       = TIM_COUNTERMODE_UP;
  hTimClb.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  hTimClb.Init.RepetitionCounter = 0;
  hTimClb.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_IC_Init(&hTimClb);
  
  sCnfgIC.ICPolarity             = TIM_INPUTCHANNELPOLARITY_RISING;
  sCnfgIC.ICSelection            = TIM_ICSELECTION_DIRECTTI;
  sCnfgIC.ICPrescaler            = TIM_ICPSC_DIV8;
  sCnfgIC.ICFilter               = 0;
  HAL_TIM_IC_ConfigChannel(&hTimClb, &sCnfgIC, CLBR_CH);
  HAL_TIMEx_RemapConfig(&hTimClb, CLBR_SRC);
  
  HAL_NVIC_SetPriority(CLBR_IRQ, 0, 0);
  HAL_NVIC_EnableIRQ(CLBR_IRQ);

  HAL_TIM_IC_Start_IT(&hTimClb, CLBR_CH);
}
/*******************************************************************************
  频率校正捕获与计数测频进位中断处理程序   实际频差校正值 = CalbrSumT/128/3906
*******************************************************************************/
u16 CalbrCntT, CalbrFrqn;
u32 CalbrSumT = 3906*128;

void Calbr_CntF_TimerISP(void)
{
  if(CLBR_TIM->SR & TIM_FLAG_CC1 && CLBR_TIM->DIER & TIM_IT_CC1)
  {
    u32 Count = CLBR_CNT;
    CalbrFrqn = Count-CalbrCntT;
    CalbrCntT = Count;
    CLBR_TIM->SR = ~TIM_IT_CC1;
  }
  if(CNT_TIM->SR & TIM_IT_UPDATE && CNT_TIM->DIER & TIM_IT_UPDATE)
  {
    CntHigh++;                    // Fx 计数测频高 16 位计数
    CNT_TIM->SR = ~TIM_IT_UPDATE;
  }
}
/*******************************************************************************
  
*******************************************************************************/
/*
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//  TIM6->SR = 0; TIM6->DIER = 0; TIM6->CR1 = 0;
//}
void Cpt_Fh_Vout_TimerCnfg(u16 Slct, u16 Psc, u32 Cycle, u32 Pulse)
{
  TIM_HandleTypeDef        hTimCpt = {0};
  TIM_IC_InitTypeDef       sCnfgIC = {0};
  TIM_ClockConfigTypeDef   sClkSrc = {0};
  TIM_OC_InitTypeDef       sCnfgOC = {0};
  TIM_MasterConfigTypeDef  sMaster = {0};
  
  u32 Mode = 1 << (Slct & 0x0F);
  u32 Kind = Slct & 0x00F0;   // Kind = DCV, SINE, NOISE, TRIANGL, USER_DEF
  
//  VAO_VAP_FH_PIN Init in DAC_Drv.c
  
  CPT_VO_TIM_RCC_EN();
  hTimCpt.Instance                  = CPT_VO_TIM;
  sClkSrc.ClockPolarity             = TIM_CLOCKPOLARITY_NONINVERTED;
//  sClkSrc.ClockPrescaler            = TIM_CLOCKPRESCALER_DIV1;
  sClkSrc.ClockPrescaler            = (Mode & FHM) ? TIM_CLOCKPRESCALER_DIV8 : TIM_CLOCKPRESCALER_DIV1;
  if(Mode & FHM)sClkSrc.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
  else          sClkSrc.ClockSource = TIM_CLOCKSOURCE_INTERNAL;                    
  sClkSrc.ClockFilter               = 0;// 0~15
  HAL_TIM_ConfigClockSource(&hTimCpt, &sClkSrc);
  
  hTimCpt.Init.CounterMode          = TIM_COUNTERMODE_UP;
  hTimCpt.Init.ClockDivision        = TIM_CLOCKDIVISION_DIV1;
  hTimCpt.Init.RepetitionCounter    = 0;
  hTimCpt.Init.Prescaler            = 0;
  hTimCpt.Init.Period               = 0xFFFFFFFF;
  hTimCpt.Init.AutoReloadPreload    = TIM_AUTORELOAD_PRELOAD_ENABLE;
  sMaster.MasterOutputTrigger       = TIM_TRGO_RESET;
  sMaster.MasterSlaveMode           = TIM_MASTERSLAVEMODE_DISABLE;
  
  CaptureDsbl();
  HAL_NVIC_DisableIRQ(CAPTURE_IRQ);
  __HAL_TIM_DISABLE(&hTimCpt);
  HAL_TIM_PWM_Stop(&hTimCpt, VO_PULSE_CHx);
  HAL_TIM_IC_DeInit(&hTimCpt);
  if(Mode & FHM)
  {
    HAL_TIM_Base_Init(&hTimCpt);
    HAL_TIM_Base_Start(&hTimCpt); 
  }
  else if(Mode & (CXM|FXM|LXM|LRM))
  {
    sCnfgIC.ICPolarity              = TIM_INPUTCHANNELPOLARITY_RISING;
    sCnfgIC.ICSelection             = TIM_ICSELECTION_DIRECTTI;
    sCnfgIC.ICPrescaler             = TIM_ICPSC_DIV1;
    sCnfgIC.ICFilter                = 15;// 0~15
    HAL_TIM_IC_Init(&hTimCpt);
    HAL_TIM_IC_ConfigChannel(&hTimCpt, &sCnfgIC, CAPTURE_CHx);
    HAL_TIMEx_RemapConfig(&hTimCpt, CAPTURE_REMAP);
    
    HAL_NVIC_SetPriority(CAPTURE_IRQ, 1, 0);
    HAL_NVIC_EnableIRQ(CAPTURE_IRQ);
  }
  else if(Mode & (AOM|DOM|APM|DPM))
  {
    hTimCpt.Init.Prescaler          = Psc;
    hTimCpt.Init.Period             = Cycle; 
    if(Mode & (APM|DPM))
    {
      HAL_TIM_PWM_Init (&hTimCpt);
      sCnfgOC.OCMode                = TIM_OCMODE_PWM1;
      sCnfgOC.Pulse                 = Pulse;
      sCnfgOC.OCPolarity            = TIM_OCPOLARITY_HIGH;
      sCnfgOC.OCNPolarity           = TIM_OCNPOLARITY_HIGH;
      sCnfgOC.OCFastMode            = TIM_OCFAST_DISABLE;
      sCnfgOC.OCIdleState           = TIM_OCIDLESTATE_RESET;
      sCnfgOC.OCNIdleState          = TIM_OCNIDLESTATE_RESET;
      HAL_TIM_PWM_ConfigChannel(&hTimCpt, &sCnfgOC, VO_PULSE_CHx);
      HAL_TIM_PWM_Start(&hTimCpt, VO_PULSE_CHx);
    }
    else if(Mode & (AOM | DOM) && Kind != VO_DCV)
    {
      HAL_TIM_Base_Init(&hTimCpt);
      sMaster.MasterOutputTrigger   = TIM_TRGO_UPDATE;
      HAL_TIM_Base_Start(&hTimCpt); 
    }
  }
  HAL_TIMEx_MasterConfigSynchronization(&hTimCpt, &sMaster);
}
*/

/******************************** END OF FILE *********************************/
