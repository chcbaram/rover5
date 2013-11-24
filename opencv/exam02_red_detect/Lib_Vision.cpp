
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>

#include "main.h"
#include "./Main_Lib/Thread_Lib.h"  


#include "cv.h"
#include "highgui.h"



clock_t	start_point, end_point; 
clock_t start_point_frame;
clock_t end_point_frame;




#define _DEBUG_WINDOW_		0
#define _DEBUG_LABELING_	0


#define MAX_LABEL_ID      	254

#define _FALSE 0
#define _TRUE  1

#define BINARY_THRESHOLD		155
#define RED_THRESHOLD	    	80
#define LABEL_SIZE_THRESHOLD	20

typedef struct
{
	int numPixel;
	int id;
} LabelST;





int Lib_Vision_Debug = 0;


//-- OpenNI 변수 
//
IplImage *IplImage_depth;
IplImage *IplImage_color;

IplImage *IplImage_depth_resize;
IplImage *IplImage_color_resize;



	
cv::Mat MatImage_depth;
cv::Mat MatImage_color;




int CapturedFlag = 0;


double 	process_time;
int 	process_rate;
double  process_time_depth;
double  process_time_color;
double  process_time_frame;


int Thre_Red = RED_THRESHOLD;
int Thre_Bin = BINARY_THRESHOLD;


int BlobLabeling(IplImage *imSRC, IplImage *imLabel, LabelST *stLabel, int *countLabel);
int GetMainBlob(IplImage *imLabel, LabelST *stLabel, int countLabel, int *pMainBlobIndex);


int Tracking_Red( THREAD_OBJ *pArg );



extern int Lib_Motor_PwmLeft;
extern int Lib_Motor_PwmRight;


void *Lib_Vision(void *Arg)
{
	THREAD_OBJ *pArg = (THREAD_OBJ *)Arg;

	printf("Enter Vision \n");


	pArg->Thread_Ret = Tracking_Red( pArg );


	printf("Exit Vision Ret : %d\n", pArg->Thread_Ret);

	return (void *)&(pArg->Thread_Ret);
}





int BlobLabeling(IplImage *imSRC, IplImage *imLabel, LabelST *stLabel, int *countLabel);
int GetMainBlob(IplImage *imLabel, LabelST *stLabel, int countLabel);



