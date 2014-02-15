//----------------------------------------------------------------------------
//    프로그램명 	: HMC5883 지자계 센서 관련 함수
//
//    만든이     	: Cho Han Cheol(Baram)
//
//    날  짜     	: 
//    
//    최종 수정  	:
//
//    MPU_Type	:
//
//    파일명     	: Hw_HMC5883.c
//----------------------------------------------------------------------------


//----- 헤더파일 열기
//
#define  HW_HMC5883_LOCAL


#include "Hw_HMC5883.h"
#include "Lb_Printf.h"




u8 Hw_HMC5883_EnableFlag = FALSE;



typedef struct
{

	u8	X_Compass_H;
	u8	X_Compass_L;
	u8	Z_Compass_H;
	u8	Z_Compass_L;
	u8	Y_Compass_H;
	u8	Y_Compass_L;

} HW_HMC5883_RAW_DATA_OBJ;









/*---------------------------------------------------------------------------
     TITLE   : Hw_HMC5883_Init
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_HMC5883_Init( void )
{
	u16 Err = 0;
	u8  RegData[20];

	static u8 power_on = FALSE;


	if( power_on == TRUE && Hw_HMC5883_EnableFlag == TRUE ) return TRUE;


	Hw_HMC5883_EnableFlag = FALSE;

	Hw_Wait_ms(10);


	//-- I2C 초기화
	//
	Hw_I2C_IMU_Init();	



	if( Hw_HMC5883_CheckID() == FALSE )
	{
		Lb_printf("HMC5883 Fail to read ID \n");
		Err = HW_I2C_ERR_CHECK_ID;
	}
	else
	{
		Err = Hw_I2C_IMU_HMC5883_ReadRegs( HW_HMC5883_REG_CFG_A, RegData, 3 );

		if( Err == 0 )
		{
			//-- 75Hz로 출력 설정
			//
			SET_BIT( RegData[0], 4 );	// DO2 = 1
			SET_BIT( RegData[0], 3 );	// DO1 = 1
			CLR_BIT( RegData[0], 2 );	// DO0 = 0


			//-- Continuous 모드로 변경
			//
			CLR_BIT( RegData[2], 1 );	// MD1 = 0
			CLR_BIT( RegData[2], 0 );	// MD0 = 0

			Err = Hw_I2C_IMU_HMC5883_WriteRegs( HW_HMC5883_REG_CFG_A, RegData, 3 );


			Err = Hw_I2C_IMU_HMC5883_WriteAddr( HW_HMC5883_REG_DATA_X_MSB );
		}
	}




	power_on = TRUE;


	if( Err == 0 )
	{
		Hw_HMC5883_EnableFlag = TRUE;
	}


	return Err;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_HMC5883_CheckID
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u8 Hw_HMC5883_CheckID( void )
{
	u8  Ret = TRUE;
	u16 Err = 0;
	u8  RegData[20];
	u16 i;

	Err = Hw_I2C_IMU_HMC5883_ReadRegs( HW_HMC5883_REG_ID_A, RegData, 3 );

	//Lb_printf(" %x  %x  %x\n", RegData[0], RegData[1], RegData[2] );
	if( Err == 0 )
	{
		if( RegData[0] != 0x48 ) Ret = FALSE;
		if( RegData[1] != 0x34 ) Ret = FALSE;
		if( RegData[2] != 0x33 ) Ret = FALSE;				
	}

	if( Err != 0 ) Ret = FALSE;

	return Ret;	
}




/*---------------------------------------------------------------------------
     TITLE   : Hw_HMC5883_GetData
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_HMC5883_GetData( HW_HMC5883_DATA_OBJ *ptr_data )
{
	u16 Err;

	HW_HMC5883_RAW_DATA_OBJ raw_data;


	Err = Hw_I2C_IMU_HMC5883_ReadRegs( HW_HMC5883_REG_DATA_X_MSB, (u8 *)&raw_data, 6 );


	if( Err == 0 )
	{
		ptr_data->X_Compass = (s16)((raw_data.X_Compass_H<<8) | (raw_data.X_Compass_L<<0));	
		ptr_data->Y_Compass = (s16)((raw_data.Y_Compass_H<<8) | (raw_data.Y_Compass_L<<0));	
		ptr_data->Z_Compass = (s16)((raw_data.Z_Compass_H<<8) | (raw_data.Z_Compass_L<<0));	
	}
	

	Err = Hw_I2C_IMU_HMC5883_WriteAddr( HW_HMC5883_REG_DATA_X_MSB );

	return Err;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_HMC5883_GetEnable
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u8 Hw_HMC5883_GetEnable( void )
{
	return Hw_HMC5883_EnableFlag;
}
