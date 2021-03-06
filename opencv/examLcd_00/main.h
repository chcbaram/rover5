

#ifndef _MAIN_H_
#define _MAIN_H_


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>



#include "./Main_Lib/Define.h"


#define IMG_WIDTH			320	
#define IMG_HEIGHT			240		

#define RESIZE_FACTOR		2

#define RESIZE_IMG_WIDTH	(320/RESIZE_FACTOR)		
#define RESIZE_IMG_HEIGHT	(240/RESIZE_FACTOR)		
#define RESIZE_IMG_FPS		20


extern void Sleep(int millisecs);

#endif