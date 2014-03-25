/*
 * IncFile1.h
 *
 * Created: 2/13/2013 9:31:59 PM
 *  Author: Chris
 */ 


#ifndef HARDWARE_H_
#define HARDWARE_H_

//analogs
#define AI_LINE_RIGHT 0 //analog 0
#define AI_LINE_LEFT  1

#define AI_IR_NW      7
#define AI_IR_N       6
#define AI_IR_N_long  5
#define AI_IR_FAR_N	  AI_IR_N_long
#define AI_IR_NE      4


#define AI_FLAME_N    2


#define IO_US_ECHO_AND_PING_1 IO_D0
#define IO_US_ECHO_AND_PING_2 IO_D1


//sonar ping and echo
#define IO_US_ECHO IO_D0 //IO_B3
#define IO_US_PING IO_D1 //IO_B4

//bit/addr 0,1,2 of the sonar mux
#define IO_US_MUX_0 IO_B3 //IO_A7
#define IO_US_MUX_1 IO_B4 //IO_A6
#define IO_US_MUX_2 IO_C0 //IO_A5


#define US_1_PULSE_CHANNEL 0
#define US_2_PULSE_CHANNEL 1
#define UV_PULSE_CHANNEL 2

extern const unsigned char pulseInPins[]; 

//uvtron
#define IO_UV_PULSE IO_C1 //IO_D1



/*

motor speed - large wide wheels

speed 100 => 5 enc tick per 100ms
speed 60  => 2 enc ticks per 100ms


*/


#endif /* INCFILE1_H_ */