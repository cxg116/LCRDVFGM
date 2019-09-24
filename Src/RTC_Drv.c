/*******************************************************************************
  Brief: 加载在 BIOS 中的 RTC 驱动程序子模块                    Author : bure
*******************************************************************************/

RTC_HandleTypeDef hRtc;

/*******************************************************************************
  RTC Initialization Function
*******************************************************************************/
void RTC_DevInit(void)
{
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  
  RTC_RCC_EN();
  
  hRtc.Instance            = RTC; // 0x4000 2800
  hRtc.Init.HourFormat     = RTC_HOURFORMAT_24;
  hRtc.Init.AsynchPrediv   = 127;
  hRtc.Init.SynchPrediv    = 255;
  hRtc.Init.OutPut         = RTC_OUTPUT_DISABLE;
  hRtc.Init.OutPutRemap    = RTC_OUTPUT_REMAP_NONE;
  hRtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hRtc.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
  if(HAL_RTC_Init(&hRtc) != HAL_OK) ErrLoop();
  
  sTime.Hours              = 0;
  sTime.Minutes            = 0;
  sTime.Seconds            = 0;
  sTime.DayLightSaving     = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation     = RTC_STOREOPERATION_RESET;
  if(HAL_RTC_SetTime(&hRtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) ErrLoop();
   
  sDate.WeekDay            = RTC_WEEKDAY_SUNDAY;
  sDate.Month              = RTC_MONTH_MARCH;
  sDate.Date               = 24;
  sDate.Year               = 19;
  if(HAL_RTC_SetDate(&hRtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) ErrLoop();
}
/*******************************************************************************
  
*******************************************************************************/
void RTC_GetTimeStr(char* Str)
{ 
  RTC_TimeTypeDef sTime = {0};

  HAL_RTC_GetTime(&hRtc, &sTime, RTC_FORMAT_BIN);
  u8To2DecStr(&Str[0], sTime.Hours);
  Str[2] = ':';
  u8To2DecStr(&Str[3], sTime.Minutes);
  Str[5] = ':';
  u8To2DecStr(&Str[6], sTime.Seconds);
//  Str[8] = ':';
//  u8To2DecStr(&Str[9], sTime.SubSeconds);
}
/*******************************************************************************
  
*******************************************************************************/
void RTC_GetDateStr(char* Str)
{ 
  RTC_DateTypeDef sDate = {0};

  Str[0] = '2';
  Str[1] = '0';
  HAL_RTC_GetDate(&hRtc, &sDate, RTC_FORMAT_BIN);
  u8To2DecStr(&Str[2], sDate.Year);
  Str[4] = '-';
  u8To2DecStr(&Str[5], sDate.Month);
  Str[7] = '-';
  u8To2DecStr(&Str[8], sDate.Date);
}
/******************************** END OF FILE *********************************/
