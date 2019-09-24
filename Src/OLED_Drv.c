/*******************************************************************************
  Brief: 加载在 BIOS 中的 OLED 驱动程序子模块                    Author : bure
*******************************************************************************/

#define SEG_INVR   0xA0
#define SEG_NORM   0xA1
#define COM_INVR   0xC0
#define COM_NORM   0xC8

static uc8 INIT_PARAM[] =
{
  #ifdef OLED_CTRL_SSD1316 // 128*32
//  SEG_NORM,//SEG_INVR,    // Set Segment Re-map            (SEG_INVR 左右反置)
//  COM_INVR,//COM_NORM,    // Set COM Output Scan Direction (COM_INVR 上下反置)
//  0x81, 0xF0,  // Set Contrast Control          (亮度调节 0~255) 
    0xAE,        // Display OFF
    0xA8, 0x1F,  // Set Multiplex Ratio           (1~64) 
    0x00,        // Set Lower Column Start Address for Page Addressing Mode
    0x10,        // Set Higher Column Start Address for Page Addressing Mode
    0x40,        // Set Display Start Line
    0xB0,        // Set Page Start Address 
    0xA6,        // Set Normal Display
    0xD3, 0x00,  // Set Display Offset
    0xD5, 0x80,  // Set Display Clock Divide Ratio/Oscillator Frequency
    0xD9, 0x22,  // Set Pre-charge Period
    0xDA, 0x12,  // Set COM Pins Hardware Configuration 
    0xDB, 0x40,  // Set VCOMH Deselect Level 
    0x8D,        // Set DC-DC Charging Pump Enable
    0x14,        // Set Higher Column Start Address for Page Addressing Mode
    0xAF,        // Display ON
  #endif
  #ifdef OLED_CTRL_SSD1306 // 96*16
//  SEG_NORM,    // Set Segment Re-map            (SEG_INVR 左右反置)
//  COM_NORM,    // Set COM Output Scan Direction (COM_INVR 上下反置)
//  0x81, 0xFF,  // Set Contrast Control          (亮度调节 0~255) 
    0xAE,        // Display OFF
    0xA8, 0x0f,  // Set Multiplex Ratio           (1~64)
    0xD5, 0x52,  // Set Display Clock Divide Ratio/Oscillator Frequency
    0xD3, 0x00,  // Set Display Offset
    0x40,        // Set Display Start Line
    0x8D,        // Set DC-DC Charging Pump Enable
    0x14,        // Set Higher Column Start Address for Page Addressing Mode
    0xDA, 0x02,  // Set COM Pins Hardware Configuration 
    0xD9, 0xF1,  // Set Pre-charge Period
    0xDB, 0x30,  // Set VCOMH Deselect Level 
    0xA4,        // Resume to RAM content display
    0xA6,        // Set Normal Display
    0xAF,        // Display ON
  #endif
  #ifdef OLED_CTRL_SSD128x64
//  SEG_NORM,    // Set Segment Re-map            (SEG_INVR 左右反置)
//  COM_NORM,    // Set COM Output Scan Direction (COM_INVR 上下反置)
//  0x81, 0xFF,  // set contrast control register (亮度调节 0~255)
    0xAE,        // Display OFF
    0xA8, 0x3F,  // set multiplex ratio           (1 to 64)
    0x00,        // Set Lower Column Start Address for Page Addressing Mode
    0x10,        // Set Higher Column Start Address for Page Addressing Mode
    0x20, 0x10,  // Set Memory Addressing Mode(00: Horizontal, 01: Vertical, 10: Page, 11: Invalid)  
    0xB0,        // Set Page Start Address for Page Addressing Mode, 0-7
    0x40,        // set start line address
    0xA6,        // Set Normal Display
    0xA4,        // 0xA4: Output follows RAM content, 0xA5: Output ignores RAM content
    0xD3, 0x00,  // set display offset (not offset)
    0xD5, 0xF0,  // set display clock divide ratio/oscillator frequency
    0xD9, 0x22,  // Set pre-charge period
    0xDA, 0x12,  // Set COM pins hardware configuration
    0xDB, 0x20,  // Set VCOMH (0.77xVcc)
    0x8D,        // Set DC-DC Charging Pump Enable
    0x14,        // Set Higher Column Start Address for Page Addressing Mode
    0xAF,        // Display ON
  #endif
};

