/*******************************************************************************
  Brief: 加载在 BIOS 中的 OPAMP 驱动程序子模块                    Author : bure
*******************************************************************************/


/*******************************************************************************
  OPAMP Initialization Function
*******************************************************************************/
void OPAMP_DevCnfg(u32 Slct)
{
  OPAMP_HandleTypeDef  hOpAmp = {0};
  GPIO_InitTypeDef     sGpio  = {0};
  
  OPAMP_IO_RCC_EN();
  sGpio.Pin   = VO_PIN;
  sGpio.Pull  = GPIO_NOPULL;
  sGpio.Mode  = GPIO_MODE_ANALOG;
  sGpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(OPAMP_IO_PORT, &sGpio);
  
  OPAMP_DEV_RCC_EN();
  hOpAmp.Instance               = OPAMPx;
  hOpAmp.Init.PgaGain           = ((1 << Slct) & RLM+LVM) ? OPAMP_PGA_GAIN_8 : OPAMP_PGA_GAIN_2;
  hOpAmp.Init.Mode              = ((1 << Slct) & RLM+LVM+NVM) ? OPAMP_PGA_MODE : 
                                  OPAMP_FOLLOWER_MODE; 
  hOpAmp.Init.InvertingInput    = ((1 << Slct) & NVM) ? VM_CONNECT_PA1 : VM_CONNECT_NO;
  hOpAmp.Init.NonInvertingInput = ((1 << Slct) & NVM) ? VP_CONNECT_DAC : VP_CONNECT_PA0;
  hOpAmp.Init.PowerSupplyRange  = OPAMP_POWERSUPPLY_HIGH;
  hOpAmp.Init.PowerMode         = OPAMP_POWERMODE_LOWPOWER;
  hOpAmp.Init.UserTrimming      = OPAMP_TRIMMING_FACTORY;
  
  if((1 << Slct) & LVM+PVM+NVM+RLM+RMM+RHM)
  {
    HAL_OPAMP_Init(&hOpAmp);
    HAL_OPAMP_Start(&hOpAmp);
  }
  else
  {
    HAL_OPAMP_Stop(&hOpAmp);
    HAL_OPAMP_DeInit(&hOpAmp);
  }
}
/*******************************************************************************
  @brief OPAMP Initialization Function  Gain = -n, 1, 2, 4, 8, 16
*******************************************************************************/
void OpAmpCnfg(s8 Gain)
{
  OPAMP_HandleTypeDef  hOpAmp = {0};
  
  OPAMP_DEV_RCC_EN();
  hOpAmp.Instance                     = OPAMPx;
  hOpAmp.Init.PowerSupplyRange        = OPAMP_POWERSUPPLY_HIGH;
  hOpAmp.Init.InvertingInput          = OPAMP_INVERTINGINPUT_IO1;
  hOpAmp.Init.NonInvertingInput       = OPAMP_NONINVERTINGINPUT_IO0;
  hOpAmp.Init.PowerMode               = OPAMP_POWERMODE_LOWPOWER;
  hOpAmp.Init.UserTrimming            = OPAMP_TRIMMING_FACTORY;
  if(Gain < 0) hOpAmp.Init.Mode       = OPAMP_STANDALONE_MODE;
  else if(Gain == 1) hOpAmp.Init.Mode = OPAMP_FOLLOWER_MODE;
  else if(Gain > 1)
  {
    hOpAmp.Init.Mode                  = OPAMP_PGA_MODE;
    hOpAmp.Init.PgaGain               = (Gain == 2) ? OPAMP_PGA_GAIN_2 :
                                        (Gain == 4) ? OPAMP_PGA_GAIN_4 :
                                        (Gain == 8) ? OPAMP_PGA_GAIN_8 :
                                        OPAMP_PGA_GAIN_16;
  }
  if(HAL_OPAMP_Init (&hOpAmp) != HAL_OK) ErrLoop();
  if(HAL_OPAMP_Start(&hOpAmp) != HAL_OK) ErrLoop();
}
/******************************** END OF FILE *********************************/
