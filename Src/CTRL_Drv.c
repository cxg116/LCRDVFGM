/*******************************************************************************
  Brief: 加载在 BIOS 中的 CTRL 驱动程序子模块                     Author : bure
*******************************************************************************/


/*******************************************************************************
  
*******************************************************************************/
void PowerDown(void)
{
  GPIO_InitTypeDef  sGpio = {0};
  
  OLED_Fill(0x00);                          // 清除 OLED 显示
  OLED_DispOff();                           // 关闭 OLED
  ACCEL_PWRDOWN();                          // 关闭加速度传感器
  MeasuSlctUpdt(NC);
  
  WAKEUP_IO_RCC_EN();
  sGpio.Pin  = WAKEUP_PIN | S5_PIN;
  sGpio.Mode = GPIO_MODE_INPUT;
  sGpio.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(WAKEUP_IO_PORT, &sGpio);    // Wakeup Pin & Select Pin Init
    
  HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_PORT, PWR_WAKEUP_PIN);
  HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_PORT, PWR_S5_PIN);
  HAL_PWREx_EnablePullUpPullDownConfig();
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP);     // Disable Last Wakeup Sources
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);        // Clear Related Wakeup Flags
  HAL_PWR_EnableWakeUpPin(WAKEUP_POLARITY); // Re-enable Wakeup Sources
  HAL_PWREx_EnterSHUTDOWNMode();            // Enter the Shutdown Mode
}
/*******************************************************************************
  Slct: Kind+Type
*******************************************************************************/
void CtrlPinsCnfg(u32 Slct)
{
  GPIO_InitTypeDef  sGpio = {0};
  
  sGpio.Pull  = GPIO_NOPULL;
  sGpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  u32 Items;
  
  S1_IO_RCC_EN();
  Items      = LXM;
  sGpio.Pin  = S1_PIN;
  sGpio.Mode = ((1 << Slct) & Items) ? GPIO_MODE_OUTPUT_PP : GPIO_MODE_INPUT;
  HAL_GPIO_Init(S1_IO_PORT, &sGpio);
  CLR_PINS(S1_IO_PORT, S1_PIN);

  S2_IO_RCC_EN();
  Items      = LXM+LRM+LVM+PVM+NVM;
  sGpio.Pin  = S2_PIN;
  sGpio.Mode = ((1 << Slct) & Items) ? GPIO_MODE_OUTPUT_PP : GPIO_MODE_INPUT;
  HAL_GPIO_Init(S2_IO_PORT, &sGpio);
  CLR_PINS(S2_IO_PORT, S2_PIN);
  
  S3_IO_RCC_EN();
  Items      = CXM+FXM+FHM+LXM+LRM;
  sGpio.Pin  = S3_PIN;
  sGpio.Mode = ((1 << Slct) & Items) ? GPIO_MODE_OUTPUT_PP : GPIO_MODE_INPUT;
  HAL_GPIO_Init(S3_IO_PORT, &sGpio);
  SET_PINS(S3_IO_PORT, S3_PIN);
  
  S4_IO_RCC_EN();
  Items      = NVM+LXM+LRM;
  sGpio.Pin  = S4_PIN;
  sGpio.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(S4_IO_PORT, &sGpio);
  if((1 << Slct) & Items) SET_PINS(S4_IO_PORT, S4_PIN);
  else              CLR_PINS(S4_IO_PORT, S4_PIN);
  
  S5_IO_RCC_EN();
  Items      = CXM+LVM+PVM+NVM+RLM+RMM+RHM+POM+AOM;
  sGpio.Pin  = S5_PIN;
  HAL_GPIO_Init(S5_IO_PORT, &sGpio);
  if((1 << Slct) & Items) SET_PINS(S5_IO_PORT, S5_PIN);
  else                    CLR_PINS(S5_IO_PORT, S5_PIN);
  
  S6_IO_RCC_EN();
  Items      = FXM+FHM+CXM+LXM+LRM;
  sGpio.Pin  = S6_PIN;
  sGpio.Mode = ((1 << Slct) & Items) ? GPIO_MODE_OUTPUT_PP : GPIO_MODE_INPUT;
  HAL_GPIO_Init(S6_IO_PORT, &sGpio);
  CLR_PINS(S6_IO_PORT, S6_PIN);
  
  VIN_IO_RCC_EN();
  sGpio.Pin  = VIN_PIN;
  sGpio.Pull = GPIO_PULLDOWN;
  sGpio.Mode = GPIO_MODE_INPUT;
  HAL_GPIO_Init(VIN_IO_PORT, &sGpio);
}
/*******************************************************************************

*******************************************************************************/
void USB_IO_PinsCnfg(u8 Slct)
{
  GPIO_InitTypeDef  sGpio = {0};
  
  USB_IO_RCC_EN();
  sGpio.Pin       = DP_PIN | DM_PIN;
  sGpio.Pull      = GPIO_NOPULL;
  sGpio.Alternate = GPIO_AF10_USB_FS;
  sGpio.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  sGpio.Mode      = (Slct == ENABLE) ? GPIO_MODE_AF_PP : GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(USB_IO_PORT, &sGpio);
  CLR_PINS(USB_IO_PORT, DP_PIN | DM_PIN);
}
/******************************** END OF FILE *********************************/