/*******************************************************************************
  Display OFF (Sleep mode) 
*******************************************************************************/
void OLED_DispOff(void)
{
  I2C_WrByte(OLED_DEV_ID, 0x00, 0x8D); // Set DC-DC Charging Pump Enable 
  I2C_WrByte(OLED_DEV_ID, 0x00, 0x10);
  I2C_WrByte(OLED_DEV_ID, 0x00, 0xAE); // Display OFF
}
/*******************************************************************************
  Display ON (Active mode) 
*******************************************************************************/
void OLED_DispOn(void)
{
  I2C_WrByte(OLED_DEV_ID, 0x00, 0x8D); // Set DC-DC Charging Pump Enable 
  I2C_WrByte(OLED_DEV_ID, 0x00, 0x14); // Set Higher Column Start Address for Page Addressing Mode
  I2C_WrByte(OLED_DEV_ID, 0x00, 0xAF); // Display OFF
}
/*******************************************************************************
  OLED 初始化设置
*******************************************************************************/
void OLED_Direction(u16 Direction)
{
  I2C_WrByte(OLED_DEV_ID, 0x00, (Direction & 1) ? SEG_INVR : SEG_NORM);
  I2C_WrByte(OLED_DEV_ID, 0x00, (Direction & 2) ? COM_INVR : COM_NORM);
}
/*******************************************************************************
  OLED 初始化设置
*******************************************************************************/
void OLED_DevInit(u16 Contrast)
{
//  OLED_RST();
//  Dly_mS(2); 
//  OLED_ACT();
//  Dly_mS(20);
  I2C_WrByte(OLED_DEV_ID, 0x00, 0x81);
  I2C_WrByte(OLED_DEV_ID, 0x00, Contrast); // Contrast Control(亮度调节 0~255) 
  
  u8* p = (u8*)INIT_PARAM;
  u16 n = sizeof(INIT_PARAM);
  for(u32 i = 0; i <= n; i++) I2C_WrByte(OLED_DEV_ID, 0x00, *p++);
}
/*******************************************************************************
  设置显示位置  x0: 0~127, y0: 0~7  注: Oled96x32 Seg Remap 时 x0 从 32 开始
*******************************************************************************/
void OLED_SetPosn(u8 x, u8 y)
{
  #ifdef OLED96x32_SEG_INVR
    x += 32;
  #endif
  I2C_WrByte(OLED_DEV_ID, 0x00, 0xB0+y);
  I2C_WrByte(OLED_DEV_ID, 0x00, x/16+0x10);
  I2C_WrByte(OLED_DEV_ID, 0x00, x & 0x0F);
}
/*******************************************************************************
  全屏填充
*******************************************************************************/
void OLED_Fill(u8 Data)
{
  for(u32 y = 0; y < 8; y++)
  {
    OLED_SetPosn(0, y);
    for(u32 x = 0; x < 128; x++) I2C_WrByte(OLED_DEV_ID, 0x40, Data);
  }
}
/*******************************************************************************
  起始点坐标  x0: 0~127, y0: 0~7
*******************************************************************************/
void DispStr6x8(u8 x0, u8 y0, char* Str)
{
  OLED_SetPosn(x0, y0);
  while(*Str != 0)
  {
    u8* Pixl = (u8*)&FONT_6x8[(*Str -' ')*6]; 
    for(u32 Col = 0; Col < 6; Col++) I2C_WrByte(OLED_DEV_ID, 0x40, Pixl[Col]);
    Str++;
  }
}
/*******************************************************************************
  起始点坐标  x0: 0~127, y0: 0~7
*******************************************************************************/
void DispStr8x16(u8 x0, u8 y0, char* Str)
{
  while(*Str != 0)
  {
    u8* Pixl = (u8*)&FONT_8x16[(*Str -' ')*16]; 
    OLED_SetPosn(x0, y0);
    for(u32 Col = 0; Col < 8;  Col++) I2C_WrByte(OLED_DEV_ID, 0x40, Pixl[Col]);
    OLED_SetPosn(x0, y0+1);
    for(u32 Col = 8; Col < 16; Col++) I2C_WrByte(OLED_DEV_ID, 0x40, Pixl[Col]);
    Str++;
    x0 += 8;
  }
}
/******************************** END OF FILE *********************************/
