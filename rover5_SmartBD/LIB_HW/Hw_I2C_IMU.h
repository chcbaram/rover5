//----------------------------------------------------------------------------
//    프로그램명 	: I2C IMU 관련 헤더
//
//    만든이     	: 
//
//    날  짜     	:
//    
//    최종 수정  	:
//
//    MPU_Type		:
//
//    파일명     	: Hw_I2C_IMU.h
//----------------------------------------------------------------------------
                                                                                                 
#ifndef __HW_I2C_IMU_H__
#define __HW_I2C_IMU_H__


#ifdef   HW_I2C_IMU_LOCAL
#define  EXT_HW_I2C_IMU_DEF
#else
#define  EXT_HW_I2C_IMU_DEF     extern
#endif



#include "Hw.h"


#define HW_I2C_IMU_ADDR_MPU6050		0x68
#define HW_I2C_IMU_ADDR_HMC5883		0x1E



EXT_HW_I2C_IMU_DEF void  Hw_I2C_IMU_Init( void );
EXT_HW_I2C_IMU_DEF u16   Hw_I2C_IMU_WriteRegs( u16 i2c_addr, u8 reg_addr, u8 *ptr_reg, u16 len );
EXT_HW_I2C_IMU_DEF u16   Hw_I2C_IMU_ReadRegs ( u16 i2c_addr, u8 reg_addr, u8 *ptr_reg, u16 len );
EXT_HW_I2C_IMU_DEF u16   Hw_I2C_IMU_Write    ( u16 i2c_addr, u8 *ptr_data, u16 len );
EXT_HW_I2C_IMU_DEF u16   Hw_I2C_IMU_Read     ( u16 i2c_addr, u8 *ptr_reg, u16 len );



//-- MPU6050
//
EXT_HW_I2C_IMU_DEF u16   Hw_I2C_IMU_MPU6050_WriteReg ( u8 reg_addr, u8 reg_data );
EXT_HW_I2C_IMU_DEF u16   Hw_I2C_IMU_MPU6050_WriteRegs( u8 reg_addr, u8 *ptr_data, u16 len );

EXT_HW_I2C_IMU_DEF u16   Hw_I2C_IMU_MPU6050_ReadReg ( u8 reg_addr, u8 *ptr_reg );
EXT_HW_I2C_IMU_DEF u16   Hw_I2C_IMU_MPU6050_ReadRegs( u8 reg_addr, u8 *ptr_reg, u16 len );



//-- HMC5883
//
EXT_HW_I2C_IMU_DEF u16   Hw_I2C_IMU_HMC5883_WriteAddr( u8 reg_addr );
EXT_HW_I2C_IMU_DEF u16   Hw_I2C_IMU_HMC5883_WriteReg ( u8 reg_addr, u8 reg_data );
EXT_HW_I2C_IMU_DEF u16   Hw_I2C_IMU_HMC5883_WriteRegs( u8 reg_addr, u8 *ptr_data, u16 len );

EXT_HW_I2C_IMU_DEF u16   Hw_I2C_IMU_HMC5883_ReadReg ( u8 reg_addr, u8 *ptr_reg );
EXT_HW_I2C_IMU_DEF u16   Hw_I2C_IMU_HMC5883_ReadRegs( u8 reg_addr, u8 *ptr_reg, u16 len );

#endif
