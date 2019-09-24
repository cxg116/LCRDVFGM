/*******************************************************************************
  Brief: 加载在 BIOS 中的 Accelerometer 驱动程序子模块           Author : bure
*******************************************************************************/


uc8 LIS_ACCEL_INIT_PARAM[] =
{
  ACCEL_CTRL_REG1, 0x27, // 10Hz, ZEN, YEN, XEN
  ACCEL_CTRL_REG2, 0x80, // NormaHighpass filter
  ACCEL_CTRL_REG3, 0x00, // Disable interrupt
  ACCEL_CTRL_REG4, 0x88, // BDU, FS=2g, HR
  ACCEL_CTRL_REG5, 0x00, // FIFO_EN, 
  ACCEL_CTRL_REG6, 0x00, // 
  ACCEL_TEMP_CNFG, 0xC0, // Temperature sensor enable
};

/*******************************************************************************
  ACCEL_DevInit（加速度计初始化设置）
*******************************************************************************/
void ACCEL_DevInit(void)
{
  for(u32 i = 0; i <= sizeof(LIS_ACCEL_INIT_PARAM); i += 2)
  {
    u32 Addr = LIS_ACCEL_INIT_PARAM[i];
    u32 Data = LIS_ACCEL_INIT_PARAM[i+1];
    I2C_WrByte(ACCEL_DEV_ADDR, Addr, Data);
  }
}
/*******************************************************************************
  ACCEL_Get_Data （读取加速度计 X,Y,Z 的方向值）
*******************************************************************************/
void ACCEL_Get_Data(s16 *pData)
{
  pData[3] = pData[0];
  pData[4] = pData[1];
  pData[5] = pData[2];
  *((u8*)&pData[0]+0) = I2C_RdByte(ACCEL_DEV_ADDR, ACCEL_OUT_X_L_REG);
  *((u8*)&pData[0]+1) = I2C_RdByte(ACCEL_DEV_ADDR, ACCEL_OUT_X_H_REG);
  *((u8*)&pData[1]+0) = I2C_RdByte(ACCEL_DEV_ADDR, ACCEL_OUT_Y_L_REG);
  *((u8*)&pData[1]+1) = I2C_RdByte(ACCEL_DEV_ADDR, ACCEL_OUT_Y_H_REG);
  *((u8*)&pData[2]+0) = I2C_RdByte(ACCEL_DEV_ADDR, ACCEL_OUT_Z_L_REG);
  *((u8*)&pData[2]+1) = I2C_RdByte(ACCEL_DEV_ADDR, ACCEL_OUT_Z_H_REG);
  pData[0] = pData[0]/16;
  pData[1] = pData[1]/16;
  pData[2] = pData[2]/16;
  pData[6] = abs(pData[0]-pData[3]);
  pData[7] = abs(pData[1]-pData[4]);
  pData[8] = abs(pData[2]-pData[5]);
  *((u8*)&pData[9]+0) = I2C_RdByte(ACCEL_DEV_ADDR, ACCEL_TEMP_L_REG);
  *((u8*)&pData[9]+1) = I2C_RdByte(ACCEL_DEV_ADDR, ACCEL_TEMP_H_REG);
  pData[9] = pData[9]/64;
} 

/******************************** END OF FILE *********************************/
