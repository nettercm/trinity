#include <Windows.h>
#include <stdio.h>
#include "orangutan.h"


void BP(void)
{
}

#if 0
#define NOT_IMPLEMENTED() \
	(1==1 ? printf("\n\n%s is not yet implemented!\n\n", __FUNCTION__ ) , BP() ,  0 : 0)
#else
#define NOT_IMPLEMENTED() 0
#endif



void set_digital_output(unsigned char pin, unsigned char outputState) { NOT_IMPLEMENTED(); }
void set_digital_input(unsigned char pin, unsigned char inputState) { NOT_IMPLEMENTED(); }



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OrangutanSVP.h
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SVPStatus svp_status;
SVPStatus svp_get_status(void) { return svp_status; }
void svp_set_mode(unsigned char mode) { NOT_IMPLEMENTED(); }
unsigned char svp_get_firmware_version(void) { NOT_IMPLEMENTED(); }



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OrangutanTime.h
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

volatile unsigned long elapsed_milliseconds=0;

unsigned long get_ticks(void){ return elapsed_milliseconds * 25000; }
unsigned long ticks_to_microseconds(unsigned long ticks) {	NOT_IMPLEMENTED(); return 0; }
void time_reset(void) { elapsed_milliseconds=0; }
void delay_us(unsigned int microseconds) { NOT_IMPLEMENTED(); }

unsigned long get_ms(void) 
{
	return elapsed_milliseconds;
	//return GetTickCount();
}

