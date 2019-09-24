/*******************************************************************************
  File Name   : AppMain.c
  Description : 
*******************************************************************************/
#include "Bios.h"

typedef void (*pFunc)(void);

USBD_HandleTypeDef USBD_Device;
extern PCD_HandleTypeDef hpcd;
extern void DfuDiskInit(void);

s16 Dxyz[10];

/*******************************************************************************

*******************************************************************************/
void main(void)
{
  char Str[32] = {0};
  char St0[32] = {0}, St1[32] = {0}, St2[32] = {0}, St3[32] = {0};

  HAL_Init();                        // 1.90mA
  SysClkCnfg(APP);                   // 2.53mA
  ADC_DevInit();                     // 4.20mA
  I2C_DevInit();                     // 3.04mA
//  u32 Contrast = 32;                 // 读取配置参数中 OLED 的显示亮度: 0~255
  OLED_DevInit(10+Contrst*10);            // 清屏幕
  OLED_Direction(1);                 // 设置 OLED 的显示方向: 0~3
  OLED_Fill(0x00);                   // 3.45mA
  OLED_DispOn();                     // 3.66mA
/*  
  float Vpwr = AdcVlue[VPWR]*1.465;
  if(Vpwr > 4300) while(1){CtrlPinsCnfg(NC);};    // 调试用！等待进入编程状态
//  if(Vpwr < 3000)      PowerDown();      // Vpwr < 3000mV Into Power Down Mode
//  else if(Vpwr > 4300) DFU_Handler();    // Vpwr > 4300mV Into DFU Mode
//  else                 APP_Handler();    // else Into APP Mode 
//  if(VIN_ST) DFU_Handler();              // Vpwr > 4300mV Into DFU Mode
//  else       APP_Handler();              // else Into APP Mode 
*/
  // =================== APP Init ====================   
  SysClkCnfg(APP);
  TSC_DevInit();
  RTC_DevInit();
  CalbrTimerInit();
  CounterTimerInit();
  ACCEL_DevInit();
  MeasuSlctUpdt(Item);                          // 重新配置工作模式
  TSCxKey = 0;
  Pcnt  = 0;
  // =================== APP Loop ====================    
  while(1)
  {
    HAL_Delay(5);
    if(Pcnt > SleepTc*1000) PowerDown();       // 10分钟无操作则关机
    if(TSCxKey)
    {                                          // 触摸按键处理
      if(++Mode > USERDEF) Mode = MEASU_V;     // 测量模式循环 NOISES
      OLED_Fill(0x00);                         // 清屏幕
      Pcnt  = 0;
      TSCxKey = 0;
       
      if(Mode == PUSLES) Item = PO;
      if(Mode >  PUSLES) Item = AO;
      if(Mode >= PUSLES) MeasuSlctUpdt(Item);  // 配置信号输出模式
      DispUpd = 1;
    }
    if(Mode > USERDEF) Mode = MEASU_V;
    if(ACCEL_ZYX_ACTV)                         // 检测加速度有无变化
    {
      DispUpd = 1;                             // 加速度有变化则置显示刷新标志
      ACCEL_Get_Data(Dxyz);                    // 读取 3 轴加速度信息
      if((Dxyz[0]^Dxyz[3]) >> 15)              // x 轴加速度方向有改变
      {
        OLED_Direction((Dxyz[0] > 0) ? 1 : 2); // 根据手持方位重新设置显示方向
        Dxyz[3] = Dxyz[0];
        OLED_Fill(0x00);                       // 清屏幕
      }
    }
    if(DispUpd)
    {                                          // 刷新显示信息
      DispUpd = 0;
      float f, t;
      strcpy(Str, (char*)&MEASU_STR[Mode][0]); // 显示测量种类
      if(Mode == MEASU_F)
      {
        f = Wx[((Wx[FH] > 4000*1000 || !Wx[FX+6]) ? FH : FX)]/FxSCALE; // @ Hz
        t = Wx[FX+6]*FxSCALE;                                          // @ nS 
        if(Wx[FX] < 20*1000 && Wx[FX+6]) f = (1e+9)/t;
        FloatTo3P1Str(&Str[3], f, (u8*)F);
        
//        St0[0] = 0;
//        strcpy(&St1[0], "Fh:"); FloatTo3P1Str(&St1[3], Wx[FH]  /FxSCALE, (u8*)F);
//        strcpy(&St2[0], "Fl:"); FloatTo3P1Str(&St2[3], Wx[FX]  /FxSCALE, (u8*)F);
//        strcpy(&St3[0], "Ft:"); FloatTo3P1Str(&St3[3], Wx[FX+6]/FxSCALE, (u8*)T);
      }
      if(Mode == MEASU_V)
      {
        u32 n = (Wx[NV]) ? NV : (Wx[LV] < 4000) ? LV : PV;
        float Vx = Wx[n]*((n == PV) ? VpSCALE : VxSCALE)/10;
        strcpy(&Str[3], ((n == NV) ? "-" : " "));
        if(Wx[n] > 4000) strcpy(&Str[4], "Over! ");
        else FloatTo3P1Str(&Str[4], ((Vx < 0.1) ? 0 : K0[n])+K1[n]*Vx, (u8*)V);
        
//        strcpy(&St0[0], "           "); 
//        float VxN = Wx[NV]*VxSCALE/10;
//        strcpy(&St1[0], "VN:"); FloatTo3P1Str(&St1[3], ((VxN < 0.1) ? 0 : K0[NV])+K1[NV]*VxN, (u8*)V);
//        float VxL = Wx[LV]*VxSCALE/10;
//        strcpy(&St2[0], "VL:"); FloatTo3P1Str(&St2[3], ((VxL < 0.1) ? 0 : K0[LV])+K1[LV]*VxL, (u8*)V);
//        float VxP = Wx[PV]*VpSCALE/10;
//        strcpy(&St3[0], "VP:"); FloatTo3P1Str(&St3[3], ((VxP < 0.1) ? 0 : K0[PV])+K1[PV]*VxP, (u8*)V);
      }
      if(Mode == MEASU_R)
      {
        u32 n = (Wx[RL] < 4000) ? RL : (Wx[RM] < 4000) ? RM : RH;
        float Vx = Wx[n]*VxSCALE/((n == RL) ? 8 : 1);
        float Vk = (4096*VxSCALE-Vx > 0) ? 4096*VxSCALE-Vx : 0.1;
        float Rk = ((n == RH) ? R3 : 0)+K0[n];
        float Rh = (n == RH) ? R5 : R6;
        float Rx = K1[n]*Vx*Rh/Vk-Rk;
        if(Wx[NV] != 0 || Wx[PV] != 0) strcpy(&Str[3], " Volt! ");
        else if(Wx[RH] > 4000)         strcpy(&Str[3], " - - - ");
        else FloatTo3P1Str(&Str[3], (Rx > 0.1) ? Rx : 0, (u8*)R);
        
//        strcpy(&St0[0], (Wx[NV] != 0) ? "-Volt" : (Wx[PV] != 0) ? "+Volt" : "     "); 
//        float VxL = Wx[RL]*VxSCALE/8, VkL = (4096*VxSCALE-VxL > 0) ? 4096*VxSCALE-VxL : 0.1, RxL = K1[RL]*VxL*R6/VkL-K0[RL];
//        strcpy(&St1[0], "RL:"); FloatTo3P1Str(&St1[3], (RxL > 0.1) ? RxL : 0, (u8*)R);
//        float VxM = Wx[RM]*VxSCALE/1, VkM = (4096*VxSCALE-VxM > 0) ? 4096*VxSCALE-VxM : 0.1, RxM = K1[RM]*VxM*R6/VkM-K0[RM];
//        strcpy(&St2[0], "RM:"); FloatTo3P1Str(&St2[3], (RxM > 0.1) ? RxM : 0, (u8*)R);
//        float VxH = Wx[RH]*VxSCALE/1, VkH = (4096*VxSCALE-VxH > 0) ? 4096*VxSCALE-VxH : 0.1, RxH = K1[RH]*VxH*R5/VkH-K0[RH]-R3;
//        strcpy(&St3[0], "RH:"); FloatTo3P1Str(&St3[3], (RxH > 0.1) ? RxH : 0, (u8*)R);
      }
      if(Mode == MEASU_D)
      {
        float Vx = Wx[RM]*VpSCALE;
        if(Wx[NV] != 0 || Wx[PV] != 0) strcpy(&Str[3], " Volt! ");
        else if(Wx[RM] > 4000)         strcpy(&Str[3], " - - - ");
        else if(Wx[RM]/Wx[RH] > 4)     strcpy(&Str[3], " ? ? ? ");
        else FloatTo3P1Str(&Str[3], Vx, (u8*)V);
        
//        strcpy(&St0[0], "DvM:"); u16To4DecStr(&St0[4], Wx[RL]); strcpy(&St1[0], "DvH:"); u16To4DecStr(&St1[4], Wx[RH]); 
//        float VxM = Wx[RM]*VxSCALE/1, VkM = (4096*VxSCALE-VxM > 0) ? 4096*VxSCALE-VxM : 0.1, RxM = K1[RM]*VxM*R6/VkM-K0[RM];
//        strcpy(&St2[0], "RM:"); FloatTo3P1Str(&St2[3], (RxM > 0.1) ? RxM : 0, (u8*)R);
//        float VxH = Wx[RH]*VxSCALE/1, VkH = (4096*VxSCALE-VxH > 0) ? 4096*VxSCALE-VxH : 0.1, RxH = K1[RH]*VxH*R5/VkH-K0[RH]-R3;
//        strcpy(&St3[0], "RH:"); FloatTo3P1Str(&St3[3], (RxH > 0.1) ? RxH : 0, (u8*)R);
      }
      if(Mode == MEASU_C)
      {
        f = (Wx[CX] == 0) ? 0.1 : Wx[CX]*FxSCALE;   // @ Hz
        t = Wx[CX+6]*FxSCALE;                       // @ nS 
        float Xt = (Wx[CX] > 20000) ? (1e+9)/f : t;
        u32 e = 0;
        for(; e < 8; e++) if(Xt <= Ct[e]) break;    // 判断 Cx 档位       
        float Tm = Xt-(Cp[e]+K0[CX]);
        float Cx = Cm[e]*Tm*Tm+Cn[e]*Tm+Cs[e];
        if(Wx[RH] < 3724) strcpy(&Str[3], " ? ? ? "); // RH < 2M
        else FloatTo3P1Str(&Str[3], (Cx <= 0) ? 0 : Cx, (u8*)C);
        
//        strcpy(&St0[0], (Wx[FX]) ? "Pulse" : (Wx[NV] || Wx[PV]) ? "Volt!" : "     ");
//        float VxH = Wx[RH]*VxSCALE/1, VkH = (4096*VxSCALE-VxH > 0) ? 4096*VxSCALE-VxH : 0.1, RxH = K1[RH]*VxH*R5/VkH-K0[RH]-R3;
//        strcpy(&St1[0], "RH:"); FloatTo3P1Str(&St1[3], (RxH > 0.1) ? RxH : 0, (u8*)R);
//        strcpy(&St2[0], "Cf:"); FloatTo3P1Str(&St2[3], f, (u8*)F);
//        strcpy(&St3[0], "Ct:"); FloatTo3P1Str(&St3[3], t, (u8*)T);
      }
      if(Mode == MEASU_L)
      {
        float r = Wx[LR]*FxSCALE;
        f = (Wx[LX] == 0) ? 0.1 : Wx[LX]*FxSCALE;  // @ Hz
        t = Wx[LX+6]*FxSCALE;                      // @ nS 
        float Xt = (Wx[LX] > 20000) ? (1e+9)/f : t;
        u32 e = 0;
        for(; e < 4; e++) if(Xt <= Lt[e]) break;   // 判断 Lx 档位         
        float Tm = Xt-Lp[e];
        float Lx = Lm[e]*Tm*Tm+Ln[e]*Tm+Ls[e];
        if(Wx[NV] != 0 || Wx[PV] != 0) strcpy(&Str[3], " Volt! ");
        else if(FloatAbs(r-f)/f < 0.1) strcpy(&Str[3], " - - - ");
        else FloatTo3P1Str(&Str[3], (Lx <= 0) ? 0 : Lx, (u8*)L);
        
//        strcpy(&St0[0], "Lr:"); FloatTo3P1Str(&St0[3], r, (u8*)F);
//        float VxL = Wx[RL]*VxSCALE/8, VkL = (4096*VxSCALE-VxL > 0) ? 4096*VxSCALE-VxL : 0.1, RxL = K1[RL]*VxL*R6/VkL-K0[RL];
//        strcpy(&St1[0], "RL:"); FloatTo3P1Str(&St1[3], (RxL > 0.1) ? RxL : 0, (u8*)R);
//        strcpy(&St2[0], "Lf:"); FloatTo3P1Str(&St2[3], f, (u8*)F);
//        strcpy(&St3[0], "Lt:"); FloatTo3P1Str(&St3[3], t, (u8*)T);
      }
      DispStr8x16(20, 1, Str);
//      DispStr6x8(0, 0, St0); DispStr6x8(0, 0, St1);
//      DispStr6x8(0, 0, St2); DispStr6x8(0, 0, St3);
    }
  }
}
/******************************** END OF FILE *********************************/
