/********************* (C) COPYRIGHT 2019 e-Design Co.,Ltd. ********************
  Func.h Ver: 2.1                                        Author : bure & Kewei
*******************************************************************************/
#ifndef __FUNC_H__
#define __FUNC_H__

  typedef struct{
    u8 decPos;
    u8 len;
    u8 str[9];
  } I32STR_RES;

  #define SIGN         0
  #define UNSIGN       1
  #define STD          2

  void u16mV_to_V(u8 *p, u16 n);
  void Mem32Copy(u32* d, u32* s,   u32 n);
  void Mem32Fill(u32* d, u32 Word, u32 n);

  u32  HexStr2u32(char *pStr);
  u8   CharEqu(char *p, char *Str);
  void CharCpy(char *p, char *Str);
  void Rev32(u32 *pBuf);
  void Rev16(u16 *pBuf);
  u32  Power(u8 x, u8 y);
  u32  Exp(u8 x);
  u8*  SeekStr(u8* ptr, u8 Idx);
  u8   Char2Nib(u8 x);

//  void Int2Str(char *p, s32 n, uc8 *pUnit, u8 e, u8 Mode);
//void Int32String_sign(I32STR_RES *r, s32 n);
//void Int32String(I32STR_RES *r, u32 n, u32 e);
  void u32To3DecFoltStr(char *p, u32 n, uc8 *Unit);
  void Value2Str(u8 *p, s32 n, uc8 *pUnit, s8 e, u8 Mode);
  u8   Str2Byte(u8 x,u8 y);
  void u32To10DecStr(char *p, u32 n);
  void u16To5DecStr(char *p, u16 n);
  void s16To5DecStr(char *p, s16 n);
  void u16To4DecStr(char *p, u16 n);
  void u8To3DecStr (char *p, u8  n);
  void u8To2DecStr (char *p, u8  n);
  void u8To2HexStr (char *p, u8  n);
  void u16To4HexStr(char *p, u16 n);
  void u32To8HexStr(char *p, u32 n);
  u16  Sqrt32(u32 n);
  void s8ToPercen(char *p, s8 n);
  void dectostr(u8 *nStr,u8* decpos, u32 Num);
  void Int32String(I32STR_RES *r, u32 n, u32 e);
  void Int32String_sign(I32STR_RES *r, s32 n);

#endif
/********************************* END OF FILE ********************************/