int Tracking_Red( THREAD_OBJ *pArg )
{
	IplImage *frame = NULL;//NULL « ºˆ.
	IplImage *image;
	IplImage *imgRed = NULL;
	IplImage *imgBlue = NULL;
	IplImage *imgGreen = NULL;
	IplImage *imgBin = NULL;
	IplImage *imgMorph = NULL;
	IplImage *imgLabel   = NULL;
	IplImage *imgResize   = NULL;

	LabelST stLabel[MAX_LABEL_ID];
	int countLabel;
	char strLine[255];
	int  ObjDistance;	
	int  MainBlobIndex;

	// Create capture device ready
	// here 0 indicates that we want to use camera at 0th index
	CvCapture *capture = cvCaptureFromCAM(0);
	
	printf("1 \n");

	// 캡쳐 이미지 사이즈 설정
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH	, IMG_WIDTH);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT	, IMG_HEIGHT);


	image 		= cvCreateImage(cvSize(RESIZE_IMG_WIDTH,RESIZE_IMG_HEIGHT), 8, 3);
	imgBlue 	= cvCreateImage(cvSize(RESIZE_IMG_WIDTH,RESIZE_IMG_HEIGHT), 8, 1);
	imgGreen 	= cvCreateImage(cvSize(RESIZE_IMG_WIDTH,RESIZE_IMG_HEIGHT), 8, 1);
	imgRed 		= cvCreateImage(cvSize(RESIZE_IMG_WIDTH,RESIZE_IMG_HEIGHT), 8, 1);
	imgBin 		= cvCreateImage(cvSize(RESIZE_IMG_WIDTH,RESIZE_IMG_HEIGHT), 8, 1);
	imgMorph 	= cvCreateImage(cvSize(RESIZE_IMG_WIDTH,RESIZE_IMG_HEIGHT), 8, 1);
	imgLabel 	= cvCreateImage(cvSize(RESIZE_IMG_WIDTH,RESIZE_IMG_HEIGHT), 8, 1);
	imgResize 	= cvCreateImage(cvSize(RESIZE_IMG_WIDTH,RESIZE_IMG_HEIGHT), 8, 3);
	
	

    CvFont font;
    //cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.8, 0.8, 0, 1, CV_AA);
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5);
    
	

	while ( pArg->Thread_Stop == FALSE )
	{
		// 카메라로 부터 1프레임 이미지를 읽어온다.
		frame = cvQueryFrame(capture);

		cvResize(frame, imgResize);

		// YCrCb 형태로 변경
		//cvCvtColor(frame, image, CV_RGB2YCrCb);
		cvCvtColor(imgResize, image, CV_RGB2YCrCb);

		//√§≥Œ ∫–∏Æ, red color
		cvSplit(image, imgBlue, imgGreen, imgRed, NULL);
		//RED
		cvMaxS(imgRed, Thre_Red, imgRed);
		//¿Ã¡¯»≠
		cvThreshold(imgRed, imgBin, Thre_Bin, 255, CV_THRESH_BINARY);
		
		// ∆ÿ√¢
		cvDilate(imgBin, imgMorph, NULL, 4);
		
		// ƒßΩƒ
		cvErode(imgMorph, imgMorph, NULL, 4);
		

		

		
		double M;
		int x_order;
		int y_order;
		double cX, cY;
		double m00;
		int binary = 1;
		int DetectObj = _FALSE;
		int ObjDistance = 0;

		MainBlobIndex = 0;

		//Labeling
		if(BlobLabeling(imgMorph, imgLabel, stLabel, &countLabel) == _FALSE);
		else
		{
			//Tracking
			DetectObj = GetMainBlob(imgLabel, stLabel, countLabel, &MainBlobIndex );
			
			// 물체의 거리를 구한다.
			//
			//ObjDistance = GetObjDistance(imgLabel);
			ObjDistance = 0;

			//Tracking 
			CvMoments moments;
			cvMoments(imgLabel, &moments, binary);
			
			for( y_order=0; y_order<=3; y_order++ )
			{
				for( x_order=0; x_order<=3; x_order++ )
				{
					if( x_order+y_order > 3 )
					{
						continue;
					}
					
					M = cvGetSpatialMoment( &moments, x_order, y_order );
					
					if( x_order == 0 && y_order == 0 )
					{
						m00 = M;
					}
					else if( x_order == 1 && y_order == 0 )
					{
						cX = M;
					}
					else if( x_order == 0 && y_order == 1 )
					{
						cY = M;
					}
				}
			}
			
			cX = cX / m00;
			cY = cY / m00;
		}
		
		/*
		if( DetectObj == _TRUE && ObjDistance > 0 )
		{
			int x_offset;
			int z_offset;
			int   M_Speed  = 0;
			float M_Handle = 0.0;

			x_offset = cX - (IMG_WIDTH/2);
			z_offset = -(900 - ObjDistance);

			M_Speed = z_offset / 10;

			M_Handle = abs(M_Speed) * (float)x_offset/100.;


			Lib_Motor_PwmLeft  = M_Speed + M_Handle;
			Lib_Motor_PwmRight = M_Speed - M_Handle; 


		}
		else
		{
			Lib_Motor_PwmLeft  = 0;
			Lib_Motor_PwmRight = 0; 
		}
		*/
	


		if( DetectObj == _TRUE )
		{

			cvCircle(frame, cvPoint(cvRound(cX*RESIZE_FACTOR), cvRound(cY*RESIZE_FACTOR)), 20, CV_RGB(255, 0, 0), 2);
			cvCircle(imgBin, cvPoint(cvRound(cX), cvRound(cY)), 20, CV_RGB(255, 255, 255), 2);
			cvCircle(imgRed, cvPoint(cvRound(cX), cvRound(cY)), 20, CV_RGB(255, 255, 255), 2);
			//cvCircle(IplImage_depth, cvPoint(cvRound(cX*1.06), cvRound(cY*1.04)), 20, CV_RGB(255, 255, 255), 2);
		
			sprintf(strLine, "%d", stLabel[MainBlobIndex].numPixel);
			cvPutText(frame, strLine, cvPoint(160/2+10, 40), &font, cvScalar(0, 0, 255, 0));

		}
		else
		{
			cX = 0;
			cY = 0;

			cvPutText(frame, "No Red", cvPoint(160/2+10, 20), &font, cvScalar(0, 0, 255, 0));			
		}			

		

		//-- 영상 출력
		//
		//cvSaveImage("/mnt/ramdisk/img1/depth.jpg",frame);  
		//cvSaveImage("/mnt/ramdisk/color/color.jpg",imgLabel); 
		//cvSaveImage("/mnt/ramdisk/img2/color.jpg",imgBin); 

		//cvResize(IplImage_depth, IplImage_depth_resize);
		//cvResize(frame, IplImage_color_resize);
		//cvSaveImage("/mnt/ramdisk/depth/depth.jpg",IplImage_depth_resize);  
		//cvSaveImage("/mnt/ramdisk/color/color.jpg",IplImage_color_resize); 		
		

		end_point = clock();
		process_time = ((double)(end_point - start_point)/(CLOCKS_PER_SEC/1000));

		if( Lib_Vision_Debug == 1 )
		{
			printf("Exe time : %04f msec  %03.1f frames  x : %d, y %d \r\n", process_time, 1000./process_time, (int)cX, (int)cY ); 
		}
		
		start_point = clock();	
	}
	
	cvReleaseImage(&frame);
	cvReleaseImage(&image);
	cvReleaseImage(&imgRed);
	cvReleaseImage(&imgBlue);
	cvReleaseImage(&imgGreen);
	cvReleaseImage(&imgBin);
	cvReleaseImage(&imgMorph);
	cvReleaseImage(&imgLabel);
	cvReleaseImage(&imgResize);
	

	return 0;
}



