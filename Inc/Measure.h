/*******************************************************************************
  @file           : Measure.h
  @brief          : Header for Measure.c file.
*******************************************************************************/
#ifndef __MEASURE_H
#define __MEASURE_H

//  #define OVER                 9876543210.0
  #define OVER                 199.9*1000*1000

  typedef struct{
    float Value;
    u8*   pUnit;
  } combo;

  extern float Vpwr, Vadc, Vdda, Vref;//, Vcal
  extern uc8 L[3][4], C[3][4], R[3][4], F[3][4], T[3][4], V[3][4];
  
  float FloatAbs(float n);
  void FloatTo3P1Str(char *pStr, float n, u8 *Units);
  void MeasuSlctUpdt(u16 Slct);

#endif /* __MEASURE_H */

/******************************** END OF FILE *********************************/
