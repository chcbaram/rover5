//----------------------------------------------------------------------------
//    프로그램명 : PWM 관련 함수
//
//    만든이     : Cho Han Cheol 
//
//    날  짜     : 
//    
//    최종 수정  : 
//
//    MPU_Type   : 
//
//    파일명     : Hw_Pwm.c
//----------------------------------------------------------------------------



//----- 헤더파일 열기
//
#define  HW_PWM_LOCAL  

#include "Hw_Pwm.h"






//----- 내부 함수
//
void Hw_Pwm_IO_Setup(void);
void Hw_Pwm_Timer_Setup(void);



//----- 내부 변수











/*---------------------------------------------------------------------------
     TITLE   : Hw_Pwm_Init
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_Pwm_Init(void)
{	
	Hw_Pwm_IO_Setup();
	Hw_Pwm_Timer_Setup();
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_Pwm_Timer_Setup
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_Pwm_IO_Setup(void)
{	
	//-- GPIO 설정
	//
	//-- PWM0 PB.0
	//
	REG_GPIOB_CRL &= ~(0x0F << ( 0));	// Clear
	REG_GPIOB_CRL |=  (0x03 << ( 0));	// MODE, PB.0 Output mode, max speed 50Mhz
	REG_GPIOB_CRL |=  (0x02 << ( 2));	// CNF,  PB.0 Alternate function output Push-pull

	//-- PWM1 PB.1
	//
	REG_GPIOB_CRL &= ~(0x0F << ( 4));	// Clear
	REG_GPIOB_CRL |=  (0x03 << ( 4));	// MODE, PB.1 Output mode, max speed 50Mhz
	REG_GPIOB_CRL |=  (0x02 << ( 6));	// CNF,  PB.1 Alternate function output Push-pull
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_Pwm_Timer_Setup
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_Pwm_Timer_Setup(void)
{	
	SET_BIT( REG_RCC_APB1ENR, 1 );		// TIM3 Clock Enable APB1*2 = 72Mhz

	REG_TIM3_PSC = (72*8)-1; 				// 72Mhz/(72*4) = 1Mhz


	REG_TIM3_CR1 = ( 1<< 7 );			// TIMx_ARR register is buffered 
	REG_TIM3_CR2 = 0;					

	REG_TIM3_CCMR2 = 0 
		| ( 0x06 <<  4)					// OE3
		| ( 1    <<  3)					// OC3PE Output compare3 preload enable
		| ( 0x06 << 12)					// OE4
		| ( 1    << 11);				// OC4PE Output compare3 preload enable
	
	REG_TIM3_ARR = HW_PWM_PERIOD-1;		// 1000us = 1Khz 	

	SET_BIT( REG_TIM3_CCER,  8 );		// CC3E : Enable
	SET_BIT( REG_TIM3_CCER, 12 );		// CC4E : Enable

	Hw_Pwm_SetDuty(0, 0);
	Hw_Pwm_SetDuty(1, 0);

	SET_BIT( REG_TIM3_CR1, 0 );			// Counter Enable 
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_Pwm_SetDuty
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_Pwm_SetDuty( u8 Ch, u16 DutyRatio )
{
	switch( Ch )
	{
		case 0:
			REG_TIM3_CCR3 = DutyRatio * HW_PWM_PERIOD / HW_PWM_MAX_DUTY;

		case 1:
			REG_TIM3_CCR4 = DutyRatio * HW_PWM_PERIOD / HW_PWM_MAX_DUTY;
	}
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_Pwm_SetPercent
     WORK    : 
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_Pwm_SetPercent( u8 Ch, u16 PercentRatio )
{
	if( PercentRatio > 100 ) PercentRatio = 100;

	Hw_Pwm_SetDuty( Ch, PercentRatio * HW_PWM_PERIOD / 100 );
}


