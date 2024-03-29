/*******************************************************************************
  @file           : Measure.c
  @brief          : Measure Operation Subroutine  of the Application
*******************************************************************************/
#include "Bios.h"
#include "Measure.h"

//===========+======+======+======+
uc8 L[3][4] = {"uH ", "mH ", "H  "};
uc8 C[3][4] = {"pF ", "nF ", "uF "};
uc8 R[3][4] = {"R  ", "K  ", "M  "};
uc8 F[3][4] = {"Hz ", "KHz", "MHz"};
uc8 T[3][4] = {"nS ", "uS ", "mS "};
uc8 V[3][4] = {"mV ", "V  ", "KV "};
//===========+======+======+======+

uc8 MEASU_STR[][9] = 
{
  "Vx:", "Rx:", "Dx:", "Cx:", "Lx:", "Fx:",
  "Pusles", "Noises", "SineWave", "UserDef",
};

u16 UserDef[200] =   //样本波形描述
{
  1000*2, 1160*2, 1300*2, 1440*2, 1570*2, 1670*2, 1750*2, 1810*2, 1850*2, 1850*2, 
  1840*2, 1790*2, 1730*2, 1640*2, 1530*2, 1410*2, 1280*2, 1140*2, 1000*2,  860*2, 
   730*2,  600*2,  490*2,  400*2,  330*2,  270*2,  240*2,  240*2,  250*2,  290*2, 
   350*2,  430*2,  520*2,  630*2,  750*2,  870*2, 1000*2, 1120*2, 1240*2, 1350*2, 
  1450*2, 1530*2, 1600*2, 1640*2, 1670*2, 1670*2, 1660*2, 1630*2, 1570*2, 1500*2, 
  1420*2, 1330*2, 1220*2, 1110*2, 1000*2,  890*2,  790*2,  700*2,  610*2,  540*2, 
   480*2,  440*2,  420*2,  420*2,  430*2,  460*2,  500*2,  570*2,  640*2,  720*2,  
   810*2,  900*2, 1000*2, 1090*2, 1180*2, 1260*2, 1330*2, 1390*2, 1440*2, 1470*2, 
  1490*2, 1490*2, 1480*2, 1460*2, 1420*2, 1370*2, 1300*2, 1240*2, 1160*2, 1080*2, 
  1000*2,  930*2,  850*2,  790*2,  730*2,  680*2,  640*2,  610*2,  600*2,  600*2, 
   610*2,  630*2,  660*2,  700*2,  750*2,  810*2,  870*2,  930*2, 1000*2, 1060*2, 
  1120*2, 1170*2, 1220*2, 1250*2, 1280*2, 1300*2, 1310*2, 1310*2, 1310*2, 1290*2, 
  1260*2, 1230*2, 1190*2, 1150*2, 1100*2, 1050*2, 1000*2,  960*2,  910*2,  880*2,  
   840*2,  810*2,  790*2,  780*2,  770*2,  780*2,  780*2,  800*2,  820*2,  840*2, 
   870*2,  900*2,  930*2,  970*2, 1000*2, 1030*2, 1060*2, 1080*2, 1100*2, 1120*2, 
  1130*2, 1140*2, 1140*2, 1130*2, 1130*2, 1120*2, 1100*2, 1090*2, 1070*2, 1050*2, 
  1040*2, 1020*2, 1000*2,  990*2,  970*2,  960*2,  960*2,  950*2,  950*2,  950*2,  
   950*2,  960*2,  960*2,  970*2,  970*2,  980*2,  990*2,  990*2, 1000*2, 1000*2, 
  1000*2, 1000*2, 1000*2, 1000*2, 1000*2, 1000*2, 1000*2, 1000*2, 1000*2, 1000*2, 
  1000*2, 1000*2, 1000*2, 1000*2, 1000*2, 1000*2, 1000*2, 1000*2, 1000*2, 1000*2, 
};
//===========+=======+=======+=======+=======+=======+=======+=======+=======+
//  误差校正 |   +V  |   mV  |   -V  |   RH  |   RM  |   RL  |   CX  |   LX  |
//-----------+-------+-------+-------+-------+-------+-------+-------+-------+
float K0[] = {  1.800,  1.800,  0.000,  0.000,  3.400,  3.400,  284.5,  0.000};
float K1[] = {  11.03,  1.320,  1.000,  1.000,  1.020,  1.020,  1.000,  1.000};
//===========+=======+=======+=======+=======+=======+=======+=======+=======+

