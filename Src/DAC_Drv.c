/*******************************************************************************
  Brief: 加载在 BIOS 中的 DAC 驱动程序子模块                     Author : bure
*******************************************************************************/

uc16 SINE100[100] = 
{
  0x7FF, 0x87F, 0x8FF, 0x97E, 0x9FC, 0xA77, 0xAF0, 0xB66, 0xBD9, 0xC48, 
  0xCB2, 0xD18, 0xD78, 0xDD3, 0xE29, 0xE77, 0xEC0, 0xF01, 0xF3C, 0xF6F, 
  0xF9A, 0xFBE, 0xFDA, 0xFEE, 0xFFA, 0xFFE, 0xFFA, 0xFEE, 0xFDA, 0xFBE, 
  0xF9A, 0xF6F, 0xF3C, 0xF01, 0xEC0, 0xE77, 0xE29, 0xDD3, 0xD78, 0xD18, 
  0xCB2, 0xC48, 0xBD9, 0xB66, 0xAF0, 0xA77, 0x9FC, 0x97E, 0x8FF, 0x87F, 
  0x7FE, 0x77E, 0x6FE, 0x67F, 0x601, 0x586, 0x50D, 0x496, 0x424, 0x3B5, 
  0x34B, 0x2E5, 0x285, 0x22A, 0x1D4, 0x186, 0x13D, 0x0FC, 0x0C1, 0x08E, 
  0x063, 0x03F, 0x023, 0x00F, 0x003, 0x000, 0x003, 0x00F, 0x023, 0x03F, 
  0x063, 0x08E, 0x0C1, 0x0FC, 0x13D, 0x186, 0x1D5, 0x22A, 0x285, 0x2E5, 
  0x34B, 0x3B5, 0x424, 0x497, 0x50D, 0x586, 0x601, 0x67F, 0x6FE, 0x77E,
};

u16 Wfrm[512];

