/*******************************************************************************
  Brief: 加载在 BIOS 中的 CTRL 驱动程序子模块                     Author : bure
*******************************************************************************/

TSC_HandleTypeDef hTsc  = {0};

u32 TSCxVal = 0, TSCxSt = 0, TSCxKey = 0, TSCxSum = 6500;

/*******************************************************************************

*******************************************************************************/
void HAL_TSC_ConvCpltCallback(TSC_HandleTypeDef *hTsc)
{
  HAL_TSC_IODischarge(hTsc, ENABLE);
  u32 Wait = (SystemCoreClock/10000);
  while (Wait != 0)
  {
    Wait--;
  }
  if(HAL_TSC_GroupGetStatus(hTsc, TSC_GROUP2_IDX) == TSC_GROUP_COMPLETED)
  {
    TSCxVal = HAL_TSC_GroupGetValue(hTsc, TSC_GROUP2_IDX); 
    TSCxSum = TSCxSum-TSCxSum/32+TSCxVal;
    if(TSCxSt == 1 && TSCxVal < TSCxSum/32-100)
    {
      TSCxSum = TSCxVal*32;
      TSCxSt  = 0;
    }
    if(TSCxSt == 0 && TSCxVal > TSCxSum/32+100) 
    {
      TSCxSum = TSCxVal*32;
      TSCxSt  = 1;
      TSCxKey = 1;
    }
  }
  if(HAL_TSC_Start_IT(hTsc) != HAL_OK) ErrLoop();
}
/*******************************************************************************

*******************************************************************************/
void HAL_TSC_MspInit(TSC_HandleTypeDef *hTsc)
{
  GPIO_InitTypeDef  sGpio = {0};
  
  TSC_IO_RCC_EN();
  sGpio.Speed     = GPIO_SPEED_FREQ_LOW;
  sGpio.Pull      = GPIO_NOPULL;
  sGpio.Alternate = TSC_AF_MAP;
  
  sGpio.Pin       = TSC_SMPL_PIN;
  sGpio.Mode      = GPIO_MODE_AF_OD;
  HAL_GPIO_Init(TSC_IO_PORT, &sGpio);

  sGpio.Pin       = TSC_KEY1_PIN;
  sGpio.Mode      = GPIO_MODE_AF_PP;
  HAL_GPIO_Init(TSC_IO_PORT, &sGpio);

  HAL_NVIC_SetPriority(TSC_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(TSC_IRQn);
}
/*******************************************************************************

*******************************************************************************/
void TSC_DevInit(void)
{
  TSC_DEV_RCC_EN();
  hTsc.Instance                     = TSC;
  hTsc.Init.AcquisitionMode         = TSC_ACQ_MODE_NORMAL;
  hTsc.Init.CTPulseHighLength       = TSC_CTPH_2CYCLES;
  hTsc.Init.CTPulseLowLength        = TSC_CTPL_2CYCLES;
  hTsc.Init.IODefaultMode           = TSC_IODEF_OUT_PP_LOW;
  hTsc.Init.MaxCountInterrupt       = ENABLE;
  hTsc.Init.MaxCountValue           = TSC_MCV_16383;
  hTsc.Init.PulseGeneratorPrescaler = TSC_PG_PRESC_DIV64;
  hTsc.Init.SpreadSpectrum          = DISABLE;
  hTsc.Init.SpreadSpectrumDeviation = 1;
  hTsc.Init.SpreadSpectrumPrescaler = TSC_SS_PRESC_DIV1;
  hTsc.Init.SynchroPinPolarity      = TSC_SYNC_POLARITY_FALLING;
  hTsc.Init.ChannelIOs              = TSC_GROUP2_IO1;
  hTsc.Init.SamplingIOs             = TSC_GROUP2_IO4;
  hTsc.Init.ShieldIOs               = 0;
  if(HAL_TSC_Init(&hTsc) != HAL_OK) ErrLoop();
  
  HAL_TSC_IODischarge(&hTsc, ENABLE);
  HAL_Delay(1);
  if(HAL_TSC_Start_IT(&hTsc) != HAL_OK) ErrLoop();
}

/******************************** END OF FILE *********************************/
