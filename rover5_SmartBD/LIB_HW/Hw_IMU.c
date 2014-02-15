//----------------------------------------------------------------------------
//    프로그램명 	: IMU 센서 관련 함수
//
//    만든이     	: Cho Han Cheol(Baram)
//
//    날  짜     	: 
//    
//    최종 수정  	:
//
//    MPU_Type	:
//
//    파일명     	: Hw_IMU.c
//----------------------------------------------------------------------------


//----- 헤더파일 열기
//
#define  HW_HMC5883_LOCAL


#include "Hw_IMU.h"
#include "Lb_Printf.h"









/*---------------------------------------------------------------------------
     TITLE   : Hw_IMU_Init
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_IMU_Init( void )
{
	u16 Ret = 0;


	Ret = Hw_MPU6050_Init();

	if( Ret != 0 )
	{
		Lb_printf("Hw_MPU6050_Init Ret : 0x%x\n", Ret);
		return Ret;
	} 


	Ret = Hw_HMC5883_Init();

	if( Ret != 0 )
	{
		Lb_printf("Hw_HMC5883_Init Ret : 0x%x\n", Ret);
		return Ret;
	} 


	return Ret;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_IMU_GetData
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_IMU_GetData( HW_IMU_DATA_OBJ *ptr_data )
{
	u16 Err = 0;

	HW_MPU6050_DATA_OBJ MPU6050_Data;
	HW_HMC5883_DATA_OBJ HMC5883_Data;


	ptr_data->Enable_Acc     = FALSE;
	ptr_data->Enable_Temp    = FALSE;
	ptr_data->Enable_Gyro    = FALSE;
	ptr_data->Enable_Compass = FALSE;


	if( Hw_HMC5883_GetEnable() == TRUE )
	{
		Err = Hw_MPU6050_GetData( &MPU6050_Data );

		if( Err == 0 )
		{
			ptr_data->X_Acc = MPU6050_Data.X_Acc;
			ptr_data->Y_Acc = MPU6050_Data.Y_Acc;
			ptr_data->Z_Acc = MPU6050_Data.Z_Acc;
			
			ptr_data->Temp  = MPU6050_Data.Temp;

			ptr_data->X_Gyro = MPU6050_Data.X_Gyro;
			ptr_data->Y_Gyro = MPU6050_Data.Y_Gyro;
			ptr_data->Z_Gyro = MPU6050_Data.Z_Gyro;

			ptr_data->Enable_Acc     = TRUE;
			ptr_data->Enable_Temp    = TRUE;
			ptr_data->Enable_Gyro    = TRUE;				
		}
	}



	if( Hw_HMC5883_GetEnable() == TRUE )
	{
		Err = Hw_HMC5883_GetData( &HMC5883_Data );	

		if( Err == 0 )
		{
			ptr_data->X_Compass = HMC5883_Data.X_Compass;
			ptr_data->Y_Compass = HMC5883_Data.Y_Compass;
			ptr_data->Z_Compass = HMC5883_Data.Z_Compass;

			ptr_data->Enable_Compass = TRUE;			
		}
	}


	return Err;
}
