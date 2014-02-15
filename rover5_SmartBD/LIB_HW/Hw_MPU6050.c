//----------------------------------------------------------------------------
//    프로그램명 	: I2C MPU6050 관련 함수
//
//    만든이     	: 
//
//    날  짜     	: 
//    
//    최종 수정  	:
//
//    MPU_Type		:
//
//    파일명     	: Hw_MPU6050.c
//----------------------------------------------------------------------------


//----- 헤더파일 열기
//
#define  HW_MPU6050_LOCAL


#include "Hw_MPU6050.h"
#include "Lb_Printf.h"



u8 Hw_MPU6050_EnableFlag = FALSE;


typedef struct
{
	u8	X_Acc_H;
	u8	X_Acc_L;
	u8	Y_Acc_H;
	u8	Y_Acc_L;
	u8	Z_Acc_H;
	u8	Z_Acc_L;

	u8	Temp_H;
	u8	Temp_L;

	u8	X_Gyro_H;
	u8	X_Gyro_L;
	u8	Y_Gyro_H;
	u8	Y_Gyro_L;
	u8	Z_Gyro_H;
	u8	Z_Gyro_L;

} HW_MPU6050_RAW_DATA_OBJ;









/*---------------------------------------------------------------------------
     TITLE   : Hw_MPU6050_Init
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_MPU6050_Init( void )
{
	u16 Err = 0;
	u8  RegData[20];

	static u8 power_on = FALSE;


	if( power_on == TRUE && Hw_MPU6050_EnableFlag == TRUE ) return TRUE;


	Hw_MPU6050_EnableFlag = FALSE;

	Hw_Wait_ms(36);


	//-- I2C 초기화
	//
	Hw_I2C_IMU_Init();	


	while(1)
	{

		Err = Hw_I2C_IMU_MPU6050_ReadReg( MPU6050_RA_WHO_AM_I, RegData );
		if( Err != 0 )
		{
			Lb_printf("Can't find MPU6050 \n");
			break;
		}


		Err = Hw_I2C_IMU_MPU6050_WriteReg( MPU6050_RA_PWR_MGMT_1, 0x00 );	// 내부 오실레이터, SLEEP = 0
		if( Err != 0 )
		{
			Lb_printf("Err write MPU6050_RA_PWR_MGMT_1 : %d\n", Err);
			break;
		}

		break;
	}

	Hw_MPU6050_I2CAuxMasterDisable();


	power_on = TRUE;


	if( Err == 0 )
	{
		Hw_MPU6050_EnableFlag = TRUE;
	}


	return Err;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_MPU6050_GetData
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_MPU6050_GetData( HW_MPU6050_DATA_OBJ *ptr_data )
{
	u16 Err;
	s16 Temp;

	HW_MPU6050_RAW_DATA_OBJ raw_data;


	Err = Hw_I2C_IMU_MPU6050_ReadRegs( MPU6050_RA_ACCEL_XOUT_H, (u8 *)&raw_data, 14 );

	if( Err == 0 )
	{
		ptr_data->X_Acc = (s16)((raw_data.X_Acc_H<<8) | (raw_data.X_Acc_L<<0));	
		ptr_data->Y_Acc = (s16)((raw_data.Y_Acc_H<<8) | (raw_data.Y_Acc_L<<0));	
		ptr_data->Z_Acc = (s16)((raw_data.Z_Acc_H<<8) | (raw_data.Z_Acc_L<<0));	

		ptr_data->X_Gyro = (s16)((raw_data.X_Gyro_H<<8) | (raw_data.X_Gyro_L<<0));	
		ptr_data->Y_Gyro = (s16)((raw_data.Y_Gyro_H<<8) | (raw_data.Y_Gyro_L<<0));	
		ptr_data->Z_Gyro = (s16)((raw_data.Z_Gyro_H<<8) | (raw_data.Z_Gyro_L<<0));	

		Temp = (s16)((raw_data.Temp_H<<8) | (raw_data.Temp_L<<0));	
		Temp = Temp/340 + 36;
		ptr_data->Temp = Temp;
	}

	return Err;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_MPU6050_I2CAuxMasterDisable
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_MPU6050_I2CAuxMasterDisable( void )
{
	u16 Err;
	u8  RegData[20];

	//-- I2C_BYPASS_EN = 1
	//
	Err = Hw_I2C_IMU_MPU6050_ReadReg( MPU6050_RA_INT_PIN_CFG, RegData );
	if( Err ) return Err;

	SET_BIT( RegData[0], 1 );

	Err = Hw_I2C_IMU_MPU6050_WriteReg( MPU6050_RA_INT_PIN_CFG, RegData[0] );

	return Err;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_MPU6050_I2CAuxMasterEnable
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_MPU6050_I2CAuxMasterEnable( void )
{
	u16 Err;
	u8  RegData[20];

	//-- I2C_BYPASS_EN = 1
	//
	Err = Hw_I2C_IMU_MPU6050_ReadReg( MPU6050_RA_INT_PIN_CFG, RegData );
	if( Err ) return Err;

	CLR_BIT( RegData[0], 1 );

	Err = Hw_I2C_IMU_MPU6050_WriteReg( MPU6050_RA_INT_PIN_CFG, RegData[0] );

	return Err;
}




/*---------------------------------------------------------------------------
     TITLE   : Hw_MPU6050_GetEnable
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u8 Hw_MPU6050_GetEnable( void )
{
	return Hw_MPU6050_EnableFlag;
}