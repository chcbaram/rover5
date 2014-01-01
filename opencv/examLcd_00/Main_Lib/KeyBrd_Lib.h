//-----------------------------------------------------------------------------------------------
//            TITLE       : KeyBoard 처리 함수 헤더`
//            WORK        :
//            DATE        : 2003. 7. 3
//            FILE        : KeyBrd_Lib.h
//            Modified By : Hanchol Cho     - IRSL -
//----------------------------------------------------------------------------------------------


#ifndef  _KEYBRD_LIB_H_
#define  _KEYBRD_LIB_H_    


#ifdef   KEYBRD_LIB_LOCAL
#define  EXT_KEYBRD_LIB_DEF 
#else
#define  EXT_KEYBRD_LIB_DEF     extern
#endif

#ifdef   LIB_INCLUDE
#undef  EXT_KEYBRD_LIB_DEF
#define EXT_KEYBRD_LIB_DEF
#endif


#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>                                                                                             

                                                                                                 
                                                                                                 
//-----------------------------------------------------------------------------------------------//            TITLE   : 변수 선언
//            WORK    :
//            DATE    : 2003. 2. 27
//----------------------------------------------------------------------------------------------
                                                                                                 

EXT_KEYBRD_LIB_DEF void KeyBrd_Init( void );
EXT_KEYBRD_LIB_DEF void KeyBrd_Close( void );
EXT_KEYBRD_LIB_DEF int  KeyBrd_Hit( void );
EXT_KEYBRD_LIB_DEF int  KerBrd_Getch_Buf( void );
EXT_KEYBRD_LIB_DEF char KeyBrd_Getch( void );



#endif                                                                                                 

