
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>


#include "main.h"
#include "./Main_Lib/Thread_Lib.h"  


#include "cv.h"
#include "highgui.h"



clock_t	start_point, end_point; 
clock_t start_point_frame;
clock_t end_point_frame;




#define _DEBUG_WINDOW_		0
#define _DEBUG_LABELING_	0


#define MAX_LABEL_ID      	5	// 254

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



//-- fb 
//
int      fbfd = 0;
struct   fb_var_screeninfo vinfo;
struct   fb_fix_screeninfo finfo;
long int fb_screensize = 0;
char     *fbp = 0;




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
int DetectCount = 0;


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



int fb_open(void)
{
	int x;
	int y;
	int y_offset;

	fbfd = open("/dev/fb0", O_RDWR);
	if (!fbfd) 
	{
		printf("Error: cannot open framebuffer device.\n");
		return FALSE;
	}

	printf("The framebuffer device was opened successfully.\n");

	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) 
	{
		printf("Error reading fixed information.\n");
		return FALSE;
	}

	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) 
	{
		printf("Error reading variable information.\n");
		return FALSE;
	}

	fb_screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	fbp = (char *)mmap(0, fb_screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if ((int)fbp == -1) 
	{
		printf("Error: failed to map framebuffer device to memory.\n");
		return FALSE;
	}	

	unsigned short *pfb     = (unsigned short *)fbp;


	for( y=0; y<vinfo.yres; y++ )
	{
		y_offset = (y) * vinfo.xres;
				
		for( x=0; x<vinfo.xres; x++ )
		{
			pfb[y_offset + x] = 0x00;
		}
	}

	return TRUE;
}


void fb_close(void)
{
	if( fbp != (char *)-1 )
	{
		munmap(fbp, fb_screensize);
		close(fbfd);
	}	
}


void fb_DrawImage( int x, int y, int Rotate, IplImage *pImg )
{
	int  y_offset;
	int  x_pos;
	int  y_pos;
	int  x_pixel;
	int  y_pixel;

	unsigned short *pImgLcd = (unsigned short *)pImg->imageData;
	unsigned short *pfb     = (unsigned short *)fbp;

	pImgLcd = (unsigned short *)pImg->imageData;
	pfb     = (unsigned short *)fbp;




	if( Rotate == 0 )
	{
		x_pos = x;
		y_pos = y;

		for( y=0; y<pImg->height; y++ )
		{
			y_offset = (y+y_pos) * vinfo.xres;
				
			for( x=0; x<pImg->width; x++ )
			{
				pfb[y_offset + x + x_pos] = pImgLcd[ y*pImg->width + x];
			}
		}
	}
	else
	if( Rotate == 90 )
	{
		x_pos = x;
		y_pos = y;

		for( y=0; y<pImg->height; y++ )
		{
			y_offset = (y+y_pos) * vinfo.xres;
				
			for( x=0; x<pImg->width; x++ )
			{
				x_pixel = vinfo.xres - (y+y_pos);
				y_pixel = (x+x_pos) * vinfo.xres;
				
				pfb[y_pixel + x_pixel] = pImgLcd[ y*pImg->width + x];
			}
		}

	}
	else
	if( Rotate == -90 )
	{
		x_pos = x;
		y_pos = y;

		for( y=0; y<pImg->height; y++ )
		{
			y_offset = (y+y_pos) * vinfo.xres;
				
			for( x=0; x<pImg->width; x++ )
			{
				x_pixel = y+y_pos;
				y_pixel = (vinfo.yres - (x+x_pos)) * vinfo.xres;
				
				pfb[y_pixel + x_pixel] = pImgLcd[ y*pImg->width + x];
			}
		}

	}
}





