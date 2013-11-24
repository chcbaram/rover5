
#include <stdio.h>


//-- For OpenCV
//
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>



//#define LIB_INCLUDE
//#define THREAD_LIB_LOCAL

#include "./Main_Lib/KeyBrd_Lib.h"
#include "./Main_Lib/Thread_Lib.h"                                                                                                 
                                                                                                
//#include "./Main_Lib/KeyBrd_Lib.c"
//#include "./Main_Lib/Thread_Lib.c"



extern void *Lib_Vision(void *Arg);


int  Main_Init();
void Sleep(int millisecs);


extern int Lib_Vision_Debug;

extern int Thre_Red;
extern int Thre_Bin;


int main()
{
	int Ch;

	Main_Init();

	printf("OpenCV Red Detect\n");

	Thread_Create( 1, Lib_Vision, NULL  );


	U8 Key_Data;


	KeyBrd_Init();

	Thread_Start(0);
	Thread_Start(1);

	while(1)
	{
		if( KeyBrd_Hit() )
		{
			Key_Data = KeyBrd_Getch();

			if( Key_Data == 'q' ) break;


			if( Key_Data == '2' )	Lib_Vision_Debug ^= 1;
			


			if( Key_Data == 'o' )
			{
				Thre_Bin++;
				printf("Thre_Bin : %d\n", Thre_Bin);
			}	
			if( Key_Data == 'l' )
			{
				Thre_Bin--;
				printf("Thre_Bin : %d\n", Thre_Bin);
			}	


			if( Key_Data == 'i' )
			{
				Thre_Red++;
				printf("Thre_Red : %d\n", Thre_Red);
			}	
			if( Key_Data == 'k' )
			{
				Thre_Red--;
				printf("Thre_Red : %d\n", Thre_Red);
			}	

			
			if( Key_Data == 'w' )
			{
				
			}

			printf("KeyPressed\n");
		}		
	}

	Thread_Stop_All();

	KeyBrd_Close();

	printf("Robot End...\n");
	return 0;
}




int Main_Init(void)
{
	return true;
}



/*
void Sleep(int millisecs)
{
	usleep(millisecs * 1000);
}
*/
