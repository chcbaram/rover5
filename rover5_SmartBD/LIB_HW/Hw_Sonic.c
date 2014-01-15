//----------------------------------------------------------------------------
//    프로그램명 	: 초음파 센서 관련 함수
//
//    만든이     	: Cho Han Cheol
//
//    날  짜     	: 2013. 1. 30
//    
//    최종 수정  	: 2013. 1. 30
//
//    MPU_Type	:
//
//    파일명     	: Hw_Sonic.h
//----------------------------------------------------------------------------



//----- 헤더파일 열기
//
#define  HW_SONIC_LOCAL

#include "Hw_Sonic.h"



//-- 내부 선언
//



//-- 내부 변수
//



//-- 내부 함수
//



/*---------------------------------------------------------------------------
     TITLE   : Hw_Sonic_ISR
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_Sonic_ISR(void)
{
	u16	TimerWidth;

    if ( REG_EXTI_PR & (1<<14) )
    {

    	if( Hw_Sonic_Log[0].Edge == 0 )
    	{
    		Hw_Sonic_Log[0].CaptureTimerValueStart = Hw_Timer_Get_u16Count_Usec();

    	}
    	if( Hw_Sonic_Log[0].Edge == 1 )
    	{
    		Hw_Sonic_Log[0].CaptureTimerValueEnd = Hw_Timer_Get_u16Count_Usec();

    		TimerWidth = Hw_Sonic_Log[0].CaptureTimerValueEnd - Hw_Sonic_Log[0].CaptureTimerValueStart;

    		if( TimerWidth > 150 )
    		{
    			Hw_Sonic_Log[0].CaptureTimerWidth = TimerWidth;
    			Hw_Sonic_Log[0].DistanceMM        = TimerWidth*332/2000;
    		}
    	}

    	Hw_Sonic_Log[0].Edge++;

        //-- 인터럽트 팬딩 레지스터 Clear
		//
		REG_EXTI_PR = (1<<14);

    }

    if ( REG_EXTI_PR & (1<<15) )
    {
    	if( Hw_Sonic_Log[1].Edge == 0 )
    	{
    		Hw_Sonic_Log[1].CaptureTimerValueStart = Hw_Timer_Get_u16Count_Usec();

    	}
    	if( Hw_Sonic_Log[1].Edge == 1 )
    	{
    		Hw_Sonic_Log[1].CaptureTimerValueEnd = Hw_Timer_Get_u16Count_Usec();

    		TimerWidth = Hw_Sonic_Log[1].CaptureTimerValueEnd - Hw_Sonic_Log[1].CaptureTimerValueStart;

    		if( TimerWidth > 150 )
    		{
    			Hw_Sonic_Log[1].CaptureTimerWidth = TimerWidth;
    			Hw_Sonic_Log[1].DistanceMM        = TimerWidth*332/2000;
    		}
    	}

    	Hw_Sonic_Log[1].Edge++;

        //-- 인터럽트 팬딩 레지스터 Clear
		//
		REG_EXTI_PR = (1<<15);
    }
}





void Hw_Sonic_Tick( void )
{
	u8 Ret;
	u8 i;
	static u8 Cnt = 0;

	for( i=0; i<HW_SONIC_MAX_CH; i++ )
	{
		switch( Cnt )
		{
			case 0:
				Hw_Sonic_SetPortHIGH(i);
				break;

			case 1:
				Hw_Sonic_SetPortLOW(i);
				Hw_Sonic_SetPortDirIN(i);
				Hw_Sonic_Log[i].Edge = 0;
				Hw_Sonic_ExIntEnable(i);
				break;

			case 19:
				Hw_Sonic_ExIntDisable(i);
				Hw_Sonic_SetPortDirOUT(i);
				Hw_Sonic_SetPortLOW(i);
				break;
		}
	}
	Cnt++;

	if( Cnt >= 50 ) Cnt = 0;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_Sonic_Init
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_Sonic_Init( void )
{
	u16 i;



    //-- 초음파 센서 핀설정.


    for( i=0; i<HW_SONIC_MAX_CH; i++ )
    {
    	Hw_Sonic_SetPortDirOUT(i);
    	Hw_Sonic_SetPortLOW(i);

    	Hw_Sonic_Log[i].Enable				   = 0;
    	Hw_Sonic_Log[i].Edge 				   = 0;
    	Hw_Sonic_Log[i].CaptureTimerValueStart = 0;
    	Hw_Sonic_Log[i].CaptureTimerValueEnd   = 0;
    	Hw_Sonic_Log[i].CaptureTimerWidth	   = 0;
    	Hw_Sonic_Log[i].DistanceMM			   = 0;
    }



	// 1. 인터럽트 핀 설정 
	//
	REG_AFIO_EXTICR4 &= ~(0x0F<<8);
	REG_AFIO_EXTICR4 |=  (0x02<<8);		// EXTI14, PC.14를 외부 인터럽트로 설정 

	REG_AFIO_EXTICR4 &= ~(0x0F<<12);
	REG_AFIO_EXTICR4 |=  (0x02<<12);	// EXTI15, PC.15를 외부 인터럽트로 설정 


	// 2. Rising & Falling trigger로 설정
	// 
	REG_EXTI_FTSR |= (1<<14) | (1<<15);
	REG_EXTI_RTSR |= (1<<14) | (1<<15);
	 

	Hw_ISR_SetIRQFunc( 40, (u32)Hw_Sonic_ISR, 0 );	
	SET_BIT( REG_NVIC_ISER( 40/32 ), 40%32 );	// NVIC 40번 EXTI15:10 인터럽트 활성화


    // EXT
    //
    Hw_Sonic_ExIntEnable(0);
    Hw_Sonic_ExIntDisable(1);

	Hw_Timer_Set  ( HW_TIMER_CH_SONIC, 1, LOOP_TIME, Hw_Sonic_Tick, NULL );
	Hw_Timer_Start( HW_TIMER_CH_SONIC );

}





/*---------------------------------------------------------------------------
     TITLE   : Hw_Sonic_ExIntEnable
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_Sonic_ExIntEnable( u8 Ch )
{

	switch( Ch )
	{
		case 0:
			// EXTI14 Enable
			// 
			REG_EXTI_EMR |= (1<<14);	// Event Enable
			REG_EXTI_IMR |= (1<<14);	// Interrupt nable	
			break;

		case 1:
			// EXTI15 Enable
			// 
			REG_EXTI_EMR |= (1<<15);	// Event Enable
			REG_EXTI_IMR |= (1<<15);	// Interrupt nable	
			break;
	}
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_Sonic_ExIntDisable
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_Sonic_ExIntDisable( u8 Ch )
{

	switch( Ch )
	{
		case 0:
			// EXTI14 Disable
			// 
			REG_EXTI_EMR &= ~(1<<14);	// Event Enable
			REG_EXTI_IMR &= ~(1<<14);	// Interrupt nable
			break;
		case 1:
			// EXTI15 Disable
			// 
			REG_EXTI_EMR &= ~(1<<15);	// Event Enable
			REG_EXTI_IMR &= ~(1<<15);	// Interrupt nable
			break;
	}
}






/*---------------------------------------------------------------------------
     TITLE   : Hw_Sonic_SetPortDirIN
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_Sonic_SetPortDirIN( u8 Ch )
{


	switch( Ch )
	{
		case 0:
			REG_GPIOC_CRH &= ~(0x0F << ((14-8)*4+0));	// Clear
			REG_GPIOC_CRH |=  (0x00 << ((14-8)*4+0));	// MODE, PC.14 Input Mode
			REG_GPIOC_CRH |=  (0x02 << ((14-8)*4+2));	// CNF,  PC.14 Pull Up/DOwn

			SET_BIT( REG_GPIOC_ODR, 14 );	
			break;

		case 1:
			REG_GPIOC_CRH &= ~(0x0F << ((15-8)*4+0));	// Clear
			REG_GPIOC_CRH |=  (0x00 << ((15-8)*4+0));	// MODE, PC.15 Input Mode
			REG_GPIOC_CRH |=  (0x02 << ((15-8)*4+2));	// CNF,  PC.15 Pull Up/DOwn

			SET_BIT( REG_GPIOC_ODR, 15 );	
			break;
	}
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_Sonic_SetPortDirOUT
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_Sonic_SetPortDirOUT( u8 Ch )
{


	switch( Ch )
	{
		case 0:
			REG_GPIOC_CRH &= ~(0x0F << ((14-8)*4+0));	// Clear
			REG_GPIOC_CRH |=  (0x03 << ((14-8)*4+0));	// MODE, PC.14 Output Mode
			REG_GPIOC_CRH |=  (0x00 << ((14-8)*4+2));	// CNF,  PC.14 Push Pull		    
			break;

		case 1:
			REG_GPIOC_CRH &= ~(0x0F << ((15-8)*4+0));	// Clear
			REG_GPIOC_CRH |=  (0x03 << ((15-8)*4+0));	// MODE, PC.15 Output Mode
			REG_GPIOC_CRH |=  (0x00 << ((15-8)*4+2));	// CNF,  PC.15 Push Pull		    
			break;
	}
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_Sonic_SetPortHIGH
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_Sonic_SetPortHIGH( u8 Ch )
{
	switch( Ch )
	{
		case 0:
			SET_BIT( REG_GPIOC_ODR, 14 );
			break;

		case 1:
			SET_BIT( REG_GPIOC_ODR, 15 );
			break;
	}
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_Sonic_SetPortLOW
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void Hw_Sonic_SetPortLOW( u8 Ch )
{
	switch( Ch )
	{
		case 0:
			SET_BIT( REG_GPIOC_ODR, 14 );
			break;

		case 1:
			SET_BIT( REG_GPIOC_ODR, 15 );
			break;
	}
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_Sonic_GetPort
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u8 Hw_Sonic_GetPort( u8 Ch )
{
	u8 Ret = 0;

	switch( Ch )
	{
		case 0:
			if( REG_GPIOC_IDR & (1<<14) ) Ret = 1;
			break;

		case 1:
			if( REG_GPIOC_IDR & (1<<15) ) Ret = 1;
			break;
	}


	return Ret;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_Sonic_GetDistanceTime
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_Sonic_GetDistanceTime( u8 Ch )
{
	return Hw_Sonic_Log[Ch].CaptureTimerWidth;
}





/*---------------------------------------------------------------------------
     TITLE   : Hw_Sonic_GetDistanceMM
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
u16 Hw_Sonic_GetDistanceMM( u8 Ch )
{
	return Hw_Sonic_Log[Ch].DistanceMM;
}