int BlobLabeling(IplImage *imSRC, IplImage *imLabel, LabelST *stLabel, int *pCountLabel)
{
	int ix, iy, ig;
	int arNumBuf[MAX_LABEL_ID] = {0, };
	int groupBuf[9] = { 0, };
	int minLabel;
	int maxLabel;
	int bufLabel;
	int curLabel;
	int outflag;
	int width, height;
	unsigned char *pImg   = (unsigned char *)imSRC->imageData;
	unsigned char *pLabel = (unsigned char *)imLabel->imageData;
	int yOffM1, yOff, yOffP1;
	int countLabel;
	
	
	width  = imSRC->width;
	height = imSRC->height;
	
	
	// ∑π¿Ã∫Ì øµªÛ √ ±‚»≠
	for(iy=0; iy<height; iy++)
	{
		yOff   = iy * width;
		for(ix=0; ix<width; ix++)
		{
			pLabel[yOff + ix] = 255;
		}
	}
	for(ig=0; ig<MAX_LABEL_ID; ig++)
	{
		stLabel[ig].id         = 0;
		stLabel[ig].numPixel = 0;
		arNumBuf[ig] = 0;
	}
	
	
	// 1¬˜  ∑π¿Ã∫Ì ºˆ«‡
	maxLabel = -1;
	for(iy=1; iy<height-1; iy++)
	{
		yOffM1 = (iy-1) * width;
		yOff   = iy * width;
		
		for(ix=1; ix<width-1; ix++)
		{
			if(pImg[yOff+ix] == 255)
			{
				groupBuf[0] = pLabel[yOffM1+ix-1];
				groupBuf[1] = pLabel[yOffM1+ix  ];
				groupBuf[2] = pLabel[yOffM1+ix+1];
				groupBuf[3] = pLabel[yOff  +ix-1];
				
				minLabel = 255;
				for(ig=0; ig<4; ig++)
				{
					bufLabel = groupBuf[ig];
					if(bufLabel < minLabel)
						minLabel = bufLabel;
				}
				if(minLabel == 255)
				{
					maxLabel++;
					pLabel[yOff + ix] = maxLabel;
				}
				else
					pLabel[yOff + ix] = minLabel;
			}
		}
	}
	
	if(maxLabel == -1)
		return _FALSE;
	
	
	while(1)
	{
		outflag = 1;
		for(iy=1; iy<height-1; iy++)
		{
			yOffM1 = (iy-1) * width;
			yOff   = iy * width;
			yOffP1 = (iy+1) * width;
			for(ix=1; ix<width-1; ix++)
			{
				if(pImg[yOff + ix] == 255)
				{
					groupBuf[0] = pLabel[yOffM1 + ix-1];
					groupBuf[1] = pLabel[yOffM1 + ix  ];
					groupBuf[2] = pLabel[yOffM1 + ix+1];
					groupBuf[3] = pLabel[yOff   + ix-1];
					groupBuf[4] = pLabel[yOff   + ix  ];
					groupBuf[5] = pLabel[yOff   + ix+1];
					groupBuf[6] = pLabel[yOffP1 + ix-1];
					groupBuf[7] = pLabel[yOffP1 + ix  ];
					groupBuf[8] = pLabel[yOffP1 + ix+1];
					
					curLabel = groupBuf[4];
					minLabel = 255;
					for(ig=0; ig<9; ig++)
					{
						bufLabel = groupBuf[ig];
						if(bufLabel < minLabel)
							minLabel = bufLabel;
					}
					
					if(minLabel == 255 || curLabel == minLabel);
					else
					{
						pLabel[yOff + ix] = minLabel;
						outflag = 0;
					}
				}
			}
		}
		if(outflag == 1)
			break;
	}
	
	// ∑π¿Ã∫Ì º≥¡§.
	for(iy=1; iy<height-1; iy++)
	{
		yOff   = iy * width;
		for(ix=1; ix<width-1; ix++)
		{
			arNumBuf[pLabel[yOff + ix]]++;
		}
	}
	
	countLabel = 0;
	for(ig=0;ig<MAX_LABEL_ID; ig++)
	{
		if(arNumBuf[ig] != 0)
		{
			stLabel[countLabel].id       = ig;
			stLabel[countLabel].numPixel = arNumBuf[ig];
			countLabel++;
		}
	}
	
	if(countLabel == 0)
		return _FALSE;
	
	*pCountLabel = countLabel;
	
#if _DEBUG_WINDOW_ == 1
#if _DEBUG_LABELING_ == 1
	int arID[MAX_LABEL_ID] = {0, };
	int count;
	
	for(ig=0; ig<MAX_LABEL_ID; ig++)
	{
		arID[ig] = 0;
	}
	
	count = 0;
	
	for(ig=0;ig<MAX_LABEL_ID; ig++)
	{
		if(arNumBuf[ig] != 0)
		{
			arID[ig] = count;
			count++;
		}
	}
	
	for(iy=1; iy<height-1; iy++)
	{
		yOff   = iy * width;
		for(ix=1; ix<width-1; ix++)
		{
			if(pLabel[yOff + ix] == 255);
			else
				pLabel[yOff + ix] = 255/(count+1)*arID[pLabel[yOff+ix]];
		}
	}
#endif
#endif
	
	return _TRUE;
}


