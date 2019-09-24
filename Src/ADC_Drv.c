/*******************************************************************************
  Brief: 加载在 BIOS 中的 ADC 驱动程序子模块                    Author : bure
*******************************************************************************/

u16 AdcVlue[5];

/*******************************************************************************
  ADC Initialization Function
*******************************************************************************/
void ADC_DevInit(void)
{
  ADC_HandleTypeDef      hAdc  = {0};
  ADC_ChannelConfTypeDef sCnfg = {0};
  DMA_HandleTypeDef      hDma  = {0};
  
//  VIN_IO_RCC_EN();
//  sGpio.Pin                                    = VIN_PIN;
//  sGpio.Mode                                   = GPIO_MODE_ANALOG;//GPIO_MODE_ANALOG_ADC_CONTROL;
//  sGpio.Pull                                   = GPIO_NOPULL;
//  HAL_GPIO_Init(VIN_IO_PORT, &sGpio);
  
  ADC_DEV_RCC_EN();
  hAdc.Instance                                = ADCx; // 0x5004 0000
  hAdc.Init.ClockPrescaler                     = ADC_CLOCK_ASYNC_DIV32;//ADC_CLOCK_ASYNC_DIV256;//
  hAdc.Init.Resolution                         = ADC_RESOLUTION_12B;
  hAdc.Init.DataAlign                          = ADC_DATAALIGN_RIGHT;
  hAdc.Init.ScanConvMode                       = ADC_SCAN_ENABLE;
  hAdc.Init.EOCSelection                       = ADC_EOC_SEQ_CONV;
  hAdc.Init.LowPowerAutoWait                   = DISABLE;
  hAdc.Init.ContinuousConvMode                 = ENABLE;
  hAdc.Init.NbrOfConversion                    = 3;
  hAdc.Init.DiscontinuousConvMode              = DISABLE;
  hAdc.Init.ExternalTrigConv                   = ADC_SOFTWARE_START;
  hAdc.Init.ExternalTrigConvEdge               = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hAdc.Init.DMAContinuousRequests              = ENABLE;
//  hAdc.Init.Overrun                            = ADC_OVR_DATA_OVERWRITTEN;
//  hAdc.Init.OversamplingMode                   = DISABLE;
  hAdc.Init.Overrun                            = ADC_OVR_DATA_OVERWRITTEN;//ADC_OVR_DATA_PRESERVED;//
  hAdc.Init.OversamplingMode                   = ENABLE;
  hAdc.Init.Oversampling.Ratio                 = ADC_OVERSAMPLING_RATIO_256;
  hAdc.Init.Oversampling.RightBitShift         = ADC_RIGHTBITSHIFT_8;
  hAdc.Init.Oversampling.TriggeredMode         = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
  hAdc.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;
  HAL_ADC_Init(&hAdc);
  HAL_ADCEx_Calibration_Start(&hAdc, ADC_SINGLE_ENDED);
   
  sCnfg.SamplingTime                           = ADC_SAMPLETIME_12CYCLES_5;
  sCnfg.SingleDiff                             = ADC_SINGLE_ENDED;
  sCnfg.OffsetNumber                           = ADC_OFFSET_NONE;
  
  sCnfg.Channel                                = ADC_CHANNEL_7;
  sCnfg.Rank                                   = ADC_REGULAR_RANK_1;
  HAL_ADC_ConfigChannel(&hAdc, &sCnfg);
   
  sCnfg.Channel                                = VXM_CH;
  sCnfg.Rank                                   = ADC_REGULAR_RANK_2;
  HAL_ADC_ConfigChannel(&hAdc, &sCnfg);
   
  sCnfg.Channel                                = REF_CH;
  sCnfg.Rank                                   = ADC_REGULAR_RANK_3;
  HAL_ADC_ConfigChannel(&hAdc, &sCnfg);
   
//  sCnfg.Channel                                = ADC_CHANNEL_VBAT;
//  sCnfg.Rank                                   = ADC_REGULAR_RANK_4;
//  HAL_ADC_ConfigChannel(&hAdc, &sCnfg);
//   
//  sCnfg.Channel                                = ADC_CHANNEL_TEMPSENSOR;
//  sCnfg.Rank                                   = ADC_REGULAR_RANK_5;
//  HAL_ADC_ConfigChannel(&hAdc, &sCnfg);
  
  ADC_DMA_RCC_EN();
  hDma.Instance                                = ADC_DMA_CH; // 0x4002 0000
  hDma.Init.Request                            = DMA_REQUEST_0;
  hDma.Init.Direction                          = DMA_PERIPH_TO_MEMORY;
  hDma.Init.PeriphInc                          = DMA_PINC_DISABLE;
  hDma.Init.MemInc                             = DMA_MINC_ENABLE;
  hDma.Init.PeriphDataAlignment                = DMA_PDATAALIGN_HALFWORD;
  hDma.Init.MemDataAlignment                   = DMA_MDATAALIGN_HALFWORD;
  hDma.Init.Mode                               = DMA_CIRCULAR;
  hDma.Init.Priority                           = DMA_PRIORITY_LOW;
  HAL_DMA_Init(&hDma);
    
  __HAL_LINKDMA(&hAdc, DMA_Handle, hDma);

  HAL_ADC_Start(&hAdc);
  
  __HAL_DMA_DISABLE(&hDma);
  ADC_DMA_CH->CNDTR = 3;
  ADC_DMA_CH->CMAR  = (u32)AdcVlue;
  ADC_DMA_CH->CPAR  = (u32)&ADCx->DR;
  ADCx->CFGR       |= ADC_CFGR_DMAEN;
  __HAL_DMA_ENABLE(&hDma);
}

/******************************** END OF FILE *********************************/
