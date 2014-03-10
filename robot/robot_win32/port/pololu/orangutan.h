
#define _ORANGUTAN_SVP 1

#define cli()
#define sei()


#define inline

extern volatile unsigned long elapsed_milliseconds;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OrangutanDigital.h
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef OrangutanDigital_h
#define OrangutanDigital_h

#define INPUT 				0
#define OUTPUT				1
#define LOW					0
#define HIGH				1
#define TOGGLE				0xFF
#define HIGH_IMPEDANCE		0
#define PULL_UP_ENABLED		1

// port D pins
#define IO_D0				0
#define IO_D1				1
#define IO_D2				2
#define IO_D3				3
#define IO_D4				4
#define IO_D5				5
#define IO_D6				6
#define IO_D7				7

// port B pins
#define IO_B0				8
#define IO_B1				9
#define IO_B2				10
#define IO_B3				11
#define IO_B4				12
#define IO_B5				13


#if defined(_ORANGUTAN_SVP) || defined(_ORANGUTAN_X2)

#define IO_B6				14
#define IO_B7				15

// port C pins
#define IO_C0				16
#define IO_C1				17
#define IO_C2				18
#define IO_C3				19
#define IO_C4				20
#define IO_C5				21
#define IO_C6				22
#define IO_C7				23

// port A pins
#define IO_A0				31
#define IO_A1				30
#define IO_A2				29
#define IO_A3				28
#define IO_A4				27
#define IO_A5				26
#define IO_A6				25
#define IO_A7				24

#else

// port C pins
#define IO_C0				14
#define IO_C1				15
#define IO_C2				16
#define IO_C3				17
#define IO_C4				18
#define IO_C5				19
#define IO_C6				20	// only used if RESET pin is changed to be a digital I/O

#endif


void set_digital_output(unsigned char pin, unsigned char outputState);
void set_digital_input(unsigned char pin, unsigned char inputState);

#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OrangutanTime.h
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef OrangutanTime_h
#define OrangutanTime_h

unsigned long get_ticks(void);
unsigned long ticks_to_microseconds(unsigned long ticks);
unsigned long get_ms(void);
void delay_ms(unsigned int milliseconds);
void time_reset(void);

// This is inline for efficiency:
void delay_us(unsigned int microseconds);

// These are alternative aliases:
static void delay(unsigned int milliseconds) { delay_ms(milliseconds); }
static unsigned long millis(void) { return get_ms(); }
static void delayMicroseconds(unsigned int microseconds) { delay_us(microseconds); }

#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OrangutanSVP.h
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef OrangutanSVP_h
#define OrangutanSVP_h

#define SVP_MODE_RX           0
#define SVP_MODE_ANALOG       2
#define SVP_MODE_ENCODERS     4

#define SVP_SLAVE_SELECT_ON   1

typedef	union SVPStatus
{
	unsigned char status;
	struct
	{
	    unsigned usbPowerPresent :1;
        unsigned usbConfigured :1;
		unsigned usbSuspend :1;
		unsigned dtrEnabled :1;
	    unsigned rtsEnabled :1;
		unsigned toggle :1;
	};
} SVPStatus;

void svp_set_mode(unsigned char mode);

unsigned char svp_get_firmware_version(void);

SVPStatus svp_get_status(void);
static inline unsigned char usb_power_present(void) { return svp_get_status().usbPowerPresent; }
static inline unsigned char usb_configured(void) { return svp_get_status().usbConfigured; }
static inline unsigned char usb_suspend(void) { return svp_get_status().usbSuspend; }
static inline unsigned char dtr_enabled(void) { return svp_get_status().dtrEnabled; }
static inline unsigned char rts_enabled(void) { return svp_get_status().rtsEnabled; }

// Encoder Functions
int svp_get_counts_ab(void);
int svp_get_counts_and_reset_ab(void);
int svp_get_counts_cd(void);
int svp_get_counts_and_reset_cd(void);
unsigned char svp_check_error_ab(void);
unsigned char svp_check_error_cd(void);


#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OrangutanPulseIn.h
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef OrangutanPulseIn_h
#define OrangutanPulseIn_h

// possible values for the newPulse member of PulseInputStruct
#define LOW_PULSE			1		// the pulse just completed was a low pulse (pin just went high)
#define HIGH_PULSE			2		// the pulse just completed was a high pulse (pin just went low)
#define ANY_PULSE			3		// newPulse member is not zero


