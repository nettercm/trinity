/*
 * lookup.h
 *
 * Created: 11/20/2012 11:20:57 PM
 *  Author: Chris
 */ 


#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_


//#ifdef WIN32
typedef		unsigned char		uint8;
typedef		signed char			sint8;
typedef		unsigned short		uint16;
typedef		signed short		sint16;
typedef		unsigned long		uint32;
typedef		signed long			sint32;
//#endif


typedef unsigned long u32;
typedef signed long s32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u08;
typedef signed char s08;
typedef float f32;
typedef float flt;


#define LIMIT(var,min,max)					if((var)>(max)) (var)=(max);  if((var)<(min)) (var)=(min)

#define LIMIT2(var,min,max,flag)	\
if( (var)>(max) ) \
{ \
	(var)=(max); \
	flag=2; \
} \
else if( (var)<(min) ) \
{ \
	(var)=(min); \
	flag=1; \
} \
else flag=0;


#endif //TYPEDEFS_H_
