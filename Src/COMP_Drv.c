/*******************************************************************************
  Brief: 加载在 BIOS 中的 COMP 驱动程序子模块                    Author : bure
*******************************************************************************/


/*******************************************************************************

*******************************************************************************/
void COMP_DevCnfg(u32 Slct)
{
  COMP_HandleTypeDef  hComp = {0};
  GPIO_InitTypeDef    sGpio = {0};
  
  u32 Modes = 1 << (Slct & 0x0F);
  u32 Items;
  
  hComp.Instance               = COMPx;
  hComp.Init.NonInvertingInput = CP_CONNECT;
  hComp.Init.InvertingInput    = (Modes & FXM) ? CM_CONNECT_DAC : CM_CONNECT;
  hComp.Init.Hysteresis        = COMP_HYSTERESIS_HIGH;
  hComp.Init.Mode              = COMP_POWERMODE_HIGHSPEED;
  hComp.Init.OutputPol         = COMP_OUTPUTPOL_NONINVERTED;
  hComp.Init.WindowMode        = COMP_WINDOWMODE_DISABLE;
  hComp.Init.BlankingSrce      = COMP_BLANKINGSRC_NONE;
  hComp.Init.TriggerMode       = COMP_TRIGGERMODE_NONE;
  HAL_COMP_Init(&hComp);
  if(Modes & CXM+FXM+LXM+LRM)
  {
    HAL_COMP_Init(&hComp);
    HAL_COMP_Start(&hComp);
  }
  else 
  {
    HAL_COMP_Stop(&hComp);
    HAL_COMP_DeInit(&hComp);
  }
  
  COMP_IO_RCC_EN();
  sGpio.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  sGpio.Pull      = GPIO_NOPULL;
  
  Items           = Modes & RHM+LVM+PVM;
  sGpio.Pin       = CP_VM_PIN;
  sGpio.Mode      = (Items) ? GPIO_MODE_OUTPUT_PP : GPIO_MODE_ANALOG;
  HAL_GPIO_Init(COMP_IO_PORT, &sGpio);
  if(Modes & RHM) SET_PINS(COMP_IO_PORT, CP_VM_PIN);
  else            CLR_PINS(COMP_IO_PORT, CP_VM_PIN);
  
  sGpio.Pin       = CM_AO_PO_PIN;
  u32 Item1       = Modes & POM;
  u32 Item2       = Modes & RLM+RMM;
  sGpio.Alternate = (Item1) ? PO_IO_AF : CO_AF_MAP;
  sGpio.Mode      = (Item1) ? GPIO_MODE_AF_PP : 
                    (Item2) ? GPIO_MODE_OUTPUT_PP : GPIO_MODE_ANALOG;
  HAL_GPIO_Init(COMP_IO_PORT, &sGpio);
  SET_PINS(COMP_IO_PORT, CM_AO_PO_PIN);

  Items           = Modes & CXM+FXM+LXM+LRM;
  sGpio.Pin       = CO_VP_PIN;
  sGpio.Alternate = CO_AF_MAP;
  sGpio.Mode      = (Items) ? GPIO_MODE_AF_PP : GPIO_MODE_ANALOG;
  HAL_GPIO_Init(COMP_IO_PORT, &sGpio);

}

/******************************** END OF FILE *********************************/
