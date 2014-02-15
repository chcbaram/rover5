//----------------------------------------------------------------------------
//    프로그램명 	: HMC5883 지자계 센서 관련 헤더
//
//    만든이     	: Cho Han Cheol(Baram)
//
//    날  짜     	:
//    
//    최종 수정  	:
//
//    MPU_Type	:
//
//    파일명     	: Hw_HMC5883.h
//----------------------------------------------------------------------------
                                                                                                 
#ifndef __HW_HMC5883_H__
#define __HW_HMC5883_H__


#ifdef   HW_HMC5883_LOCAL
#define  EXT_HW_HMC5883_DEF
#else
#define  EXT_HW_HMC5883_DEF     extern
#endif



#include "Hw.h"





#define HW_HMC5883_REG_CFG_A		0x00
#define HW_HMC5883_REG_CFG_B		0x01
#define HW_HMC5883_REG_MODE			0x02
#define HW_HMC5883_REG_DATA_X_MSB	0x03
#define HW_HMC5883_REG_DATA_X_LSB	0x04
#define HW_HMC5883_REG_DATA_Z_MSB	0x05
#define HW_HMC5883_REG_DATA_Z_LSB	0x06
#define HW_HMC5883_REG_DATA_Y_MSB	0x07
#define HW_HMC5883_REG_DATA_Y_LSB	0x08
#define HW_HMC5883_REG_STATUS		0x09
#define HW_HMC5883_REG_ID_A			0x0A
#define HW_HMC5883_REG_ID_B			0x0B
#define HW_HMC5883_REG_ID_C			0x0C






typedef struct
{

	s16 X_Compass;
	s16 Y_Compass;
	s16 Z_Compass;
	
} HW_HMC5883_DATA_OBJ;





EXT_HW_HMC5883_DEF u16 Hw_HMC5883_Init( void );
EXT_HW_HMC5883_DEF u8  Hw_HMC5883_CheckID( void );


EXT_HW_HMC5883_DEF u16 Hw_HMC5883_GetData( HW_HMC5883_DATA_OBJ *ptr_data );
EXT_HW_HMC5883_DEF u8  Hw_HMC5883_GetEnable( void );




#endif
