//----------------------------------------------------------------------------
//    프로그램명 	: IMU 센서 관련 헤더
//
//    만든이     	: Cho Han Cheol(Baram)
//
//    날  짜     	:
//    
//    최종 수정  	:
//
//    MPU_Type	:
//
//    파일명     	: Hw_IMU.h
//----------------------------------------------------------------------------
                                                                                                 
#ifndef __HW_IMU_H__
#define __HW_IMU_H__


#ifdef   HW_IMU_LOCAL
#define  EXT_HW_IMU_DEF
#else
#define  EXT_HW_IMU_DEF     extern
#endif



#include "Hw.h"



#define HW_IMU_ACC		0
#define HW_IMU_GYRO		1
#define HW_IMU_COMPASS	2





typedef struct
{
	u8 Enable_Acc;
	u8 Enable_Temp;
	u8 Enable_Gyro;
	u8 Enable_Compass;
	
	s16 X_Acc;
	s16 Y_Acc;
	s16 Z_Acc;

	s16 Temp;
	
	s16 X_Gyro;
	s16 Y_Gyro;
	s16 Z_Gyro;

	s16 X_Compass;
	s16 Y_Compass;
	s16 Z_Compass;
	
} HW_IMU_DATA_OBJ;





EXT_HW_IMU_DEF u16 Hw_IMU_Init( void );

EXT_HW_IMU_DEF u16 Hw_IMU_GetData( HW_IMU_DATA_OBJ *ptr_data );

EXT_HW_IMU_DEF u16 Hw_IMU_GetEnable( void );




#endif