// Structure for storing the port register and approrpiate bitmask for an I/O pin.
// This lets us easily change the output value of the pin represented by the struct.
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


// use of pulse_in_init() is discouraged; use pulse_in_start() instead
unsigned char pulse_in_start(const unsigned char *pulsePins, unsigned char numPins);
void get_pulse_info(unsigned char idx, struct PulseInputStruct* pulse_info);
unsigned char new_pulse(unsigned char idx);
unsigned char new_high_pulse(unsigned char idx);
unsigned char new_low_pulse(unsigned char idx);
unsigned long get_last_high_pulse(unsigned char idx);
unsigned long get_last_low_pulse(unsigned char idx);
void get_current_pulse_state(unsigned char idx, unsigned long* pulse_width, unsigned char* state);
unsigned long pulse_to_microseconds(unsigned long pulse);
void pulse_in_stop(void);

#endif




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OrangutanPulseIn.h
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef OrangutanSerial_h
#define OrangutanSerial_h



 // The Orangutan SVP has two UARTs and one virtual COM port via USB.
 #define _SERIAL_PORTS 				3
 #define UART0    					0
 #define UART1    					1
 #define USB_COMM 					2
 #define _PORT_IS_UART 				(port!=2)
 #define BYTES_RECEIVED				OrangutanSVPRXFIFO::getReceivedBytes()
 #define NEXT_BYTE					OrangutanSVPRXFIFO::getNextByte()
 #define SEND_BYTE_IF_READY(byte)	OrangutanSVP::serialSendIfReady(byte)

// C Function declarations.
void serial_check(void);
void serial_set_baud_rate(unsigned char port, unsigned long baud);
void serial_set_mode(unsigned char port, unsigned char mode);
unsigned char serial_get_mode(unsigned char port);
void serial_receive(unsigned char port, char *buffer, unsigned char size);
void serial_cancel_receive(unsigned char port);
char serial_receive_blocking(unsigned char port, char *buffer, unsigned char size, unsigned int timeout);
void serial_receive_ring(unsigned char port, char *buffer, unsigned char size);
unsigned char serial_get_received_bytes(unsigned char port);
char serial_receive_buffer_full(unsigned char port);
void serial_send(unsigned char port, char *buffer, unsigned char size);
void serial_send_blocking(unsigned char port, char *buffer, unsigned char size);
unsigned char serial_get_sent_bytes(unsigned char port);
char serial_send_buffer_empty(unsigned char port);

#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OrangutanBuzzer.h
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef OrangutanBuzzer_h
#define OrangutanBuzzer_h

#define NOTE_C(x)			( 0 + (x)*12)
#define NOTE_C_SHARP(x)		( 1 + (x)*12)
#define NOTE_D_FLAT(x)		( 1 + (x)*12)
#define NOTE_D(x)			( 2 + (x)*12)
#define NOTE_D_SHARP(x)		( 3 + (x)*12)
#define NOTE_E_FLAT(x)		( 3 + (x)*12)
#define NOTE_E(x)			( 4 + (x)*12)
#define NOTE_F(x)			( 5 + (x)*12)
#define NOTE_F_SHARP(x)		( 6 + (x)*12)
#define NOTE_G_FLAT(x)		( 6 + (x)*12)
#define NOTE_G(x)			( 7 + (x)*12)
#define NOTE_G_SHARP(x)		( 8 + (x)*12)
#define NOTE_A_FLAT(x)		( 8 + (x)*12)
#define NOTE_A(x)			( 9 + (x)*12)
#define NOTE_A_SHARP(x)		(10 + (x)*12)
#define NOTE_B_FLAT(x)		(10 + (x)*12)
#define NOTE_B(x)			(11 + (x)*12)

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

// special notes
#define A4				A(4)			// center of the Equal-Tempered Scale

#define SILENT_NOTE		0xFF			// this note will silence the buzzer

#define DIV_BY_10		(1 << 15)		// frequency bit that indicates Hz/10

#if defined(_ORANGUTAN_SVP) || defined(_ORANGUTAN_X2)
#define BUZZER_DDR		DDRD
#define BUZZER			(1 << PORTD4)
#define BUZZER_IO		IO_D4
#else
#define BUZZER_DDR		DDRB
#define BUZZER			(1 << PORTB2)
#define BUZZER_IO		IO_B2
#endif

