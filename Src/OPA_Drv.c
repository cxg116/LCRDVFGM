/********************* (C) COPYRIGHT 2019 e-Design Co.,Ltd. ********************

 Brief: 加载在 BIOS 中的 OPAMP 驱动程序子模块                    Author : bure

*******************************************************************************/
  

/*******************************************************************************

*******************************************************************************/
void OPAMP_IoCnfg(u32 Type)
{
  GPIO_InitTypeDef  sGpio = {0};

  sGpio.Mode      = GPIO_MODE_ANALOG;
  sGpio.Pin       = Vp_PIN;
  HAL_GPIO_Init(OPA_IO_PORT, &sGpio);
  
  sGpio.Mode      = (Type <= FX) ? GPIO_MODE_OUTPUT_PP : GPIO_MODE_ANALOG;
  sGpio.Pin       = Vo_PIN;
  HAL_GPIO_Init(OPA_IO_PORT, &sGpio);
  
  sGpio.Mode      = (Type >= VP) ? GPIO_MODE_OUTPUT_PP : GPIO_MODE_ANALOG;
  sGpio.Pin       = Vm_PIN;
  HAL_GPIO_Init(OPA_IO_PORT, &sGpio);
  
  SET_PINS(OPA_IO_PORT, Vo_PIN);
  if(Type == RH) SET_PINS(OPA_IO_PORT, Vm_PIN);
  else           CLR_PINS(OPA_IO_PORT, Vm_PIN);
}
/*******************************************************************************
  OPAMP Initialization Function
*******************************************************************************/
void OPAMP_DevCnfg(u32 Type)
{
  OPAMP_HandleTypeDef  hOpAmp = {0};
  
  __HAL_RCC_OPAMP_CLK_ENABLE();
  hOpAmp.Instance               = OPAMPx;
  hOpAmp.Init.PgaGain           = OPAMP_PGA_GAIN_16;
  hOpAmp.Init.Mode              = (Type == RL || Type == VL) ? OPAMP_PGA_MODE :  
                                  (Type == VM) ? OPAMP_STANDALONE_MODE : OPAMP_FOLLOWER_MODE;
  hOpAmp.Init.InvertingInput    = (Type == VM) ? VM_CONNECT_PA0 : VM_CONNECT_NO;
  hOpAmp.Init.NonInvertingInput = VP_CONNECT_PA1;
  hOpAmp.Init.PowerSupplyRange  = OPAMP_POWERSUPPLY_HIGH;
  hOpAmp.Init.PowerMode         = OPAMP_POWERMODE_LOWPOWER;
  hOpAmp.Init.UserTrimming      = OPAMP_TRIMMING_FACTORY;
  if(HAL_OPAMP_Init(&hOpAmp) != HAL_OK) ErrLoop();
  
  if(Type >= VM && Type <= RH)  HAL_OPAMP_Start(&hOpAmp);
  else                          HAL_OPAMP_Stop (&hOpAmp);
}
/*******************************************************************************

*******************************************************************************/
void OPAMP_DevInit(void)
{
  OPAMP_DevCnfg(NC);
  OPAMP_IoCnfg(NC);
}
/*******************************************************************************
  @brief OPAMP Initialization Function  Gain = -n, 1, 2, 4, 8, 16
******************************************************************************
void OpAmpCnfg(s8 Gain)
{
  hOpAmp.Instance               = OPAMPx;
  hOpAmp.Init.PowerSupplyRange  = OPAMP_POWERSUPPLY_HIGH;
  hOpAmp.Init.InvertingInput    = OPAMP_INVERTINGINPUT_IO1;
  hOpAmp.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO0;
  hOpAmp.Init.PowerMode         = OPAMP_POWERMODE_LOWPOWER;
  hOpAmp.Init.UserTrimming      = OPAMP_TRIMMING_FACTORY;
  
  if(Gain < 0)
  {
    hOpAmp.Init.Mode            = OPAMP_STANDALONE_MODE;
  }
  else if(Gain == 1)
  {
    hOpAmp.Init.Mode            = OPAMP_FOLLOWER_MODE;
  }
  else if(Gain > 1)
  {
    hOpAmp.Init.Mode            = OPAMP_PGA_MODE;
    hOpAmp.Init.PgaGain         = (Gain == 2) ? OPAMP_PGA_GAIN_2 :
                                  (Gain == 4) ? OPAMP_PGA_GAIN_4 :
                                  (Gain == 8) ? OPAMP_PGA_GAIN_8 :
                                  OPAMP_PGA_GAIN_16;
  }
  if(HAL_OPAMP_Stop (&hOpAmp) != HAL_OK) ErrLoop();
  if(HAL_OPAMP_Init (&hOpAmp) != HAL_OK) ErrLoop();
  if(HAL_OPAMP_Start(&hOpAmp) != HAL_OK) ErrLoop();
}*/
/******************************** END OF FILE *********************************/
