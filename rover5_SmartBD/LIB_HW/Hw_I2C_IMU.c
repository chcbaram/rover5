//----------------------------------------------------------------------------
//    프로그램명 	: I2C IMU 관련 함수
//
//    만든이     	: 
//
//    날  짜     	: 
//    
//    최종 수정  	:
//
//    MPU_Type		:
//
//    파일명     	: Hw_I2C_IMU.c
//----------------------------------------------------------------------------


//----- 헤더파일 열기
//
#define  HW_I2C_IMU_LOCAL


#include "Hw_I2C_IMU.h"
#include "Lb_Printf.h"



#define HW_I2C_IMU_TIMEOUT	10000



void Hw_I2C_IMU_IO_Setup( void );
u16  Hw_I2C_IMU_WaitForBitSet( u32 *reg_addr, u32 reg_bit, u32 time_out );
u16  Hw_I2C_IMU_WaitForBitClr( u32 *reg_addr, u32 reg_bit, u32 time_out );
u16  Hw_I2C_IMU_GetErrStatus( void );




/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_Init
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_I2C_IMU_Init( void )
{
	static u8 power_on = FALSE;
	u32 SR;



	if( power_on == TRUE ) return;


	//-- I/O 초기화
	//
	Hw_I2C_IMU_IO_Setup();	


	REG_I2C1_CR1 = 0;

	//-- 1. I2C 클럭 입력 주파수 설정 
	//
	REG_I2C1_CR2 = 0 
				| ( 36 << 0 );		// FREQ = 36Mhz

	//-- 2. Clock Control 
	//
	REG_I2C1_CCR = 0 
				| (  1 << 15 )		// Fast Mode I2C
				| (  0 << 14 )		// Duty Tlow/Thig = 2
				| ( 30 <<  0 );		// 30*27ns + 2*30*27ns = 400Khz

	//-- 3. Maximum Rise Time  
	//
	REG_I2C1_TRISE = 11;			// 300ns / 28ns + 1 = 11 


	//-- Clock Enable
	//
	REG_RCC_APB1ENR |= (1<<21);		// I2C1 Clock enable


	CLR_BIT( REG_I2C1_CR1, 1 );		// I2C Mode		
	SET_BIT( REG_I2C1_CR1, 0 );		// Peripheral enable	

	SR = REG_I2C1_SR1;
	SR = REG_I2C1_SR2;


	power_on = TRUE;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_IO_Setup
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_I2C_IMU_IO_Setup( void )
{

	//-- I2C1 Remap
	//
	SET_BIT( REG_AFIO_MAPR, 1 );	// SCL/PB8, SDA/PB9)


	//-- SCL PB.8
	//
	REG_GPIOB_CRH &= ~(0x0F << ( 0));	// Clear
	REG_GPIOB_CRH |=  (0x03 << ( 0));	// MODE, PB.8 Output mode, max speed 50Mhz
	REG_GPIOB_CRH |=  (0x03 << ( 2));	// CNF,  PB.8 Alternate function output Open-drain	


	//-- SDA PB.9
	//
	REG_GPIOB_CRH &= ~(0x0F << ( 4));	// Clear
	REG_GPIOB_CRH |=  (0x03 << ( 4));	// MODE, PB.9 Output mode, max speed 50Mhz
	REG_GPIOB_CRH |=  (0x03 << ( 6));	// CNF,  PB.9 Alternate function output Open-drain	
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_MPU6050_WriteReg
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_MPU6050_WriteReg( u8 reg_addr, u8 reg_data )
{
	return Hw_I2C_IMU_WriteRegs( HW_I2C_IMU_ADDR_MPU6050, reg_addr, &reg_data, 1 );
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_MPU6050_WriteRegs
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_MPU6050_WriteRegs( u8 reg_addr, u8 *ptr_data, u16 len )
{
	return Hw_I2C_IMU_WriteRegs( HW_I2C_IMU_ADDR_MPU6050, reg_addr, ptr_data, len );
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_MPU6050_ReadReg
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_MPU6050_ReadReg( u8 reg_addr, u8 *ptr_reg )
{
	return Hw_I2C_IMU_ReadRegs( HW_I2C_IMU_ADDR_MPU6050, reg_addr, ptr_reg, 1 );
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_MPU6050_ReadRegs
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_MPU6050_ReadRegs( u8 reg_addr, u8 *ptr_reg, u16 len )
{
	return Hw_I2C_IMU_ReadRegs( HW_I2C_IMU_ADDR_MPU6050, reg_addr, ptr_reg, len );
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_HMC5883_WriteAddr
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_HMC5883_WriteAddr( u8 reg_addr )
{
	u8 reg_data;

	return Hw_I2C_IMU_WriteRegs( HW_I2C_IMU_ADDR_HMC5883, reg_addr, &reg_data, 0 );
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_HMC5883_WriteReg
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_HMC5883_WriteReg( u8 reg_addr, u8 reg_data )
{	
	return Hw_I2C_IMU_WriteRegs( HW_I2C_IMU_ADDR_HMC5883, reg_addr, &reg_data, 1 );
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_HMC5883_WriteRegs
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_HMC5883_WriteRegs( u8 reg_addr, u8 *ptr_data, u16 len )
{
	return Hw_I2C_IMU_WriteRegs( HW_I2C_IMU_ADDR_HMC5883, reg_addr, ptr_data, len );
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_HMC5883_ReadReg
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_HMC5883_ReadReg( u8 reg_addr, u8 *ptr_reg )
{
	return Hw_I2C_IMU_ReadRegs( HW_I2C_IMU_ADDR_HMC5883, reg_addr, ptr_reg, 1 );
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_HMC5883_ReadRegs
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_HMC5883_ReadRegs( u8 reg_addr, u8 *ptr_reg, u16 len )
{
	/*
	u16 i;
	u16 Ret;
	volatile u8  RegData;

	
	for( i=0; i<len; i++ )
	{
		Ret = Hw_I2C_IMU_HMC5883_ReadReg( reg_addr+i, &RegData );
		if( Ret ) break;

		ptr_reg[i] = RegData;
	}
	

	return Ret;
	*/
	return Hw_I2C_IMU_ReadRegs( HW_I2C_IMU_ADDR_HMC5883, reg_addr, ptr_reg, len );
}







/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_WriteRegs
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_WriteRegs( u16 i2c_addr, u8 reg_addr, u8 *ptr_data, u16 len )
{
    u16 Ret = 0;
    u16 Err = 0;
	u8  i;
	u32 TimeOut;
	u32 SR1;
	u32 SR2;


	while(1)
	{
			//-- START
			//
			SET_BIT( REG_I2C1_CR1, 8 );

			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 0, HW_I2C_IMU_TIMEOUT );
			if( Ret != 0 )
			{
				Err = HW_I2C_ERR_START;
				break;
			}

			//Lb_printf("S1 %x\n", REG_I2C1_SR1);
			//Lb_printf("S1 %x\n", REG_I2C1_SR2);

			//-- ADDR
			//
			REG_I2C1_DR = i2c_addr<<1;	// ADDR Write 

			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 1, HW_I2C_IMU_TIMEOUT );	// ADDR Sent
			SR2 = REG_I2C1_SR2;

			if( Ret != 0 ) 
			{
				Err = HW_I2C_ERR_ADDR;
				break;
			}


			//Lb_printf("S2 %x\n", REG_I2C1_SR1);
			//Lb_printf("S2 %x\n", REG_I2C1_SR2);
				
			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 7, HW_I2C_IMU_TIMEOUT );	// TXE
			if( Ret != 0 )
			{
				Err = HW_I2C_ERR_TXE;
				break;
			}


			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 7, HW_I2C_IMU_TIMEOUT );	// TXE
			if( Ret != 0 )
			{
				Err = HW_I2C_ERR_TXE;
				break;
			}

			REG_I2C1_DR = reg_addr;	



			for( i=0; i<len; i++ )
			{
				Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 7, HW_I2C_IMU_TIMEOUT );	// TXE
				if( Ret != 0 )
				{
					Err = HW_I2C_ERR_TXE;
					break;
				}

				REG_I2C1_DR = ptr_data[i];						
			}


			//-- STOP
			//
			if( len > 0 )
			{
				Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 2, HW_I2C_IMU_TIMEOUT );	// BTF
				if( Ret != 0 ) 
				{
					Err = HW_I2C_ERR_STOP;
					break;
				}
			}

			SET_BIT( REG_I2C1_CR1, 9 );	// STOP
			break;
	}


	//-- I2C가 Idle일때까지 기다린다.
	//
	Ret = Hw_I2C_IMU_WaitForBitClr( (u32 *)&REG_I2C1_SR2, 1, HW_I2C_IMU_TIMEOUT );

	if( Ret != 0 )	Err = HW_I2C_ERR_END;



	if( Err == 0 )
	{
		Err = Hw_I2C_IMU_GetErrStatus();
	}
	

    return Err;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_ReadRegs
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_ReadRegs( u16 i2c_addr, u8 reg_addr, u8 *ptr_reg, u16 len )
{
    u16 Ret = 0;
    u16 Err = 0;
	u8  i;
	u32 TimeOut;
	volatile u32 SR1;
	volatile u32 SR2;
	u8  LastIndex = 0;


	if( len > 0 ) LastIndex = len - 1;


	while(1)
	{

			//Lb_printf("C1 %x\n", REG_I2C1_CR1);

			SR1 = REG_I2C1_SR1;
			SR2 = REG_I2C1_SR2;


			//Lb_printf("S1 %x\n", REG_I2C1_SR1);
			//Lb_printf("S1 %x\n", REG_I2C1_SR2);

			//-- START
			//
			SET_BIT( REG_I2C1_CR1, 8 );

			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 0, HW_I2C_IMU_TIMEOUT );

			//Lb_printf("S1 %x\n", REG_I2C1_SR1);
			//Lb_printf("S1 %x\n", REG_I2C1_SR2);

			if( Ret != 0 )
			{
				Err = HW_I2C_ERR_START;
				break;
			}


			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR2, 0, HW_I2C_IMU_TIMEOUT );
			if( Ret != 0 ) 
			{
				Err = HW_I2C_ERR_START;
				break;
			}



			//-- ADDR
			//
			REG_I2C1_DR = i2c_addr<<1;	// ADDR Write 

			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 1, HW_I2C_IMU_TIMEOUT );	// ADDR Sent
			SR2 = REG_I2C1_SR2;

			if( Ret != 0 ) 
			{
				Err = HW_I2C_ERR_ADDR;
				break;
			}

			
			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 7, HW_I2C_IMU_TIMEOUT );	// TXE
			if( Ret != 0 )
			{
				Err = HW_I2C_ERR_TXE;
				break;
			}


			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 7, HW_I2C_IMU_TIMEOUT );	// TXE
			if( Ret != 0 )
			{
				Err = HW_I2C_ERR_TXE;
				break;
			}

			REG_I2C1_DR = reg_addr;						


			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 2, HW_I2C_IMU_TIMEOUT );	// BTF
			if( Ret != 0 ) 
			{
				Err = HW_I2C_ERR_STOP;
				break;
			}



			//-- START
			//
			SET_BIT( REG_I2C1_CR1, 8 );

			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 0, HW_I2C_IMU_TIMEOUT );
			if( Ret != 0 )
			{
				Err = HW_I2C_ERR_START;
				break;
			}


			//-- ADDR
			//
			REG_I2C1_DR = (i2c_addr<<1) | (1<<0) ;	// ADDR Read

			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 1, HW_I2C_IMU_TIMEOUT );	// ADDR Sent
			SR2 = REG_I2C1_SR2;

			if( Ret != 0 ) 
			{
				Err = HW_I2C_ERR_ADDR;
				break;
			}


			//-- Read DATA
			//
			for( i=0; i<len; i++ )
			{
				if( i < LastIndex )
				{
					SET_BIT( REG_I2C1_CR1, 10 );	// ACK Enable					
				}
				else
				{
					CLR_BIT( REG_I2C1_CR1, 10 );	// ACK Disable
					SET_BIT( REG_I2C1_CR1, 9 );	// STOP
				}


				Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 6, HW_I2C_IMU_TIMEOUT );
				if( Ret == 0 )
				{
					ptr_reg[i] = REG_I2C1_DR;
				}
				else
				{
					Err = HW_I2C_ERR_RXE;
					break;
				}					 		
			}

			break;
	}


	//-- I2C가 Idle일때까지 기다린다.
	//
	Ret = Hw_I2C_IMU_WaitForBitClr( (u32 *)&REG_I2C1_SR2, 1, HW_I2C_IMU_TIMEOUT );

	if( Ret != 0 )	Err = HW_I2C_ERR_END;


	if( Err == 0 )
	{
		Err = Hw_I2C_IMU_GetErrStatus();
	}
	

    return Err;

}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_Write
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_Write( u16 i2c_addr, u8 *ptr_data, u16 len )
{
    u16 Ret = 0;
    u16 Err = 0;
	u8  i;
	u32 TimeOut;
	u32 SR1;
	u32 SR2;


	while(1)
	{
			//-- START
			//
			SET_BIT( REG_I2C1_CR1, 8 );

			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 0, HW_I2C_IMU_TIMEOUT );
			if( Ret != 0 )
			{
				Err = HW_I2C_ERR_START;
				break;
			}

			//-- ADDR
			//
			REG_I2C1_DR = i2c_addr<<1;	// ADDR Write 

			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 1, HW_I2C_IMU_TIMEOUT );	// ADDR Sent
			SR2 = REG_I2C1_SR2;

			if( Ret != 0 ) 
			{
				Err = HW_I2C_ERR_ADDR;
				break;
			}

				
			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 7, HW_I2C_IMU_TIMEOUT );	// TXE
			if( Ret != 0 )
			{
				Err = HW_I2C_ERR_TXE;
				break;
			}


			for( i=0; i<len; i++ )
			{
				Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 7, HW_I2C_IMU_TIMEOUT );	// TXE
				if( Ret != 0 )
				{
					Err = HW_I2C_ERR_TXE;
					break;
				}

				REG_I2C1_DR = ptr_data[i];						
			}


			//-- STOP
			//
			if( len > 0 )
			{
				Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 2, HW_I2C_IMU_TIMEOUT );	// BTF
				if( Ret != 0 ) 
				{
					Err = HW_I2C_ERR_STOP;
					break;
				}
			}

			SET_BIT( REG_I2C1_CR1, 9 );	// STOP
			break;
	}


	//-- I2C가 Idle일때까지 기다린다.
	//
	Ret = Hw_I2C_IMU_WaitForBitClr( (u32 *)&REG_I2C1_SR2, 1, HW_I2C_IMU_TIMEOUT );

	if( Ret != 0 )	Err = HW_I2C_ERR_END;



	if( Err == 0 )
	{
		Err = Hw_I2C_IMU_GetErrStatus();
	}
	

    return Err;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_Read
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_Read( u16 i2c_addr, u8 *ptr_reg, u16 len )
{
    u16 Ret = 0;
    u16 Err = 0;
	u8  i;
	u32 TimeOut;
	volatile u32 SR1;
	volatile u32 SR2;
	u8  LastIndex = 0;


	if( len > 0 ) LastIndex = len - 1;


	while(1)
	{

			SR1 = REG_I2C1_SR1;
			SR2 = REG_I2C1_SR2;


			//-- START
			//
			SET_BIT( REG_I2C1_CR1, 8 );

			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 0, HW_I2C_IMU_TIMEOUT );
			if( Ret != 0 )
			{
				Err = HW_I2C_ERR_START;
				break;
			}


			//-- ADDR
			//
			REG_I2C1_DR = (i2c_addr<<1) | (1<<0) ;	// ADDR Read

			Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 1, HW_I2C_IMU_TIMEOUT );	// ADDR Sent
			SR2 = REG_I2C1_SR2;

			if( Ret != 0 ) 
			{
				Err = HW_I2C_ERR_ADDR;
				break;
			}


			//-- Read DATA
			//
			for( i=0; i<len; i++ )
			{
				if( i < LastIndex )
				{
					SET_BIT( REG_I2C1_CR1, 10 );	// ACK Enable					
				}
				else
				{
					CLR_BIT( REG_I2C1_CR1, 10 );	// ACK Disable
					SET_BIT( REG_I2C1_CR1, 9 );		// STOP
				}


				Ret = Hw_I2C_IMU_WaitForBitSet( (u32 *)&REG_I2C1_SR1, 6, HW_I2C_IMU_TIMEOUT );
				if( Ret == 0 )
				{
					ptr_reg[i] = REG_I2C1_DR;
				}
				else
				{
					Err = HW_I2C_ERR_RXE;
					break;
				}					 		
			}

			break;
	}


	//-- I2C가 Idle일때까지 기다린다.
	//
	Ret = Hw_I2C_IMU_WaitForBitClr( (u32 *)&REG_I2C1_SR2, 1, HW_I2C_IMU_TIMEOUT );

	if( Ret != 0 )	Err = HW_I2C_ERR_END;


	if( Err == 0 )
	{
		Err = Hw_I2C_IMU_GetErrStatus();
	}
	

    return Err;

}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_WaitForBitSet
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_WaitForBitSet( u32 *reg_addr, u32 reg_bit, u32 time_out )
{
	u32 Ret = 0;
	volatile u32 Reg;


	while( time_out )
	{
		Reg = *reg_addr;

		if( IS_SET_BIT( Reg, reg_bit ) ) break;

		time_out--;
	}
			
	if( time_out == 0 ) 
	{
		Ret = HW_I2C_ERR_TIMEOUT;
	}

	return Ret;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_WaitForBitClr
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_WaitForBitClr( u32 *reg_addr, u32 reg_bit, u32 time_out )
{
	u32 Ret = 0;
	volatile u32 Reg;


	while( time_out )
	{
		Reg = *reg_addr;

		if( IS_CLR_BIT( Reg, reg_bit ) ) break;

		time_out--;
	}
			
	if( time_out == 0 ) 
	{
		Ret = HW_I2C_ERR_TIMEOUT;
	}

	return Ret;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_I2C_IMU_GetErrStatus
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_I2C_IMU_GetErrStatus( void )
{
	u16 Err = 0;
	u16 SR;

	if( IS_SET_BIT(REG_I2C1_SR1, 11) ) Err = 11;
	if( IS_SET_BIT(REG_I2C1_SR1, 10) ) Err = 10;
	if( IS_SET_BIT(REG_I2C1_SR1,  9) ) Err =  9;
	if( IS_SET_BIT(REG_I2C1_SR1,  8) ) Err =  8;


	//-- 에러가 난경우 복구
	//
	if( Err != 0 )
	{
		CLR_BIT( REG_I2C1_CR1, 0 );		// Peripheral disable	
		SR = REG_I2C1_SR1;
		SR = REG_I2C1_SR2;

		SET_BIT( REG_I2C1_CR1, 0 );		// Peripheral enable	
		SR = REG_I2C1_SR1;
		SR = REG_I2C1_SR2;
	}

	return Err;
}