void delay_ms(unsigned int milliseconds)
{
	Sleep(milliseconds);
	elapsed_milliseconds += milliseconds;
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OrangutanPulseIn.h
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// use of pulse_in_init() is discouraged; use pulse_in_start() instead
unsigned char pulse_in_start(const unsigned char *pulsePins, unsigned char numPins) { NOT_IMPLEMENTED(); }
void get_pulse_info(unsigned char idx, struct PulseInputStruct* pulse_info) { NOT_IMPLEMENTED(); }
unsigned char new_pulse(unsigned char idx) { NOT_IMPLEMENTED(); }
unsigned char new_high_pulse(unsigned char idx) { NOT_IMPLEMENTED(); }
unsigned char new_low_pulse(unsigned char idx) { NOT_IMPLEMENTED(); }
unsigned long get_last_high_pulse(unsigned char idx) { NOT_IMPLEMENTED(); }
unsigned long get_last_low_pulse(unsigned char idx) { NOT_IMPLEMENTED(); }
void get_current_pulse_state(unsigned char idx, unsigned long* pulse_width, unsigned char* state) { NOT_IMPLEMENTED(); }
unsigned long pulse_to_microseconds(unsigned long pulse) { NOT_IMPLEMENTED(); }
void pulse_in_stop(void) { NOT_IMPLEMENTED(); }



// Encoder Functions
int svp_get_counts_ab(void) { NOT_IMPLEMENTED(); }
int svp_get_counts_and_reset_ab(void) { NOT_IMPLEMENTED(); }
int svp_get_counts_cd(void) { NOT_IMPLEMENTED(); }
int svp_get_counts_and_reset_cd(void) { NOT_IMPLEMENTED(); }
unsigned char svp_check_error_ab(void) { NOT_IMPLEMENTED(); }
unsigned char svp_check_error_cd(void) { NOT_IMPLEMENTED(); }



void serial_check(void) { NOT_IMPLEMENTED(); }
void serial_set_baud_rate(unsigned char port, unsigned long baud) { NOT_IMPLEMENTED(); }
void serial_set_mode(unsigned char port, unsigned char mode) { NOT_IMPLEMENTED(); }
unsigned char serial_get_mode(unsigned char port) { NOT_IMPLEMENTED(); }
void serial_receive(unsigned char port, char *buffer, unsigned char size) { NOT_IMPLEMENTED(); }
void serial_cancel_receive(unsigned char port) { NOT_IMPLEMENTED(); }
char serial_receive_blocking(unsigned char port, char *buffer, unsigned char size, unsigned int timeout) { NOT_IMPLEMENTED(); }
void serial_receive_ring(unsigned char port, char *buffer, unsigned char size) { NOT_IMPLEMENTED(); }
unsigned char serial_get_received_bytes(unsigned char port) { NOT_IMPLEMENTED();  return 0; }
char serial_receive_buffer_full(unsigned char port) { NOT_IMPLEMENTED(); return 0; }
void serial_send(unsigned char port, char *buffer, unsigned char size) { NOT_IMPLEMENTED(); }
void serial_send_blocking(unsigned char port, char *buffer, unsigned char size) { NOT_IMPLEMENTED(); }
unsigned char serial_get_sent_bytes(unsigned char port) { NOT_IMPLEMENTED(); }
char serial_send_buffer_empty(unsigned char port) { NOT_IMPLEMENTED(); return 1; }


void buzzer_init(void) { NOT_IMPLEMENTED(); }
void play_frequency(unsigned int freq, unsigned int duration, unsigned char volume) { NOT_IMPLEMENTED(); }
void play_note(unsigned char note, unsigned int duration, unsigned char volume) { NOT_IMPLEMENTED(); }
void play(const char *sequence) { NOT_IMPLEMENTED(); }
void play_from_program_space(const char *sequence) { NOT_IMPLEMENTED(); }
unsigned char is_playing(void) { NOT_IMPLEMENTED(); }
void stop_playing(void) { NOT_IMPLEMENTED(); }
unsigned char play_check(void) { NOT_IMPLEMENTED(); }
void play_mode(unsigned char mode) { NOT_IMPLEMENTED(); }


void lcd_init_printf(void) { NOT_IMPLEMENTED(); }
void lcd_init_printf_with_dimensions(unsigned char width, unsigned char height) { NOT_IMPLEMENTED(); }
void clear(void) { printf("\n"); }
void print(const char *str) { NOT_IMPLEMENTED(); }
void print_from_program_space(const char *str) { NOT_IMPLEMENTED(); }
void print_character(char c) { NOT_IMPLEMENTED(); }
void print_long(long value) { NOT_IMPLEMENTED(); }
void print_unsigned_long(unsigned long value) { NOT_IMPLEMENTED(); }
void print_binary(unsigned char value) { NOT_IMPLEMENTED(); }
void print_hex(unsigned int value) { NOT_IMPLEMENTED(); }
void print_hex_byte(unsigned char value) { NOT_IMPLEMENTED(); }
void lcd_goto_xy(unsigned char col, unsigned char row) { NOT_IMPLEMENTED(); }
void lcd_show_cursor(unsigned char cursorType) { NOT_IMPLEMENTED(); }
void lcd_hide_cursor(void) { NOT_IMPLEMENTED(); }
void lcd_move_cursor(unsigned char direction, unsigned char num) { NOT_IMPLEMENTED(); }
void lcd_scroll(unsigned char direction, unsigned char num, unsigned int delay_time) { NOT_IMPLEMENTED(); }
void lcd_load_custom_character(const char *picture, unsigned char number) { NOT_IMPLEMENTED(); }



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OrangutanMotors.h
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void motors_init(void) { NOT_IMPLEMENTED(); }
void set_m1_speed(int speed) { NOT_IMPLEMENTED(); }
void set_m2_speed(int speed) { NOT_IMPLEMENTED(); }
void set_motors(int m1, int m2) { NOT_IMPLEMENTED(); }


void set_analog_mode(unsigned char mode) { NOT_IMPLEMENTED(); }
unsigned char get_analog_mode(void) { NOT_IMPLEMENTED(); }
unsigned int analog_read(unsigned char channel) { return 100; }
unsigned int analog_read_millivolts(unsigned char channel) { NOT_IMPLEMENTED(); }
unsigned int analog_read_average(unsigned char channel, unsigned int samples) { NOT_IMPLEMENTED(); }
unsigned int analog_read_average_millivolts(unsigned char channel, unsigned int samples) { NOT_IMPLEMENTED(); }
void start_analog_conversion(unsigned char channel) { }
unsigned char analog_is_converting(void) { return 0; }
unsigned int analog_conversion_result(void) { return 100; }
unsigned int analog_conversion_result_millivolts(void) { NOT_IMPLEMENTED(); }
void set_millivolt_calibration(unsigned int calibration) { NOT_IMPLEMENTED(); }
unsigned int read_vcc_millivolts(void) { return 5000; }
unsigned int to_millivolts(unsigned int analog_result) { NOT_IMPLEMENTED(); }
unsigned int read_trimpot(void) { NOT_IMPLEMENTED(); }
unsigned int read_trimpot_millivolts(void) { NOT_IMPLEMENTED(); }
unsigned int read_battery_millivolts_svp(void) { return 10000; }
unsigned int read_battery_millivolts(void) { return 10000; }


void buttons_init(void) { NOT_IMPLEMENTED(); }
unsigned char wait_for_button_press(unsigned char buttons) { NOT_IMPLEMENTED(); }
unsigned char wait_for_button_release(unsigned char buttons) { NOT_IMPLEMENTED(); }
unsigned char wait_for_button(unsigned char buttons) { NOT_IMPLEMENTED(); }
unsigned char button_is_pressed(unsigned char buttons) { NOT_IMPLEMENTED(); return 0; }
unsigned char get_single_debounced_button_press(unsigned char buttons) { NOT_IMPLEMENTED(); }
unsigned char get_single_debounced_button_release(unsigned char buttons) { NOT_IMPLEMENTED(); }
