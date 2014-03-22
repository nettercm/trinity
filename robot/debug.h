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

#define DBG_BUFFER_SIZE 2048

extern u08 dbg_buffer[DBG_BUFFER_SIZE];
extern s16 dbg_buffer_write;
extern s16 dbg_buffer_read;

#define DBG_printf(flag,p) if(flag){int size = sprintf p; if(usb_power_present()) serial_send_blocking(USB_COMM,_b,size); }

#define SIM 0
//////////////////////////////////////////////////////////////////////////////
#if SIM

extern int	SIM_printf(const char *__fmt, ...);
extern uint8 SIM_serial_get_received_bytes(void);

#define serial_receive_ring(a,b,c)
#define serial_get_received_bytes(a) SIM_serial_get_received_bytes()

#define set_analog_mode(a)
#define svp_set_mode(a)

#define lcd_init_printf()
#define lcd_goto_xy(x,y)
#define clear()

#define set_motors(a,b)

#define printf SIM_printf

#endif
//////////////////////////////////////////////////////////////////////////////



#endif /* DEBUG_H_ */