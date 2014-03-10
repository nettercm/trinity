
#include "../robot/typedefs.h"
#include <windows.h>

#define extern


extern void set_digital_output(u08 pin, u08 outputState)
{
}


extern u08 is_digital_input_high(u08 pin)
{
	return 0;
}


extern void set_digital_input(u08 pin, u08 mode)
{
}


extern void delay_ms(u16 delay)
{
	Sleep(delay);
}


extern int read_battery_millivolts_svp(void)
{
	return 9000;
}

extern void lcd_init_printf(void) {}
extern void clear(void) {}
extern void set_motors(s16 left, s16 right) {}
extern void pulse_in_start(const u08 pins[], u08 numpins) {}
extern u32 get_ms(void) { return GetTickCount(); }
extern u16 analog_read(u08 channel) {return 0x0001;}
extern u08 button_is_pressed(u08 button) { return 0; }
extern void lcd_goto_xy(u08 x, u08 y) {}
extern void play_note(u08 note, u08 a , u08 b) {}
extern u16 svp_get_counts_ab(void) {return 1; }
extern u16 svp_get_counts_cd(void) {return 1; }
extern void serial_check(void) {}

