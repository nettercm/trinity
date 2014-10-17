
#include "standard_includes.h"

// This array of pins is used to initialize the OrangutanPulseIn routines.  To measure
// pulses on multiple pins, add more elements to the array.  For example:
// const unsigned char pulseInPins[] = { IO_D0, IO_C0, IO_C1 };
// BUZZER_IO is IO_D4 on the Orangutan SVP and IO_B2 on the LV, SV, Baby, and 3pi
const unsigned char sound_pulse_pins[] = { IO_B3 };


u16 sound_start_count=0;
u16 consecutive_sound_start_count=0;
u16 button_count=0;
u32 frequency_in_hz=0;



int check_for_start_signal()
{
	static unsigned long curPulse;		// length of current pulse in ticks (0.4 us)
	static unsigned char state;		// current state of input (1 if high, 0 if low)
	static u32 t_last=0;
	static u32 t_now;
	static u08 start_button_state=0;
	static u08 start_button_count=0;
	t_now = get_ms();

#ifdef SVP_ON_WIN32
	return 1;
#endif

	if(t_last==0) t_last=t_now;

	if(start_button_state==0) //button not pressed
	{
		if( !(is_digital_input_high(IO_C1)) ) //is the button pressed?
		{
			start_button_state=1;
			start_button_count=0; //start counting...
		}
	}
	else //button currently in a pressed state
	{
		if( (is_digital_input_high(IO_C1)) ) //button released?
		{
			start_button_state=0;
			start_button_count=0;
		}
		else //button still pressed....
		{
			start_button_count++;
		}
	}


	// get current pulse state for D0
	get_current_pulse_state(SOUND_PULSE_CHANNEL, &curPulse, &state);	// pass arguments as pointers

	// if more than 100 ms have elapsed since the last pin
	// change on this channel, we indicate that pulses have stopped
	if (pulse_to_microseconds(curPulse) >= 50000UL)
	{
		if (is_digital_input_high(IO_B3) ) //  state == HIGH)		// if line is currently high
		{
			/*
			lcd_goto_xy(0, 0);	// go to start of first LCD row
			print("Pin HIGH  ");
			lcd_goto_xy(0, 1);	// go to start of second LCD row
			print("          ");	// clear the row by overwriting with spaces
			*/
			//dbg_printf("start == HIGH\n");
			button_count=0;
			sound_start_count=0;
			consecutive_sound_start_count=0;
		}
		else
		{
			/*
			lcd_goto_xy(0, 0);	// go to start of first LCD row
			print("          ");	// clear the row by overwriting with spaces
			lcd_goto_xy(0, 1);	// go to start of second LCD row
			print("Pin  LOW  ");
			*/
			dbg_printf("start == LOW\n");
			button_count++;
			sound_start_count=0;
			consecutive_sound_start_count=0;
		}
	}
	else if (new_high_pulse(SOUND_PULSE_CHANNEL) && new_low_pulse(SOUND_PULSE_CHANNEL))	// if we have new high and low pulses
	{
		unsigned long high_pulse = get_last_high_pulse(SOUND_PULSE_CHANNEL);
		unsigned long period_in_ticks = high_pulse + get_last_low_pulse(SOUND_PULSE_CHANNEL);

		// compute frequency as 1 / period = 1 / (0.4us * period_in_ticks)
		//  = 2.5 MHz / period_in_ticks
		unsigned long frequency_in_hz = 2500000UL / period_in_ticks;

		// duty cycle = high pulse / (high pulse + low pulse)
		// we multiply by 100 to convert it into a percentage and we add half of the denominator to
		// the numerator to get a properly rounded result
		unsigned long duty_cycle_percent = (100 * high_pulse + period_in_ticks/2) / period_in_ticks;

		if( (frequency_in_hz > 3600) && (frequency_in_hz < 4000) )
		{
			sound_start_count++;
		}
		if(t_now - t_last > 250)
		{
			dbg_printf("sound start count = %d\n",sound_start_count);
			if(sound_start_count <= 1) consecutive_sound_start_count=0;
			if(sound_start_count >  1) consecutive_sound_start_count++;
			sound_start_count=0;
			t_last=t_now;
		}

		/*
		lcd_goto_xy(0, 0);		// go to start of first LCD row
		print_unsigned_long(frequency_in_hz);		// print the measured PWM frequency
		print(" Hz      ");
		lcd_goto_xy(0, 1);		// go to start of second LCD row
		print("DC: ");
		print_unsigned_long(duty_cycle_percent);	// print the measured PWM duty cycle
		print("%  ");
		*/
	}
	if( (start_button_count>=3) || (button_count >= 10) || (consecutive_sound_start_count >= 3) ) 
	{
		dbg_printf("bc = %d, cssc = %d\n",button_count, consecutive_sound_start_count);
		button_count = 0;
		consecutive_sound_start_count = 0;
		sound_start_count = 0;
		return 1; 
	}
	else return 0;
}

