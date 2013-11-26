//----------------------------------------------------------------------------
//    프로그램명 : PWM 관련 함수 헤더
//
//    만든이     : Cho Han Cheol 
//
//    날  짜     : 
//    
//    최종 수정  : 
//
//    MPU_Type   : 
//
//    파일명     : Hw_Pwm.h
//----------------------------------------------------------------------------
                       
#ifndef __HW_PWM_H__
#define __HW_PWM_H__ 




#ifdef   HW_PWM_LOCAL
#define  EXT_HW_PWM_DEF 
#else
#define  EXT_HW_PWM_DEF     extern
#endif


#include "Hw.h"

 


EXT_HW_PWM_DEF void Hw_Pwm_Init(void);


#endif
