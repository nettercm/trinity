
#include "typedefs.h"

#define IO_D0 0
#define IO_D1 1

#define IO_B0 0
#define IO_B1 1
#define IO_B2 2
#define IO_B3 3
#define IO_B4 4

#define IO_C0 0
#define IO_C1 1

#define PULL_UP_ENABLED 1
#define HIGH_IMPEDANCE 2
#define LOW 0
#define HIGH 1

#define MODE_10_BIT 0
#define SVP_MODE_ENCODERS 1

#define TOP_BUTTON 0
#define MIDDLE_BUTTON 1
#define BOTTOM_BUTTON 2

#define USB_COMM 0
#define UART1 0

#define LOW_PULSE 0
#define HIGH_PULSE 1


#define set_analog_mode(mode)
#define svp_set_mode(mode)
#define svp_demo()
#define servo_test()

extern void set_digital_output(u08 pin, u08 outputState);
extern u08 is_digital_input_high(u08 pin);
extern void set_digital_output(u08 pin, u08 value);
extern void set_digital_input(u08 pin, u08 mode);

extern void delay_ms(u16 delay);

extern int read_battery_millivolts_svp(void);
extern void lcd_init_printf(void);
extern void clear(void);
extern void set_motors(s16 left, s16 right);
extern void pulse_in_start(const u08 pins[], u08 numpins);
extern u32 get_ms(void);
extern u16 analog_read(u08 channel);
extern u08 button_is_pressed(u08 button);
extern void lcd_goto_xy(u08 x, u08 y);
extern void play_note(u08 note, u08 a , u08 b);
extern u16 svp_get_counts_ab(void);
extern u16 svp_get_counts_cd(void);
extern void serial_check(void);



#define C(x)			( 0 + (x)*12)
#define C_SHARP(x)		( 1 + (x)*12)
#define D_FLAT(x)		( 1 + (x)*12)
#define D(x)			( 2 + (x)*12)
#define D_SHARP(x)		( 3 + (x)*12)
#define E_FLAT(x)		( 3 + (x)*12)
#define E(x)			( 4 + (x)*12)
#define F(x)			( 5 + (x)*12)
#define F_SHARP(x)		( 6 + (x)*12)
#define G_FLAT(x)		( 6 + (x)*12)
#define G(x)			( 7 + (x)*12)
#define G_SHARP(x)		( 8 + (x)*12)
#define A_FLAT(x)		( 8 + (x)*12)
#define A(x)			( 9 + (x)*12)
#define A_SHARP(x)		(10 + (x)*12)
#define B_FLAT(x)		(10 + (x)*12)
#define B(x)			(11 + (x)*12)

struct PulseInputStruct
{
	volatile unsigned char* pinRegister;
	unsigned char bitmask;
	volatile unsigned long lastPCTime;
	volatile unsigned char inputState;
	volatile unsigned long lastHighPulse;
	volatile unsigned long lastLowPulse;
	volatile unsigned char newPulse;
};