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
PD5				Servo PWM – not used	Servo PWM (usage is TBD)
PD4				Buzzer					Buzzer
PD3 / TXD1		Bluetooth TX			Bluetooth TX
PD2 / RXD1		Bluetooth RX			Bluetooth RX
PD1 / TXD0		Sonar 1					Sonar 1 
PD0 / RXD0		Sonar 2					Sonar 2 
PC1 / SDA		Start button			Servo Mux (usage is TBD)
PC0 / SCL	 							UV Tron (???)
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
1	Sharp IR NW
2	Sharp IR NE
3	Sharp IR W
4	Sharp IR E
5	Sharp IR SW
6	Sharp IR SE
7	Sharp IR S
8	Reserved (Sharp IR)
9	Reserved (Sharp IR)
10	Line R
11	Line L
12	Flame
13	Reserved (Flame)
14	Reserved (Flame)
15	Start button (may not be needed)
*/ 


//index into s.ir and s.inputs.ir - does not need to match actual input channel numbers
#define IR_N			0
#define IR_NE			1
#define IR_E			2
#define IR_SE			3
#define IR_S			4
#define IR_SW			5
#define IR_W			6
#define IR_NW			7


//analogs.  The following refer to channel numbers
#define AI_IR_N			0
#define AI_IR_NW		1
#define AI_IR_NE		2
#define AI_IR_W			3
#define AI_IR_E			4
#define AI_IR_SW		5
#define AI_IR_SE		6
#define AI_IR_S			7

#define AI_LINE_RIGHT	10
#define AI_LINE_LEFT	11

#define AI_FLAME_N		12
#define AI_FLAME_NW		13
#define AI_FLAME_NE		14

#define AI_START_BUTTON 15


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