void buzzer_init(void);
void play_frequency(unsigned int freq, unsigned int duration, unsigned char volume);
void play_note(unsigned char note, unsigned int duration, unsigned char volume);
void play(const char *sequence);
void play_from_program_space(const char *sequence);
unsigned char is_playing(void);
void stop_playing(void);
unsigned char play_check(void);
void play_mode(unsigned char mode);

#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OrangutanLCD.h
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef OrangutanLCD_h
#define OrangutanLCD_h


#define LCD_LEFT			0
#define LCD_RIGHT			1
#define CURSOR_SOLID		0
#define CURSOR_BLINKING		1

#define LCD_CLEAR		0x01
#define LCD_SHOW_BLINK	0x0F
#define LCD_SHOW_SOLID	0x0E		
#define LCD_HIDE		0x0C
#define LCD_CURSOR_L	0x10
#define LCD_CURSOR_R	0x14
#define LCD_SHIFT_L		0x18
#define LCD_SHIFT_R		0x1C

void lcd_init_printf(void);
void lcd_init_printf_with_dimensions(unsigned char width, unsigned char height);
void clear(void);
void print(const char *str);
void print_from_program_space(const char *str);
void print_character(char c);
void print_long(long value);
void print_unsigned_long(unsigned long value);
void print_binary(unsigned char value);
void print_hex(unsigned int value);
void print_hex_byte(unsigned char value);
void lcd_goto_xy(unsigned char col, unsigned char row);
void lcd_show_cursor(unsigned char cursorType);
void lcd_hide_cursor(void);
void lcd_move_cursor(unsigned char direction, unsigned char num);
void lcd_scroll(unsigned char direction, unsigned char num, unsigned int delay_time);
void lcd_load_custom_character(const char *picture, unsigned char number);

#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OrangutanMotors.h
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void motors_init(void);
void set_m1_speed(int speed);
void set_m2_speed(int speed);
void set_motors(int m1, int m2);



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OrangutanAnalog.h
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef OrangutanAnalog_h
#define OrangutanAnalog_h

#define MODE_8_BIT		1
#define MODE_10_BIT		0

// ADC Channels

#ifdef _ORANGUTAN_SVP

#define TRIMPOT   128
#define CHANNEL_A 129
#define CHANNEL_B 130
#define CHANNEL_C 131
#define CHANNEL_D 132

#else

#define TRIMPOT			7
#define TEMP_SENSOR		6

#endif


void set_analog_mode(unsigned char mode);
unsigned char get_analog_mode(void);
unsigned int analog_read(unsigned char channel);
unsigned int analog_read_millivolts(unsigned char channel);
unsigned int analog_read_average(unsigned char channel, unsigned int samples);
unsigned int analog_read_average_millivolts(unsigned char channel, unsigned int samples);
void start_analog_conversion(unsigned char channel);
unsigned char analog_is_converting(void);
unsigned int analog_conversion_result(void);
unsigned int analog_conversion_result_millivolts(void);
void set_millivolt_calibration(unsigned int calibration);
unsigned int read_vcc_millivolts(void);
unsigned int to_millivolts(unsigned int analog_result);
unsigned int read_trimpot(void);
unsigned int read_trimpot_millivolts(void);

unsigned int read_battery_millivolts_svp(void);
unsigned int read_battery_millivolts(void);

#endif




#ifndef OrangutanPushbuttons_h
#define OrangutanPushbuttons_h

// Orangutan SVP definitions (pushing button makes input low)
#define TOP_BUTTON		(1)
#define MIDDLE_BUTTON	(2)
#define BOTTOM_BUTTON	(3)

#define BUTTON_DDR		DDRC
#define BUTTON_PORT		PORTC
#define BUTTON_PIN		PINC

#define BUTTONS_DOWN		(~BUTTON_PIN)
#define BUTTONS_UP		BUTTON_PIN

void buttons_init(void);
unsigned char wait_for_button_press(unsigned char buttons);
unsigned char wait_for_button_release(unsigned char buttons);
unsigned char wait_for_button(unsigned char buttons);
unsigned char button_is_pressed(unsigned char buttons);
unsigned char get_single_debounced_button_press(unsigned char buttons);
unsigned char get_single_debounced_button_release(unsigned char buttons);

#endif