/*******************************************************************************
  Kind = SINE/NOISE/DC_VAL/TRIANGL/USE_DEF/DGTL               Vpp_mV: 0~3000mV
*******************************************************************************/
void DAC_Cnfg(u16 Slct, u16 n, u16* pBuf)
{
  DAC_HandleTypeDef      hDac  = {0};
  DAC_ChannelConfTypeDef sCnfg = {0};
  DMA_HandleTypeDef      hDma  = {0};

  DAC_DMA_RCC_EN();
  hDma.Instance                       = DC_DMA_CH;
  HAL_DMA_DeInit(&hDma);
  hDma.Init.Request                   = DC_DMA_REQ;
  hDma.Init.Direction                 = DMA_MEMORY_TO_PERIPH;
  hDma.Init.PeriphInc                 = DMA_PINC_DISABLE;
  hDma.Init.MemInc                    = DMA_MINC_ENABLE;
  hDma.Init.PeriphDataAlignment       = DMA_PDATAALIGN_HALFWORD;
  hDma.Init.MemDataAlignment          = DMA_MDATAALIGN_HALFWORD;
  hDma.Init.Mode                      = DMA_CIRCULAR;
  hDma.Init.Priority                  = DMA_PRIORITY_HIGH;
  
  DAC_RCC_EN();
  hDac.Instance                       = DACx;
  HAL_DAC_Stop_DMA(&hDac, VOUT_DC_CH);
  sCnfg.DAC_SampleAndHold             = DAC_SAMPLEANDHOLD_DISABLE;
  sCnfg.DAC_UserTrimming              = DAC_TRIMMING_FACTORY;
  HAL_DAC_Stop_DMA(&hDac, VOUT_DC_CH);
//  HAL_DAC_Stop(&hDac, VOUT_DC_CH);
  HAL_DAC_DeInit(&hDac);
  
  if(Slct == FX || Slct == NV)
  {
    sCnfg.DAC_OutputBuffer            = DAC_OUTPUTBUFFER_DISABLE;
    sCnfg.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_ENABLE;
    sCnfg.DAC_Trigger                 = DAC_TRIGGER_NONE;
    HAL_DAC_Init(&hDac);
    HAL_DAC_ConfigChannel(&hDac, &sCnfg, VOUT_DC_CH);
    HAL_DAC_SetValue(&hDac, VOUT_DC_CH, DAC_ALIGN_12B_R, (Slct == FX) ? 2048 : 0);
    HAL_DAC_Start(&hDac, VOUT_DC_CH);
  }
  if(Slct == AO)
  {
    sCnfg.DAC_OutputBuffer            = DAC_OUTPUTBUFFER_ENABLE;
    sCnfg.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
    sCnfg.DAC_Trigger                 = VOUT_ANLG_TRIG;
    HAL_DAC_Init(&hDac);
    HAL_DAC_ConfigChannel(&hDac, &sCnfg, VOUT_DC_CH);
    
    if(Mode == SINEWAV || Mode == USERDEF)
    {
      HAL_DMA_Init(&hDma);
      __HAL_LINKDMA(&hDac, DC_DMA_HANDLE, hDma);
      HAL_DAC_Start_DMA(&hDac, VOUT_DC_CH, (u32*)pBuf, n, DAC_ALIGN_12B_R);
    }
    else if(Mode == NOISES)
    {
      HAL_DACEx_NoiseWaveGenerate(&hDac, VOUT_DC_CH, DAC_LFSRUNMASK_BITS11_0);
      HAL_DAC_Start(&hDac, VOUT_DC_CH);
    }
  }
//  if(Modes & DOM)
//  {
//    u32 TrigSrc = (Kind == VO_DCV) ? DAC_TRIGGER_NONE : VOUT_ANLG_TRIG;
//    sCnfg.DAC_Trigger                 = TrigSrc;
//    sCnfg.DAC_OutputBuffer            = DAC_OUTPUTBUFFER_ENABLE;
//    sCnfg.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
//    HAL_DAC_Init(&hDac);
//    HAL_DAC_ConfigChannel(&hDac, &sCnfg, VOUT_DC_CH);
//    if(Kind == VO_DCV) 
//    {
//      HAL_DAC_SetValue(&hDac, VOUT_DC_CH, DAC_ALIGN_12B_R, Ampl);
//      HAL_DAC_Start(&hDac, VOUT_DC_CH);
//    }
//    else if(Kind == VO_SINE || Kind == VO_USER_DEF)
//    {
//      for(u32 i = 0; i < n; i++) Wfrm[i] = Vpp_mV*pWfrm[i]/3000; 
//      hDma.Instance                   = DC_DMA_CH;
//      hDma.Init.Request               = DC_DMA_REQ;
//      hDma.Init.Direction             = DMA_MEMORY_TO_PERIPH;
//      hDma.Init.PeriphInc             = DMA_PINC_DISABLE;
//      hDma.Init.MemInc                = DMA_MINC_ENABLE;
//      hDma.Init.PeriphDataAlignment   = DMA_PDATAALIGN_HALFWORD;
//      hDma.Init.MemDataAlignment      = DMA_MDATAALIGN_HALFWORD;
//      hDma.Init.Mode                  = DMA_CIRCULAR;
//      hDma.Init.Priority              = DMA_PRIORITY_HIGH;
//      if(HAL_DMA_Init(&hDma) != HAL_OK) ErrLoop();
//      __HAL_LINKDMA(&hDac, DC_DMA_HANDLE, hDma);
//      HAL_DAC_Start_DMA(&hDac, VOUT_DC_CH, (u32*)Wfrm, n, DAC_ALIGN_12B_R);
//    }
//    else if(Kind == VO_NOISE)
//    {
//      if(Ampl >= 2048)      Ampl = DAC_LFSRUNMASK_BITS11_0;
//      else if(Ampl >= 1024) Ampl = DAC_LFSRUNMASK_BITS10_0;
//      else if(Ampl >=  512) Ampl = DAC_LFSRUNMASK_BITS9_0;
//      else if(Ampl >=  256) Ampl = DAC_LFSRUNMASK_BITS8_0;
//      else if(Ampl >=  128) Ampl = DAC_LFSRUNMASK_BITS7_0;
//      else if(Ampl >=   64) Ampl = DAC_LFSRUNMASK_BITS6_0;
//      else if(Ampl >=   32) Ampl = DAC_LFSRUNMASK_BITS5_0;
//      else if(Ampl >=   16) Ampl = DAC_LFSRUNMASK_BITS4_0;
//      else if(Ampl >=    8) Ampl = DAC_LFSRUNMASK_BITS3_0;
//      else if(Ampl >=    4) Ampl = DAC_LFSRUNMASK_BITS2_0;
//      else if(Ampl >=    2) Ampl = DAC_LFSRUNMASK_BITS1_0;
//      else                  Ampl = DAC_LFSRUNMASK_BIT0;
//      HAL_DACEx_NoiseWaveGenerate(&hDac, VOUT_DC_CH, Ampl);
//      HAL_DAC_Start(&hDac, VOUT_DC_CH);
//    }
//    else if(Kind == VO_TRIANGL)
//    {
//      if(Ampl >= 2048)      Ampl = DAC_TRIANGLEAMPLITUDE_4095;
//      else if(Ampl >= 1024) Ampl = DAC_TRIANGLEAMPLITUDE_2047;
//      else if(Ampl >=  512) Ampl = DAC_TRIANGLEAMPLITUDE_1023;
//      else if(Ampl >=  256) Ampl = DAC_TRIANGLEAMPLITUDE_511;
//      else if(Ampl >=  128) Ampl = DAC_TRIANGLEAMPLITUDE_255;
//      else if(Ampl >=   64) Ampl = DAC_TRIANGLEAMPLITUDE_127;
//      else if(Ampl >=   32) Ampl = DAC_TRIANGLEAMPLITUDE_63;
//      else if(Ampl >=   16) Ampl = DAC_TRIANGLEAMPLITUDE_31;
//      else if(Ampl >=    8) Ampl = DAC_TRIANGLEAMPLITUDE_15;
//      else if(Ampl >=    4) Ampl = DAC_TRIANGLEAMPLITUDE_7;
//      else if(Ampl >=    2) Ampl = DAC_TRIANGLEAMPLITUDE_3;
//      else                  Ampl = DAC_TRIANGLEAMPLITUDE_1;
//      HAL_DACEx_TriangleWaveGenerate(&hDac, VOUT_DC_CH, Ampl);
//      HAL_DAC_Start(&hDac, VOUT_DC_CH);
//    }
//  }
//  else 
}

/******************************** END OF FILE *********************************/
