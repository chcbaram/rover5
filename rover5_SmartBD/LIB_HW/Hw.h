//----------------------------------------------------------------------------
//    프로그램명 	: Hw 공용 
//
//    만든이     	: 
//
//    날  짜     	:
//    
//    최종 수정  	:
//
//    MPU_Type		:
//
//    파일명     	: Hw.h
//----------------------------------------------------------------------------
                                                                                                 
#ifndef __HW_H__
#define __HW_H__                                                                                                  


#ifdef   HW_LOCAL
#define  EXT_HW_DEF 
#else
#define  EXT_HW_DEF     extern
#endif


//-- 기능 설정
//
#define HW_USE_USB_VCOM					0
#define HW_USE_USB_SMARTROBOT_BD		1				// 내가 현재 가지고 있는 보드와 스마트로봇 보드가 USB Enable 핀이 다름
#define HW_USE_UART_CH_MENU				HW_UART_COM1
#define HW_USE_UART_CH_BT				HW_UART_COM2
#define HW_USE_UART_CH_STRCMD			HW_UART_COM1



//-- 타이머 채널 설정
//
#define HW_TIMER_CH_LED					0
#define HW_TIMER_CH_DEBUG1				1
#define HW_TIMER_CH_DEBUG2				2
#define HW_TIMER_CH_GLCD				3
#define HW_TIMER_CH_KEYPAD				4
#define HW_TIMER_CH_7SEG				5
#define HW_TIMER_CH_DCMOTOR				6
#define HW_TIMER_CH_SONIC				7



//-- Core
//
#include "Hw_Regs.h"
#include "Hw_Define.h"
#include "Hw_PLL.h"
#include "Hw_DMA.h"
#include "Hw_Uart.h"
#include "Hw_Led.h"
#include "Hw_ISR.h"
#include "Hw_Timer.h"
#include "Hw_I2C.h"
#include "Hw_SPI.h"
#include "Hw_Adc.h"
#include "Hw_Pwm.h"

//-- Driver
//
#include "Hw_Si47xx.h"
#include "Hw_N5110G.h"
#include "Hw_VBus.h"
#include "Hw_KeyPad.h"
#include "Hw_7Seg.h"
#include "Hw_CLcd.h"
#include "Hw_Cds.h"
#include "Hw_DcMotor.h"
#include "Hw_VCom.h"
#include "Hw_Sonic.h"

#include "Lb_Printf.h"


EXT_HW_DEF void Hw_Init( void );

EXT_HW_DEF void Hw_Wait( u32 delay );
EXT_HW_DEF void Hw_Wait_Usec( u32 usec );

#endif
