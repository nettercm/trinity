/*
 * debug.h
 *
 * Created: 11/25/2012 9:47:40 PM
 *  Author: Chris
 */ 


#ifndef DEBUG_H_
#define DEBUG_H_

extern int	usb_printf(const char *__fmt, ...);
extern int	dbg_printf(const char *__fmt, ...);
extern char _b[];

#define DBG_BUFFER_SIZE 1024

extern u08 dbg_buffer[DBG_BUFFER_SIZE];
extern s16 dbg_buffer_write;
extern s16 dbg_buffer_read;

#define DBG_printf(flag,p) if(flag){int size = sprintf p; if(usb_power_present()) serial_send_blocking(USB_COMM,_b,size); }

extern unsigned char trace_buffer[];
extern unsigned char *tbptr;
extern unsigned char *msptr;
extern volatile unsigned long msCounter;


//using the buzzer is not compatible w/ servo PWM
#define play_note(a,b,c) NOP()
#define play_from_program_space(a) NOP()


/*

DEFINE_TRACE_MARKERS(1, "some function name",  1, 10); //marker ID's  1-10 are used for "some function name" debugging / tracing
DEFINE_TRACE_MARKERS(2, "some other func   ", 20, 30); //marker ID's 20-30 are used for "some other func   " debugging / tracing

idea:  define at compile time how to printf the marker parameter(s)
*/

/*
T0 - single "marker" byte - no timestamp or parameter
T1 - marker byte + 8-bit parameter
T2 - marker byte + 16-bit time stamp
T3 - marker byte + time stamp + 8-bit parameter

type 0:  marker only
type 1:  marker + 1 byte parameter
type 2:  marker + 2 byte timestamp
type 3:  marker + timestamp + parameter

T1(function_id,marker_id)

trace packet type:	2 bits
function id:		3 bits
marker id:			3 bits

or: just use 6 bits for marker id

*/

//marker only (1 byte total)
//11 cycles = 0.5us
#define T0(condition,marker) 								\
if( (condition) && (marker) )								\
{															\
	*(tbptr++) = ((marker) | 0x00);							\
} 
	
	
//marker + parameter (2 bytes total)
//15 cycles, i.e. < 1us
#define T1(condition, marker, parameter) 					\
if( (condition) && (marker) )								\
{															\
	register unsigned char *ptr = tbptr;					\
	*(ptr++) = ((((unsigned char)(marker))&0x3f) | 0x40);	\
	*(ptr++) = (unsigned char)(parameter);					\
	tbptr=ptr;												\
}
		
//marker + timestamp (3 bytes total)
#define T2(condition, marker)								\
if( (condition) && (marker) )								\
{															\
	register unsigned char *ptr = tbptr;					\
	*(ptr++) = ((((unsigned char)(marker))&0x3f) | 0x80);	\
	*(ptr++) = TCNT2;										\
	*(ptr++) = *msptr;										\
	tbptr=ptr;												\
}
	
//28 cycles = 1.4us
//marker + timestamp + parameter  (4 bytes total)
#define T3(condition, marker, parameter) 					\
if( (condition) && (marker) )								\
{															\
	register unsigned char *ptr = tbptr;					\
	*(ptr++) = ((((unsigned char)(marker))&0x3f) | 0xc0);	\
	*(ptr++) = TCNT2;										\
	*(ptr++) = *msptr;										\
	*(ptr++) = (unsigned char)(parameter);					\
	tbptr=ptr;												\
}

/*
//27 cycles for const marker, 32 otherwise
//even if marker is non-const, inline call gets optimized away if marker is known to be 0 at compile time
inline void T3_(unsigned char marker, unsigned char parameter)
{
	if(marker)											
	{													
		register unsigned char *ptr = bptr;
		*(ptr++) = (((unsigned char)(marker)) | 0x80);	
		*(ptr++) = TCNT2;								
		*(ptr++) = *msptr;								
		*(ptr++) = (unsigned char)(parameter);			
		bptr=ptr;										
	}
}

//34 cycles
#define T3(marker, parameter) 				\
	if(marker)								\
	{										\
		register unsigned char *ptr = bptr;	\
		*(ptr+0) = (((unsigned char)(marker)) | 0x80);			\
		*(ptr+1) = TCNT2;					\
		*(ptr+2) = *msptr;					\
		*(ptr+3) = (unsigned char)(parameter);				\
		bptr+=4;							\
	}
*/


/*
#define T3(marker, parameter) 		\
if(marker)							\
{									\
	b[idx+0] = (marker | 0x80);		\
	b[idx+1] = TCNT2;				\
	b[idx+2] = *msptr;				\
	b[idx+3] = parameter;			\
	idx+=4;							\
}
*/

//set SIM to 1 for running under the Atmel Studio Simulator (SIM has nothing to do w/ running under WIN32)
#define SIM 0
//////////////////////////////////////////////////////////////////////////////
#if SIM

extern int	AVRSIM_printf(const char *__fmt, ...);
extern uint8 AVRSIM_serial_get_received_bytes(void);

#define serial_receive_ring(a,b,c)
#define serial_get_received_bytes(a) AVRSIM_serial_get_received_bytes()

#define set_analog_mode(a)
#define svp_set_mode(a)

#define lcd_init_printf()
#define lcd_goto_xy(x,y)
#define clear()

#define set_motors(a,b)

#define printf AVRSIM_printf

#endif
//////////////////////////////////////////////////////////////////////////////



#endif /* DEBUG_H_ */