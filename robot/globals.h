/*
 * globals.h
 *
 * Created: 11/22/2012 5:40:59 PM
 *  Author: Chris
 */ 


#ifndef GLOBALS_H_
#define GLOBALS_H_



#include "..\pc\messages.h"
#include "..\pc\commands.h"

typedef struct
{
	//communication w/ PC
	volatile t_inputs inputs;	//this gets sent to the PC
	//volatile t_outputs outputs;    //keep this one around for now
	volatile t_commands commands; 
	
	//motor control:
	volatile sint16 lm_target;
	volatile sint16 rm_target;
	volatile sint16 lm_actual;
	volatile sint16 rm_actual;
	volatile sint16 m_ramp;
	volatile uint8 motor_command_state;
	
	//odometry:
	//float x,y,theta;
	
	//behavior control
	volatile uint8 behavior_state[16];
	
	//ir
	volatile s16 ir[8]; //use 8 so we can simply mirror the analog input channel numbers in this array
	volatile u08 line[2];
	
	//lcd
	volatile u08 lcd_screen;
	
	//sonars
	volatile uint16 us_avg[8];
	volatile uint16 us_cycles;
	
} t_state;


//extern t_outputs default_outputs;
//extern t_outputs o;
extern volatile t_state s;
extern uint32 main_iterations;



#endif /* GLOBALS_H_ */