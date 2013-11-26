//----------------------------------------------------------------------------
//    프로그램명 	: Util 관련 함수
//
//    만든이     	: 
//
//    날  짜     	: 
//    
//    최종 수정  	: 
//
//    MPU_Type		:
//
//    파일명     	: Lb_Util.h
//----------------------------------------------------------------------------



//----- 헤더파일 열기
//
#define  LB_UTIL_LOCAL

#include "Lb_Util.h"

#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>



//-- 내부 선언
//



//-- 내부 변수
//
static const char *delim = " \f\n\r\t\v";



//-- 내부 함수
//






void delay(volatile unsigned int timeCount)
{
    while(timeCount --);
}

void delay_second(void)
{
    delay(806596);
}





/*---------------------------------------------------------------------------
     TITLE   : Lb_Util_PaseArgs
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
int Lb_Util_PaseArgs(char *cmdline, char **argv)
{
	char *tok;
	int argc = 0;

	argv[argc] = NULL;

	for (tok = strtok(cmdline, delim); tok; tok = strtok(NULL, delim))
	{
		argv[argc++] = tok;
	}

	return argc;
}





/*---------------------------------------------------------------------------
     TITLE   : Lb_Util_atoi
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
int Lb_Util_atoi(char *str)
{
	int i = 0, j = 0;
 
    if (*str == '-' || isdigit(*str)) 
    {
		if(*str == '-') j = 1, str++;

		while(isdigit(*str))
		{
			i = i * 10 + (*(str)) - 48;
			str++;
		}
		if (j == 1) return -i;
		else 		return i;
	}
	else
		return 0;
}





#if 1
uint32_t __get_MSP(void) __attribute__( ( naked ) );
uint32_t __get_MSP(void)
{
  uint32_t result=0;

  __asm volatile ("MRS %0, msp\n\t" 
                  "MOV r0, %0 \n\t"
                  "BX  lr     \n\t"  : "=r" (result) );
  return(result);
}


/*
 sbrk
 Increase program data space.
 Malloc and related functions depend on this
 */
//caddr_t _sbrk(int incr) {
 void *_sbrk(int incr) { 

    extern char _ebss; // Defined by the linker
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_ebss;
    }
    prev_heap_end = heap_end;

char * stack = (char*) __get_MSP();
     if (heap_end + incr >  stack)
     {
         //_write (STDERR_FILENO, "Heap and stack collision\n", 25);
         errno = ENOMEM;
         //return  (caddr_t) -1;
         return (void *)-1;
         //abort ();
     }

    heap_end += incr;
    //return (caddr_t) prev_heap_end;
    return (void *) prev_heap_end;

}
#endif