void fb_DrawHistogram( int x, int y, int Rotate, IplImage* img, int thre_line )
{
	IplImage* canvas;
	IplImage* canvas_lcd;

	int       hist[256];
	double    scale = 1;
	int       i, j, max = 0;




	canvas     = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 1);
	canvas_lcd = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 2);

	cvSet(canvas, CV_RGB(255, 255, 255), 0);


	//-- hist 초기화 및 각 밝기의 빈도수 계산
	//
	for (i = 0; i < 256; i++)
	{
		hist[i] = 0;
	}
	
	for (i = 0; i < img->height; i++)
	{
		for (j = 0; j < img->width; j++)
		{
			hist[(uchar)img->imageData[(i * img->widthStep) + j]]++;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// max : 가장 많이 나온 밝기의 개수
	// 동일한 scale로 여러 이미지의 histogram을 보고싶을 경우
	// max 값을 임의로 고정시키면 된다
	// 여기서는 최대로 나온 개수를 256*60개라고 임의로 지정

	//max = 256 * 50;		


	//////////////////////////////////////////////////////////////////////////
	// 화면을 꽉 채워서 보고싶으면 아래의 for문을 이용해 hist 중 최대 값을 찾는다

	//*
	for (i = 0; i < 256; i++)
	{
		max = hist[i] > max ? hist[i] : max;
	}
	//*/
	

	//-- canvas 창에 맞도록 scale 값 설정
	//
	scale = max > canvas->height ? (double)canvas->height/max : 1.;


	//-- canvas에 세로 줄을 그림
	//
	for (i = 0; i < 256; i++)
	{
		CvPoint pt1 = cvPoint(i*canvas->width/255, canvas->height - (hist[i] * scale));
		CvPoint pt2 = cvPoint(i*canvas->width/255, canvas->height);
		cvLine(canvas, pt1, pt2, CV_RGB(0,0,0), 1, 8, 0);
	}

	CvPoint pt1 = cvPoint(thre_line*canvas->width/255, 0);
	CvPoint pt2 = cvPoint(thre_line*canvas->width/255, canvas->height);
	cvLine(canvas, pt1, pt2, CV_RGB(0,0,0), 1, 8, 0);


	cvCvtColor( canvas, canvas_lcd, CV_GRAY2BGR565 ); 
	fb_DrawImage( x, y, Rotate, canvas_lcd );	


	cvReleaseImage(&canvas);
	cvReleaseImage(&canvas_lcd);
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
	IplImage *imgGray = NULL;	
	IplImage *imgBin = NULL;
	IplImage *imgMorph = NULL;
	IplImage *imgLabel   = NULL;
	IplImage *imgResize   = NULL;
	IplImage *imgLcd   = NULL;
	IplImage *imgLcd_Resize   = NULL;



	LabelST stLabel[MAX_LABEL_ID];
	int countLabel;
	char strLine[255];
	int  ObjDistance;	
	int  MainBlobIndex;
	int  rx;
	int  ry;
	int  i;
	int  x;
	int  y;
	int  y_offset;
	int  x_pos;
	int  y_pos;

	double gray_thre;


	fb_open();


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
	imgGray		= cvCreateImage(cvSize(RESIZE_IMG_WIDTH,RESIZE_IMG_HEIGHT), 8, 1);	
	imgBin 		= cvCreateImage(cvSize(RESIZE_IMG_WIDTH,RESIZE_IMG_HEIGHT), 8, 1);
	imgMorph 	= cvCreateImage(cvSize(RESIZE_IMG_WIDTH,RESIZE_IMG_HEIGHT), 8, 1);
	imgLabel 	= cvCreateImage(cvSize(RESIZE_IMG_WIDTH,RESIZE_IMG_HEIGHT), 8, 1);
	imgResize 	= cvCreateImage(cvSize(RESIZE_IMG_WIDTH,RESIZE_IMG_HEIGHT), 8, 3);
	
	imgLcd 		= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 2);
	imgLcd_Resize 	= cvCreateImage(cvSize(RESIZE_IMG_WIDTH,RESIZE_IMG_HEIGHT), 8, 2);
	

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
		//cvCvtColor(imgResize, image, CV_RGB2YCrCb);
		cvCvtColor(imgResize, imgGray, CV_RGB2GRAY);

		//√§≥Œ ∫–∏Æ, red color
		//cvSplit(image, imgGray, imgBlue, imgRed, NULL);
		//cvSplit(image, imgBlue, imgGray, imgRed, NULL);
		//RED
		//cvMaxS(imgRed, Thre_Red, imgRed);

		//¿Ã¡¯»≠
		
		gray_thre = cvThreshold(imgGray, imgBin, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

		//Thre_Bin = gray_thre;

		cvThreshold(imgGray, imgBin, Thre_Bin, 255, CV_THRESH_BINARY);
		

		// ∆ÿ√¢
		cvDilate(imgBin, imgMorph, NULL, 1);
		
		// ƒßΩƒ
		cvErode(imgMorph, imgMorph, NULL, 1);
		
		
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
		

		if( DetectObj == _TRUE  )
		{
			int x_offset;
			int z_offset;

			int   M_Speed  = 0;
			float M_Handle = 0.0;

			float fHandle;
			float fSpeed;


			rx = cX*RESIZE_FACTOR;
			ry = cY*RESIZE_FACTOR;


			x_offset = rx - (IMG_WIDTH/2);
			z_offset = -(DetectCount-400);

			fHandle = (float)x_offset / 30;
			fSpeed  = ((float)abs(z_offset))/5 + (float)(abs(x_offset))/3;
			
			if( z_offset < 0 )
			{
				fSpeed = -fSpeed;
			}

			if( fSpeed >  100 ) fSpeed =  100;
			if( fSpeed < -100 ) fSpeed = -100;

			if( fSpeed > 0 )
			{
				Lib_Motor_PwmLeft  = fSpeed + fSpeed * fHandle;
				Lib_Motor_PwmRight = fSpeed - fSpeed * fHandle; 							
			}
			else
			{
				Lib_Motor_PwmLeft  = fSpeed - fSpeed * fHandle;
				Lib_Motor_PwmRight = fSpeed + fSpeed * fHandle; 			
			}
		}
		else
		{
			Lib_Motor_PwmLeft  = 0;
			Lib_Motor_PwmRight = 0; 
		}


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


		cvCvtColor( frame, imgLcd, CV_BGR2BGR565 ); 
		fb_DrawImage( 0, 0, -90, imgLcd );


		cvCvtColor( imgGray, imgLcd_Resize, CV_GRAY2BGR565 ); 
		fb_DrawImage( imgLcd->width, 0, -90, imgLcd_Resize );

		cvCvtColor( imgBin, imgLcd_Resize, CV_GRAY2BGR565 ); 
		//cvResize(imgLcd_Resize, imgLcd);
		fb_DrawImage( imgLcd->width, imgLabel->height, -90, imgLcd_Resize );


		fb_DrawHistogram( imgLcd->width, imgLabel->height*2, -90, imgGray, Thre_Bin );

	

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
	cvReleaseImage(&imgGray);	
	cvReleaseImage(&imgBin);
	cvReleaseImage(&imgMorph);
	cvReleaseImage(&imgLabel);
	cvReleaseImage(&imgResize);
	cvReleaseImage(&imgLcd);
	cvReleaseImage(&imgLcd_Resize);


	
	fb_close();

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
	int full_id;
	
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
	full_id = 0;
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

					if( maxLabel >= MAX_LABEL_ID )
					{
						maxLabel = MAX_LABEL_ID - 1;
						full_id = 1;
						break;
					}

					pLabel[yOff + ix] = maxLabel;
				}
				else
					pLabel[yOff + ix] = minLabel;
			}
		}

		if( full_id == 1 ) break;
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
		DetectCount = countMainID;

		return _TRUE;
	}
	else
	{
		return _FALSE;
	}
}
