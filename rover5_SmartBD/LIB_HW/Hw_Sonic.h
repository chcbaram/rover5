//----------------------------------------------------------------------------
//    프로그램명 	: 초음파 센서 관련 함수 헤더
//
//    만든이     	: Cho Han Cheol
//
//    날  짜     	: 2013. 1. 18.
//    
//    최종 수정  	: 2013. 1. 18.
//
//    MPU_Type 	:
//
//    파일명     	: Hw_Sonic.h
//----------------------------------------------------------------------------




                                                                                                 
#ifndef  _HW_SONIC_H
#define  _HW_SONIC_H



#include "Hw.h"


#ifdef   HW_SONIC_LOCAL
#define  EXT_HW_SONIC_DEF
#else
#define  EXT_HW_SONIC_DEF     extern
#endif


#ifndef  FALSE
#define  FALSE  0
#endif
#ifndef  TRUE
#define  TRUE   1
#endif
                                                                                                 




typedef struct 
{
	u8  Enable;
	u8	Edge;

	u16 CaptureTimerValueStart;
	u16 CaptureTimerValueEnd;
	
	u16 CaptureTimerWidth;

	u16 DistanceMM;
	
} HW_SONIC_OBJ;


#define HW_SONIC_MAX_CH  2



EXT_HW_SONIC_DEF HW_SONIC_OBJ Hw_Sonic_Log[HW_SONIC_MAX_CH];

                                            
EXT_HW_SONIC_DEF void Hw_Sonic_Init( void );
EXT_HW_SONIC_DEF void Hw_Sonic_SetPortDirIN( u8 Ch );
EXT_HW_SONIC_DEF void Hw_Sonic_SetPortDirOUT( u8 Ch );
EXT_HW_SONIC_DEF void Hw_Sonic_SetPortHIGH( u8 Ch );
EXT_HW_SONIC_DEF void Hw_Sonic_SetPortLOW( u8 Ch );
EXT_HW_SONIC_DEF void Hw_Sonic_ExIntEnable( u8 Ch );
EXT_HW_SONIC_DEF void Hw_Sonic_ExIntDisable( u8 Ch );
EXT_HW_SONIC_DEF u16  Hw_Sonic_GetDistanceTime( u8 Ch );
EXT_HW_SONIC_DEF u16  Hw_Sonic_GetDistanceMM( u8 Ch );


#endif
                                                                                                 
                                                                                                 