//===========+=======+=======+=======+=======+=======+=======+=======+=======+
//  误差校正 |  0pF  |  10pF |  .1nF |  1nF  |  10nF |  .1uF |  1uF  |  10uF |
//-----------+-------+-------+-------+-------+-------+-------+-------+-------+
float Ct[] = {2.85e+2,3.12e+2,4.89e+2,1.56e+3,1.27e+4,1.22e+5,1.21e+6,1.18e+7};
float Cs[] = {0.00e+0,9.50e+0,9.97e+1,9.84e+2,1.00e+4,9.89e+4,9.87e+5,9.74e+6};
float Cm[] = {0.98e-3,5.62e-4,16.1e-5,0.00e-6,0.00e-7,0.00e-8,0.00e-9,0.00e-9};
float Cn[] = {3.20e-1,4.10e-1,6.50e-1,8.07e-1,8.12e-1,8.09e-1,8.28e-1,8.32e-1};
float Cp[] = {0.61e+0,2.80e+1,2.05e+2,1.28e+3,1.25e+4,1.22e+5,1.22e+6,1.18e+7};
//===========+=======+=======+=======+=======+=======+=======+=======+=======+

//===========+=======+=======+=======+=======+=======+=======+
//  误差校正 |  uH   | 10uH  | .1mH  |  1mH  | 10mH  | 100mH |
//-----------+-------+-------+-------+-------+-------+-------+
float Lt[] = {6.23e+2,1.79e+3,5.56e+3,1.20e+4}; 
float Ls[] = {1.10e+0,9.80e+0,8.67e+1,8.72e+2};
float Lm[] = {8.40e-6,26.5e-6,37.7e-6,28.9e-6};  
float Ln[] = {27.0e-3,35.0e-3,66.0e-3,32.0e-2};  
float Lp[] = {3.28e+2,6.23e+2,1.79e+3,5.56e+3}; 
//===========+=======+=======+=======+=======+=======+=======+

//=============+=======+=======+=======+=======+
//             | Pulse | Noise |  Sin  | User  |
//-------------+-------+-------+-------+-------+
u16 PreScl[] = {     4,      0,      0,      0,};
u16 Period[] = {    99,    159,    159,     33,};
u16 Twidth[] = {    50,      0,      0,      0,};
//=============+=======+=======+=======+=======+

u16 SleepTc = 10*60; // 自动关机时长 @ 秒
u16 Contrst = 2;     // 显示亮度 0~10

float Vpwr, Vadc, Vdda, Vref;//

/*******************************************************************************

*******************************************************************************/
float FloatAbs(float n)
{
  if(n < 0) n = -n;
  return n; 
}
/*******************************************************************************
  浮点数转千进位带小数点 3 位有效数字+量纲字符串                   Units[3][4]
*******************************************************************************/
void FloatTo3P1Str(char *pStr, float n, u8 *Units)
{
  float m = n/1000000+0.0005;
  float k = n/1000+0.0005;
  n = n+0.0005;
  if(m >= 1)
  {
    sprintf(pStr, "%4.2f", m);
    strcpy(&pStr[4], (char*)&Units[2*4]);
  }
  else if(k >= 100) 
  {
    sprintf(pStr, "%4.0f", k);
    strcpy(&pStr[4], (char*)&Units[1*4]);
  }
  else if(k >= 10) 
  {
    sprintf(pStr, "%4.1f", k);
    strcpy(&pStr[4], (char*)&Units[1*4]);
  }
  else if(k >= 1) 
  {
    sprintf(pStr, "%4.2f", k);
    strcpy(&pStr[4], (char*)&Units[1*4]);
  }
  else if(n >= 100) 
  {
    sprintf(pStr, "%4.0f", n);
    strcpy(&pStr[4], (char*)&Units[0*4]);
  }
  else                    
  {
    sprintf(pStr, "%4.1f", n);
    strcpy(&pStr[4], (char*)&Units[0*4]);
  }
}
/*******************************************************************************
  
*******************************************************************************/
void MeasuSlctUpdt(u16 Slct)
{
  CtrlPinsCnfg(Slct);
  Cpt_Fh_TimerCnfg(Slct);
  if((1 << Slct) & CXM+LXM+LRM+FXM)
  {
    if(Wx[Slct] < 20*1000) CaptureEnbl();    // Fx < 20KHz 开始测周期
    else                   CaptureDsbl();
    CntClkCnfg((Wx[Slct] < 100000) ? 1 : 0); // Fx < 1MHz Filter = 1
  }
  
  Angl_Trig_TimerCnfg(Slct, 1-1, 160-1); // 设置较低触发频率以便更改 DAC
  COMP_DevCnfg(Slct);
  OPAMP_DevCnfg(Slct);
  if(Mode < PUSLES)   DAC_Cnfg(Slct,   0, (u16*)SINE100);
  if(Mode == NOISES)  DAC_Cnfg(Slct, 100, (u16*)SINE100);
  if(Mode == SINEWAV) DAC_Cnfg(Slct, 100, (u16*)SINE100);
  if(Mode == USERDEF) DAC_Cnfg(Slct, 200, (u16*)UserDef);
  PulseGen_TimerCnfg(Slct, PreScl[0], Period[0], Twidth[0]);
  u16 Ttc = Period[Mode-PUSLES]; 
  u16 Psc = PreScl[Mode-PUSLES]; 
  Angl_Trig_TimerCnfg(Slct, Psc, Ttc);   // 重新设置正确的触发频率
  
}
                
/******************************** END OF FILE *********************************/
