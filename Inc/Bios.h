/*******************************************************************************
  @file           : Bios.h
  @brief          : Header for Bios.c file.
*******************************************************************************/
#ifndef __BIOS_H
#define __BIOS_H

  #include "stm32l4xx_hal.h"

  typedef int32_t        s32;
  typedef int16_t        s16;
  typedef int8_t         s8;

  typedef const int32_t  sc32; /*!< Read Only */
  typedef const int16_t  sc16; /*!< Read Only */
  typedef const int8_t   sc8;  /*!< Read Only */

  typedef __IO int32_t   vs32;
  typedef __IO int16_t   vs16;
  typedef __IO int8_t    vs8;

  typedef __I int32_t    vsc32; /*!< Read Only */
  typedef __I int16_t    vsc16; /*!< Read Only */
  typedef __I int8_t     vsc8;  /*!< Read Only */

  typedef uint64_t       u64;
  typedef uint32_t       u32;
  typedef uint16_t       u16;
  typedef uint8_t        u8;

  typedef const uint32_t uc32;  /*!< Read Only */
  typedef const uint16_t uc16;  /*!< Read Only */
  typedef const uint8_t  uc8;   /*!< Read Only */

  typedef __IO uint32_t  vu32;
  typedef __IO uint16_t  vu16;
  typedef __IO uint8_t   vu8;

  typedef __I uint32_t   vuc32; /*!< Read Only */
  typedef __I uint16_t   vuc16; /*!< Read Only */
  typedef __I uint8_t    vuc8;  /*!< Read Only */

  #include "math.h"
  #include "Func.h"
  #include <stdlib.h>
  #include <string.h>
  #include "Measure.h"
  #include "stm32l4xx.h"
  #include "usb_device.h"
  #include "touchsensing.h"

  #ifdef __cplusplus
    extern "C" {
  #endif

  #define DELAYt              {asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP");\
                               asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP");}
    
  extern u16 Item, Mode, DispUpd;
  extern u32 Pcnt;
  
  void SetMainSP(u32 StackAddr);
  void PowerDown(void);
  void DFU_Handler(void);
  void APP_Handler(void);
  void SysClkCnfg(u16 Mode);
  void HAL_IncTick(void);
  void ErrLoop(void);
  void Dly_uS(u32 n);
  void CtrlPinsCnfg(u32 Slct);
      
//---------------------------- Measure Definition ------------------------------

//  #define VDDA                3026.0  // For #1
  #define VDDA                3016.0  // For #2

  #define R1                  10000  // @ ohm
  #define R2                  10000  // @ ohm
  #define R3                  10000  // @ ohm
  #define R4                  2000   // @ ohm
  #define R5                  200000 // @ ohm
  #define R6                  1000   // @ ohm
  #define C1                  1000   // @ pF
      
  #define FxSCALE             (CalbrSumT/(128*3906.0))
  #define VxSCALE             ((1.0+(VDDA-3000.0)/3000.0)*375.0/512.0)
  #define VpSCALE             (VxSCALE*((R5+R1)/(R3+R5+R1)))
      
  #define V_SCALE             1
  
  typedef enum 
  {
    PV, LV, NV, RH, RM, RL, CX, LX, FX, FH, LR, PO, AO, NC
  } measure_item;
  typedef enum 
  {
    MEASU_V, MEASU_R, MEASU_D, MEASU_C, MEASU_L, MEASU_F, 
    PUSLES,  NOISES,  SINEWAV, USERDEF, 
//    CALBR_0, CALBR_1, AUTOMOD, 
  } measure_mode;
  
  typedef enum 
  { 
    PVM = 1 << PV, LVM = 1 << LV, NVM = 1 << NV, RHM = 1 << RH, 
    RMM = 1 << RM, RLM = 1 << RL, CXM = 1 << CX, LXM = 1 << LX,
    FXM = 1 << FX, FHM = 1 << FH, LRM = 1 << LR, NCM = 1 << NC, 
    POM = 1 << PO, AOM = 1 << AO, 
  } item_mask;

  extern uc8 MEASU_STR[][9];
  extern float K0[], K1[];
  extern float Ct[], Cs[], Cm[], Cn[], Cp[], Lt[], Ls[], Lm[], Ln[], Lp[]; 
  extern u32 Wx[];
  extern u16 SleepTc, Contrst;
  
//--------------------------- SYS Ctrl Definition ------------------------------
  
  #define SET_PINS(Port, Pins) Port->BSRR = Pins
  #define CLR_PINS(Port, Pins) Port->BRR = Pins
  #define TST_PINS(Port, Pins) Port->IDR & Pins

  #define DFU                  0
  #define APP                  1
  
  #define S1_IO_RCC_EN()       __HAL_RCC_GPIOA_CLK_ENABLE();
  #define S1_IO_PORT           GPIOA
  #define S1_PIN               GPIO_PIN_6
  
  #define S2_IO_RCC_EN()       __HAL_RCC_GPIOA_CLK_ENABLE();
  #define S2_IO_PORT           GPIOA
  #define S2_PIN               GPIO_PIN_7
  
  #define S3_IO_RCC_EN()       __HAL_RCC_GPIOA_CLK_ENABLE();
  #define S3_IO_PORT           GPIOA
  #define S3_PIN               GPIO_PIN_2
  
  #define S4_IO_RCC_EN()       __HAL_RCC_GPIOB_CLK_ENABLE();
  #define S4_IO_PORT           GPIOB
  #define S4_PIN               GPIO_PIN_3
  
  #define S5_IO_RCC_EN()       __HAL_RCC_GPIOA_CLK_ENABLE();
  #define S5_IO_PORT           GPIOA
  #define S5_PIN               GPIO_PIN_15
  
  #define S6_IO_RCC_EN()       __HAL_RCC_GPIOB_CLK_ENABLE();
  #define S6_IO_PORT           GPIOB
  #define S6_PIN               GPIO_PIN_0
  
  #define WAKEUP_IO_RCC_EN()   __HAL_RCC_GPIOA_CLK_ENABLE();
  #define WAKEUP_IO_PORT       GPIOA
  #define WAKEUP_PIN           GPIO_PIN_0
  #define PWR_GPIO_PORT        PWR_GPIO_A
  #define PWR_WAKEUP_PIN       PWR_GPIO_BIT_0
  #define PWR_WAKEUP           PWR_WAKEUP_PIN1
  #define PWR_S5_PIN           PWR_GPIO_BIT_15
  #define WAKEUP_POLARITY      PWR_WAKEUP_PIN1_LOW  

  #define VIN_IO_RCC_EN()      __HAL_RCC_GPIOB_CLK_ENABLE();
  #define VIN_IO_PORT          GPIOB
  #define VIN_PIN              GPIO_PIN_1
  #define VIN_ST               TST_PINS(VIN_IO_PORT, VIN_PIN)
  
//---------------------------- I2C Dev Definition ------------------------------

  #define I2C_DEV_RCC_EN()     __HAL_RCC_I2C1_CLK_ENABLE()
  #define I2Cx                 I2C1
  #define I2C_IO_RCC_EN()      __HAL_RCC_GPIOA_CLK_ENABLE();
  #define I2C_IO_PORT          GPIOA
  #define I2C_AF_MAP           GPIO_AF4_I2C1
  #define SCL_PIN              GPIO_PIN_9
  #define SDA_PIN              GPIO_PIN_10
  
  void I2C_DevInit(void);
  void I2C_Mem_Wr(u8 DevAddr, u8 WrAddr, u8 *pBuf, u8 n);
  void I2C_Mem_Rd(u8 DevAddr, u8 RdAddr, u8 *pBuf, u8 n);
  void I2C_WrByte(u8 DevAddr, u8 WrAddr, u8 Btye);
  u8   I2C_RdByte(u8 DevAddr, u8 RdAddr);
  
//  #define SIMUL_I2C
  void Sim_I2C_IO_Init(void);
  void Simul_I2C_Start(void);
  void Simul_I2C_Stop(void);
  u32  Simul_I2C_WrByte(u8 Byte);

//------------------------- OLED Display Definition ----------------------------

//#define OLED96x32_SEG_INVR
  #define OLED_CTRL_SSD1316
  #define OLED_DEV_ID          0x78 
  
  void OLED_DevInit(u16 Contrast);
  void OLED_Direction(u16 Direction);
  void OLED_SetPosn(u8 x, u8 y);
  void OLED_Fill(u8 Data);
  void OLED_DispOn(void);
  void OLED_DispOff(void);
  void DispStr6x8(u8 x, u8 y, char* pStr);
  void DispStr8x16(u8 x0, u8 y0, char* Str);
    
//------------------------- Accelerometer Definition ---------------------------

  #define ACCEL_DEV_ADDR       0x32
  #define ACCEL_STATUS_REG     0x27
  #define ACCEL_OUT_Y_L_REG    0x28
  #define ACCEL_OUT_Y_H_REG    0x29
  #define ACCEL_OUT_X_L_REG    0x2A
  #define ACCEL_OUT_X_H_REG    0x2B
  #define ACCEL_OUT_Z_L_REG    0x2C
  #define ACCEL_OUT_Z_H_REG    0x2D
  
  #define ACCEL_TEMP_L_REG     0x0C
  #define ACCEL_TEMP_H_REG     0x0D
  #define ACCEL_TEMP_CNFG      0x1F
  
  #define ACCEL_CTRL_REG1      0x20
  #define ACCEL_CTRL_REG2      0x21
  #define ACCEL_CTRL_REG3      0x22
  #define ACCEL_CTRL_REG4      0x23
  #define ACCEL_CTRL_REG5      0x24
  #define ACCEL_CTRL_REG6      0x25
  
  #define ZYXDR_BIT            0x08
  
  #define ACCEL_GET_ST         I2C_RdByte(ACCEL_DEV_ADDR, ACCEL_STATUS_REG)
  #define ACCEL_ZYX_ACTV       ACCEL_GET_ST & ZYXDR_BIT
  #define ACCEL_PWRDOWN()      I2C_WrByte(ACCEL_DEV_ADDR, ACCEL_CTRL_REG1, 0x00)
  
  void ACCEL_DevInit(void);
  void ACCEL_Get_Data(s16 *pDir);
    
//-------------------------- Comparator Definition -----------------------------

  #define COMPx                COMP1
  #define CP_CONNECT_PA1       COMP_INPUT_PLUS_IO3
  #define CM_CONNECT_PA4       COMP_INPUT_MINUS_IO4
  #define CM_CONNECT_PB1       COMP_INPUT_MINUS_IO1
  #define CM_CONNECT_VREF      COMP_INPUT_MINUS_VREFINT
  #define CM_CONNECT_DAC       COMP_INPUT_MINUS_DAC1_CH1
  
  #define COMP_IO_RCC_EN()     __HAL_RCC_GPIOA_CLK_ENABLE();
  #define DAO_IO_RCC_EN()      __HAL_RCC_GPIOA_CLK_ENABLE();
  #define DPO_IO_RCC_EN()      __HAL_RCC_GPIOA_CLK_ENABLE();
  #define COMP_IO_PORT         GPIOA
  #define CP_VM_PIN            GPIO_PIN_1
  #define CP_CONNECT           CP_CONNECT_PA1
  #define CM_AO_PO_PIN         GPIO_PIN_4
  #define CM_AO_IO_AF          GPIO_AF14_LPTIM2
  #define CM_CONNECT           CM_CONNECT_PA4
  #define CO_VP_PIN            GPIO_PIN_0
  #define CO_AF_MAP            GPIO_AF12_COMP1
  
  void COMP_DevCnfg(u32 Slct);
  
//--------------------------- OpAmp Dev Definition ----------------------------

  #define RD_RESISTOR          0x00
  #define RD_DIODE             0x10
  #define VP_LOW               0x00
  #define VP_HIGH              0x10

  #define OPAMP_DEV_RCC_EN()   __HAL_RCC_OPAMP_CLK_ENABLE();
  #define OPAMPx               OPAMP1
  #define VP_CONNECT_PA0       OPAMP_NONINVERTINGINPUT_IO0
  #define VP_CONNECT_DAC       OPAMP_NONINVERTINGINPUT_DAC_CH
  #define VM_CONNECT_PA1       OPAMP_INVERTINGINPUT_IO0
  #define VM_CONNECT_NO        OPAMP_INVERTINGINPUT_CONNECT_NO
  
  #define OPAMP_IO_RCC_EN()    __HAL_RCC_GPIOA_CLK_ENABLE();
  #define OPAMP_IO_PORT        GPIOA
//#define CO_VP_PIN            GPIO_PIN_0
  #define CP_VM_PIN            GPIO_PIN_1
  #define VO_PIN               GPIO_PIN_3
  
  void OPAMP_DevCnfg(u32 Slct);

//------------------------------ RTC Definition --------------------------------

  #define RTC_RCC_EN()         __HAL_RCC_RTC_ENABLE();
  #define OSC32_IO_RCC_EN()    __HAL_RCC_GPIOF_CLK_ENABLE();
  #define OSC32_GPIO_Port      GPIOF
  #define Xi_PIN               GPIO_PIN_0
  #define Xo_PIN               GPIO_PIN_1

  extern RTC_HandleTypeDef  hRtc;
  
  void RTC_DevInit(void);
  void RTC_GetTimeStr(char* Str);
  void RTC_GetDateStr(char* Str);

//---------------------------- TIM Dev Definition ------------------------------

  #define CLBR_TIM_RCC_ENBL()  __HAL_RCC_TIM16_CLK_ENABLE()
  #define CLBR_TIM             TIM16
  #define CLBR_CNT             CLBR_TIM->CCR1
  #define CLBR_CH              TIM_CHANNEL_1
  #define CLBR_SRC             TIM_TIM16_TI1_LSE
  #define CLBR_IRQ             TIM1_UP_TIM16_IRQn
  
  #define CNT_TIM_RCC_ENBL()   __HAL_RCC_TIM1_CLK_ENABLE()
  #define CNT_TIM_RCC_DSBL()   __HAL_RCC_TIM1_CLK_DISABLE()
  #define CNT_TIM              TIM1
  #define CNT_VAL              CNT_TIM->CNT
  #define CNT_REMAP            TIM_TIM1_ETR_COMP1
  
  #define FH_IO_RCC_EN()       __HAL_RCC_GPIOA_CLK_ENABLE();
  #define FH_PIN               GPIO_PIN_5
  #define FH_IO_PORT           GPIOA
  #define FH_IO_AF             GPIO_AF2_TIM2
  
  #define CPT_TIM_RCC_ENBL()   __HAL_RCC_TIM2_CLK_ENABLE()
  #define CPT_TIM_RCC_DSBL()   __HAL_RCC_TIM2_CLK_DISABLE()
  #define FH_CPT_TIM           TIM2
  #define FH_CNT_VAL           FH_CPT_TIM->CNT
  
  #define CAPTURE_IRQ          TIM2_IRQn
  #define CAPTURE_CHx          TIM_CHANNEL_4
  #define CAPTURE_VAL          FH_CPT_TIM->CCR4
  #define CAPTURE_REMAP        TIM_TIM2_TI4_COMP1
  
//#define AO_PO_IO_PORT        GPIOA
//#define CM_AO_PO_PIN         GPIO_PIN_4
  #define PO_IO_AF             GPIO_AF14_LPTIM2
  
  #define PO_TIM_RCC_ENBL()    __HAL_RCC_LPTIM2_CLK_ENABLE()
  #define PO_TIM_RCC_DSBL()    __HAL_RCC_LPTIM2_CLK_DISABLE()
  #define PO_TIM               LPTIM2
  #define PO_PULSE_CHx         TIM_CHANNEL_2
  #define PO_PULSE_CCR         DP_TIM->CCR1
  #define PO_PERIOD_ARR        DP_TIM->ARR
  #define PO_PERIOD_PSC        DP_TIM->PSC
  
  #define AOT_TIM_RCC_ENBL()   __HAL_RCC_TIM7_CLK_ENABLE()
  #define AOT_TIM_RCC_DSBL()   __HAL_RCC_TIM7_CLK_DISABLE()
  #define AOT_TIM              TIM7
  
  extern u32 CalbrSumT;
  
  void Calbr_CntF_TimerISP(void);
  void CalbrTimerInit(void);
  void CntClkCnfg(u16 Filter);
  void CounterTimerInit(void);
  void CaptureTimerISP(void);
  void CaptureEnbl(void);
  void CaptureDsbl(void);
  void Cpt_Fh_TimerCnfg(u16 Slct);
  void PulseGen_TimerCnfg(u16 Slct, u16 Psc, u16 Cycle, u16 Pulse);
  void Angl_Trig_TimerCnfg(u16 Slct, u16 Psc, u16 Cycle);

//------------------------------ DAC Definition --------------------------------

  #define VO_DCV               0x00
  #define VO_SINE              0x10
  #define VO_NOISE             0x20
  #define VO_TRIANGL           0x30
  #define VO_USER_DEF          0x40

//#define AO_PO_IO_PORT        GPIOA
//#define CM_AO_PO_PIN         GPIO_PIN_4
  
  #define DAC_RCC_EN()         __HAL_RCC_DAC1_CLK_ENABLE();
  #define DACx                 DAC1
  #define DAC_DMA_RCC_EN()     __HAL_RCC_DMA1_CLK_ENABLE();
  #define VOUT_ANLG_TRIG       DAC_TRIGGER_T7_TRGO
  #define CM_DAO_DPO_PIN       GPIO_PIN_4
  
  #define VOUT_DC_CH           DAC_CHANNEL_1
  #define DC_DMA_CH            DMA1_Channel3
  #define DC_DMA_REQ           DMA_REQUEST_6
  #define DC_DMA_HANDLE        DMA_Handle1
  
  extern uc16 SINE100[], SINE360[];
  
  void DAC_Cnfg(u16 Kind, u16 n, u16* pWfrm);

//---------------------------- ADC Dev Definition ------------------------------

//  #define VIN_IO_RCC_EN()      __HAL_RCC_GPIOA_CLK_ENABLE();
//  #define VIN_PIN              GPIO_PIN_1
//  #define VIN_IO_PORT          GPIOB
  
  #define ADC_DEV_RCC_EN()     __HAL_RCC_ADC_CLK_ENABLE();
  #define ADCx                 ADC1
  
  #define ADC_DMA_RCC_EN()     __HAL_RCC_DMA1_CLK_ENABLE();
  #define ADC_DMA_CH           DMA1_Channel1
  #define ADC_DMA_IRQ          DMA1_Channel1_IRQn
  #define ADC_VAL              ADC1->DR
  
  #define VREFERNCE            1206.1    // for Vref = 1212.1mV
  #define TS30                 (u16*)(u32)0x1FFF75A8
  #define TS110                (u16*)(u32)0x1FFF75CA
  
  #define PWR_CH               ADC_CHANNEL_7
  #define VXM_CH               ADC_CHANNEL_8
  #define REF_CH               ADC_CHANNEL_VREFINT
  
  typedef enum {VPWR, VADC, VREF}adc_ch_slct;//, VDDA, TEMP, VRHS, VRLS

  void ADC_DevInit(void);
  
//------------------------------ TSC Definition --------------------------------
  
  #define TSC_IO_RCC_EN()      __HAL_RCC_GPIOB_CLK_ENABLE();
  #define TSC_IO_PORT          GPIOB
  #define TSC_KEY1_PIN         GPIO_PIN_4
  #define TSC_SMPL_PIN         GPIO_PIN_7
  #define TSC_AF_MAP           GPIO_AF9_TSC
  
  #define TSC_DEV_RCC_EN()     __HAL_RCC_TSC_CLK_ENABLE();
  #define TSC_KEY1_IO          TSC_GROUP2_IO1
  #define TSC_SMPL_IO          TSC_GROUP2_IO4
  
  extern TSC_HandleTypeDef  hTsc; 
  extern u32 TSCxVal, TSCxSt, TSCxKey, TSCxSum;

  void TSC_DevInit(void);
  
//---------------------------- USB MSC Definition ------------------------------

  #define USB_IO_RCC_EN()      __HAL_RCC_GPIOA_CLK_ENABLE();
  #define USB_IO_PORT          GPIOA
  #define DP_PIN               GPIO_PIN_12
  #define DM_PIN               GPIO_PIN_11
  
  #define USB_DEV_RCC_EN()     __HAL_RCC_USB_CLK_ENABLE();
  
  #define SECTOR_SIZE          512
  #define SECTOR_NUM           4096
  
  void USB_IO_PinsCnfg(u8 Slct);

  #ifdef __cplusplus
    }
  #endif

#endif /* __BIOS_H */

/******************************** END OF FILE *********************************/
