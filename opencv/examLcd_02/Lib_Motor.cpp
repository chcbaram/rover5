#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>




#include "./Main_Lib/Define.h"

#include "./Main_Lib/Uart_Lib.h"
#include "./Main_Lib/Uart_Sig_Lib.h"
#include "./Main_Lib/Thread_Lib.h"                                                                                          
                                                                                                 
#include "./Main_Lib/Uart_Lib.c"
#include "./Main_Lib/Uart_Sig_Lib.c"



int Lib_Motor_Debug = 0;
int Lib_Motor_Enable = 0;

int Lib_Motor_PwmLeft  = 0;
int Lib_Motor_PwmRight = 0;


extern int Thre_Red;
extern int Thre_Bin;
extern int DetectCount;

void Uart_Rxd_Func( char Data )
{
	//printf("%c", Data );
}



void *Lib_Motor(void *Arg)
{
	int  Uart_Handle_Ptr;

	THREAD_OBJ *pArg = (THREAD_OBJ *)Arg;

	//-- 시리얼 통신 초기화 
	Uart_Handle_Ptr = Uart_Open( COM_USB0, BAUD_115200 );

	if( Uart_Handle_Ptr < 0 )
	{
		printf("Uart Open Failed \n");
		pArg->Thread_Ret = 1;
		return (void *)&(pArg->Thread_Ret);	
	}

	Uart_Sig_Init( Uart_Handle_Ptr );
	Uart_Sig_Func_Init( Uart_Handle_Ptr, Uart_Rxd_Func );


	printf("Enter Motor Thread \n");

	while(pArg->Thread_Stop == FALSE)
	{
		if( Lib_Motor_Debug == 1 )
		{
			printf("Pwm %d %d bin:%d  red:%d  Cnt:%03d \n", Lib_Motor_PwmLeft, Lib_Motor_PwmRight, Thre_Bin, Thre_Red, DetectCount );	
		}

		if( Lib_Motor_Enable == 1 )
		{
			Uart_Printf( "move pwm  %d  %d \n", Lib_Motor_PwmLeft, Lib_Motor_PwmRight);
		}
		else
		{
			Uart_Printf( "move pwm  0  0\n" );			
		}
		usleep(100*1000);
	}

	printf("Exit Motor Thread \n");

	Uart_Printf( "move pwm  0  0\n" );


	return (void *)&(pArg->Thread_Ret);	
}




