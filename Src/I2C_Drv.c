/*******************************************************************************
  Brief: 加载在 BIOS 中的 I2C 驱动程序子模块                    Author : bure
*******************************************************************************/

I2C_HandleTypeDef  hI2C;

/*******************************************************************************
  @brief I2C Initialization Function
*******************************************************************************/
void I2C_DevInit(void)
{
  GPIO_InitTypeDef sInitIO = {0};
  
  I2C_IO_RCC_EN();
  sInitIO.Speed                = GPIO_SPEED_HIGH;
  sInitIO.Pull                 = GPIO_PULLUP;
  #ifdef SIMUL_I2C
    sInitIO.Pin                = SCL_PIN;           
    sInitIO.Mode               = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(I2C_IO_PORT, &sInitIO);
    sInitIO.Pin                = SDA_PIN;           
    sInitIO.Mode               = GPIO_MODE_OUTPUT_OD;
    HAL_GPIO_Init(I2C_IO_PORT, &sInitIO);
  #else
    sInitIO.Alternate          = I2C_AF_MAP;
    sInitIO.Pin                = SCL_PIN;           
    sInitIO.Mode               = GPIO_MODE_AF_PP;
    HAL_GPIO_Init(I2C_IO_PORT, &sInitIO);
    sInitIO.Pin                = SDA_PIN;           
    sInitIO.Mode               = GPIO_MODE_AF_OD;
    HAL_GPIO_Init(I2C_IO_PORT, &sInitIO);
    
    I2C_DEV_RCC_EN();
    hI2C.Instance              = I2Cx;
    hI2C.Init.Timing           = 0x00303D5B;
    hI2C.Init.OwnAddress1      = 0;
    hI2C.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
    hI2C.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
    hI2C.Init.OwnAddress2      = 0;
    hI2C.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hI2C.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
    hI2C.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
    if(HAL_I2C_Init(&hI2C) != HAL_OK) ErrLoop();
  
    if(HAL_I2CEx_ConfigAnalogFilter(&hI2C, I2C_ANALOGFILTER_ENABLE) != HAL_OK) ErrLoop();

    if(HAL_I2CEx_ConfigDigitalFilter(&hI2C, 0) != HAL_OK) ErrLoop();
  #endif
}
/*******************************************************************************
函数名:   I2C_Mem_Wr （向 I2C 设备存储器写入 n 个字节的数据）
输入参数: DevAddr 为设备地址，WrAddr 为待写数据地址，pBuf 为待写数据存放区指针
返回参数: NULL
*******************************************************************************/
void I2C_Mem_Wr(u8 DevAddr, u8 WrAddr, u8 *pBuf, u8 n)
{
  HAL_I2C_Mem_Write(&hI2C, DevAddr, WrAddr, I2C_MEMADD_SIZE_8BIT, pBuf, n, 255);
}
/*******************************************************************************
函数名:   I2C_Mem_Wr （向 I2C 设备存储器写入 1 个字节的数据）
输入参数: DevAddr 为设备地址, WrAddr 为待写数据地址, Btye 为待写数据字节
返回参数: NULL
*******************************************************************************/
void I2C_WrByte(u8 DevAddr, u8 WrAddr, u8 Btye)
{
  #ifdef SIMUL_I2C
    Simul_I2C_Start();
    Simul_I2C_WrByte(DevAddr);
    Simul_I2C_WrByte(WrAddr);
    Simul_I2C_WrByte(Btye);
    Simul_I2C_Stop();
  #else
    HAL_I2C_Mem_Write(&hI2C, DevAddr, WrAddr, I2C_MEMADD_SIZE_8BIT, &Btye, 1, 255);
  #endif
}
/*******************************************************************************
函数名:   I2C_Mem_Rd （从 I2C 设备存储器读出 n 个字节数据）
输入参数: DevAddr 为设备地址, RdAddr 为待读数据地址, pBuf 为待读数据存放区指针
返回参数: NULL
*******************************************************************************/
void I2C_Mem_Rd(u8 DevAddr, u8 RdAddr, u8 *pBuf, u8 n)
{
  HAL_I2C_Mem_Read(&hI2C, DevAddr, RdAddr, I2C_MEMADD_SIZE_8BIT, pBuf, n, 255);
}
/*******************************************************************************
函数名:   I2C_RdByte （从 I2C 设备存储器读出 1 个字节的数据）
输入参数: DevAddr 为设备地址，RdAddr 为待读数据的地址
返回参数: 读出的字节数据
*******************************************************************************/
u8 I2C_RdByte(u8 DevAddr, u8 RdAddr)
{
  u8 Byte;
  HAL_I2C_Mem_Read(&hI2C, DevAddr, RdAddr, I2C_MEMADD_SIZE_8BIT, &Byte, 1, 255);
  return Byte;
}
/*******************************************************************************

*******************************************************************************/
void Simul_I2C_Start(void)
{
  SET_PINS(I2C_IO_PORT, SDA_PIN | SCL_PIN);
  Dly_uS(10);
  CLR_PINS(I2C_IO_PORT, SDA_PIN);//START: When CLK is High, DATA Change Form High to Low 
  Dly_uS(10);
  CLR_PINS(I2C_IO_PORT, SCL_PIN);
}	  
/*******************************************************************************

*******************************************************************************/
void Simul_I2C_Stop(void)
{
  CLR_PINS(I2C_IO_PORT, SDA_PIN | SCL_PIN);
  Dly_uS(10);
  SET_PINS(I2C_IO_PORT, SDA_PIN | SCL_PIN);
  Dly_uS(10);
}
/*******************************************************************************
  Return: 0 = Success
*******************************************************************************/
u32 Simul_I2C_WrByte(u8 Byte)
{
  for(u32 i = 0; i < 8; i++)
  {
    CLR_PINS(I2C_IO_PORT, SCL_PIN);
    Dly_uS(2);
    if(Byte & 0x80) SET_PINS(I2C_IO_PORT, SDA_PIN);
    else            CLR_PINS(I2C_IO_PORT, SDA_PIN);
    Dly_uS(2);
    Byte <<= 1;
    SET_PINS(I2C_IO_PORT, SCL_PIN);
    Dly_uS(2);
  }
  CLR_PINS(I2C_IO_PORT, SCL_PIN);
  Dly_uS(2);
  SET_PINS(I2C_IO_PORT, SDA_PIN);
  Dly_uS(2);
  u32 Ack = TST_PINS(I2C_IO_PORT, SDA_PIN);
  SET_PINS(I2C_IO_PORT, SCL_PIN);
  Dly_uS(2);
  return (Ack)? 1 : 0;
}
/******************************** END OF FILE *********************************/
