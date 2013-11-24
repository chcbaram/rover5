



#ifndef   _DEFINE_H_
#define   _DEFINE_H_

typedef    signed  char  S8;
typedef  unsigned  char  U8;
typedef    signed  short S16;
typedef  unsigned  short U16;
typedef    signed  int   S32;
typedef  unsigned  int   U32;



                                                                                                 
typedef union
{
        unsigned char  BitU8[2];
          signed char  BitS8[2];
        unsigned short BitU16;
          signed short BitS16;
} WORD_OBJ;



#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE 	0
#endif


#endif