int GetMainBlob(IplImage *imLabel, LabelST *stLabel, int countLabel, int *pMainBlobIndex)
{
	int ix, iy, ig;
	int countBuf;
	int width, height;
	unsigned char *pLabel = (unsigned char *)imLabel->imageData;
	int yOff;
	int max = 0;
	int mainID;
	int mainIx = 0;
	int countMainID;

	width = imLabel->width;
	height = imLabel->height;
	
	if(countLabel == 1)
		mainID = 0;
	else
	{
		ig = 0;
		ix = 0;
		max = 0;
		for(ix=0; ix<countLabel; ix++)
		{
			countBuf = stLabel[ix].numPixel;
			if(countBuf > max)
			{
				max = countBuf;
				mainID = stLabel[ix].id;
				mainIx = ix;
			}
		}
	}
	
	countMainID = 0;

	for(iy=0; iy<height; iy++)
	{
		yOff = iy*width;
		for(ix=0; ix<width; ix++)
		{
			if(pLabel[yOff + ix] == mainID)
			{
				pLabel[yOff + ix] = 255;
				countMainID++;
			}
			else
				pLabel[yOff + ix] = 0;
		}
	}
	

	*pMainBlobIndex = mainIx;

	//if( max > LABEL_SIZE_THRESHOLD )
	if( countMainID > LABEL_SIZE_THRESHOLD )
	{
		return _TRUE;
	}
	else
	{
		return _FALSE;
	}
}
