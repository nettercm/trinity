/*
 * IncFile1.h
 *
 * Created: 2/13/2013 9:31:59 PM
 *  Author: Chris
 */ 


#ifndef HARDWARE_H_
#define HARDWARE_H_

/*
Physical I/O:
 
 				2014					2015
PD5				Servo PWM – not used	Servo PWM (for pan/tilt)
PD4				Buzzer					Buzzer (not used)
PD3 / TXD1		Bluetooth TX			Bluetooth TX
PD2 / RXD1		Bluetooth RX			Bluetooth RX
PD1 / TXD0		Sonar 1					Sonar 1 
PD0 / RXD0		Sonar 2					Sonar 2 
PC1 / SDA		Start button			Servo Mux
PC0 / SCL	 							Fire Alarm LED
PB4 / ~SS		Fan enable				Pump enable
PB3 / OC0A		Sound start (from Mic)	Sound start (from Mic)
PA0 / ADC0		Right line sensor		Analog Mux address 0
PA1 / ADC1		Left line sensor		Analog Mux address 1
PA2 / ADC2		Flame sensor			Analog Mux address 2
PA3 / ADC3		Not used				Analog Mux address 3
PA4 / ADC4		IR NE					Analog signal from the Mux
PA5 / ADC5		IR N (Far)				Sonar 3 
PA6 / ADC6		IR N					Sonar 4 
PA7 / ADC7		IR NW					Sonar 5 
PC2				LCD / Bottom-Button	 
PC3				LCD / Middle-Button	 
PC4				LCD	 
PC5				LCD / Top-Button	 
PB0				LCD	 
PB1				LCD	 
PB2				LCD	 



Analog Mux usage: (sample time for 16 ADC: approx 1.5ms, + overhead => 1.7 - 1.8ms)
 
0	Sharp IR N
1	Sharp IR NL
2	Sharp IR NE
3	Sharp IR W
4	Sharp IR NW
5	Sharp IR NR
6	
7	
8	
9	Start Button
10	Line R
11	Line L
12	Flame Omni
13	Flame NW
14	Flame NE
15	Sharp IR E
*/ 

//Start button (may not be needed)

//index into s.ir and s.inputs.ir - does not need to match actual input channel numbers
static const u08 IR_N  = 0;
static const u08 IR_NL = 1;
static const u08 IR_NR = 2;
static const u08 IR_NW = 3;
static const u08 IR_NE = 4;
static const u08 IR_W  = 5;
static const u08 IR_E  = 6;


//analogs.  The following refer to channel numbers
static const u08 AI_IR_N		= 0;
static const u08 AI_IR_NL		= 1;
static const u08 AI_IR_NE		= 2;
static const u08 AI_IR_W		= 3;
static const u08 AI_IR_E		= 15;
static const u08 AI_IR_NW		= 4;
static const u08 AI_IR_NR		= 5;

static const u08 AI_LINE_RIGHT	= 10;
static const u08 AI_LINE_LEFT	= 11;

static const u08 AI_FLAME_N		= 12;
static const u08 AI_FLAME_NW	= 13;
static const u08 AI_FLAME_NE	= 14;

#define AI_START_BUTTON 9


#define RIGHT_LINE 0 //index into s.line[]
#define LEFT_LINE 1 //index into s.line[]

//#define AI_IR_N_long	AI_IR_N
//#define AI_IR_FAR_N		AI_IR_N_long

//PORT D
#define BLUETOOTH_TX_PIN		IO_D3
#define BLUETOOTH_RX_PIN		IO_D2
#define IO_US_ECHO_AND_PING_1	IO_D1
#define IO_US_ECHO_AND_PING_2	IO_D0

//PORD C
#define SERVO_DEMUX_PIN			IO_C1
#define	available_pin_1			IO_C0

//PORT B
#define PUMP_PIN				IO_B4
#define SOUND_START_PIN			IO_B3

//PORT A
#define ANALOG_MUX_ADDR_0_PIN	IO_A0
#define ANALOG_MUX_ADDR_1_PIN	IO_A1
#define ANALOG_MUX_ADDR_2_PIN	IO_A2
#define ANALOG_MUX_ADDR_3_PIN	IO_A3
#define ANALOG_INPUT_PIN		IO_A4
#define	IO_US_ECHO_AND_PING_3	IO_A5
#define	IO_US_ECHO_AND_PING_4	IO_A6
#define	IO_US_ECHO_AND_PING_5	IO_A7



//pulse capture definitions (used for sonars and sound start)
extern const unsigned char pulseInPins[6]; 
#define SOUND_PULSE_CHANNEL 5
#define PULSE_PINS			{ IO_US_ECHO_AND_PING_1 , IO_US_ECHO_AND_PING_2 , IO_US_ECHO_AND_PING_3, IO_US_ECHO_AND_PING_4, IO_US_ECHO_AND_PING_5, SOUND_START_PIN }

//servo demux
extern const unsigned char demuxPins[1];
#define SERVO_DEMUX_PINS	{ SERVO_DEMUX_PIN }


/*

motor speed - large wide wheels

speed 100 => 5 enc tick per 100ms
speed 60  => 2 enc ticks per 100ms


*/


#endif /* INCFILE1_H_ */