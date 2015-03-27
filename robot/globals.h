/*
 * globals.h
 *
 * Created: 11/22/2012 5:40:59 PM
 *  Author: Chris
 */ 


#ifndef GLOBALS_H_
#define GLOBALS_H_


#define PI 3.1415926535897932384626433832795f
#define K_rad_to_deg (180.0f/3.1415926535897932384626433832795f)
#define K_deg_to_rad (PI/180.0f)
#define NO_CHANGE_IN_POSITION 999999.0f

#include "..\pc\messages.h"
#include "..\pc\commands.h"
#include "flame.h"

typedef struct
{
	//communication w/ PC
	t_inputs inputs;	//this gets sent to the PC
	//volatile t_outputs outputs;    //keep this one around for now
	t_commands commands; 
	
	//motor control:
	sint16 lm_target;
	sint16 rm_target;
	sint16 lm_actual;
	sint16 rm_actual;
	sint16 m_ramp;
	uint8 motor_command_state;
	
	float U;
	s32 encoder_ticks;  //total number of encoder ticks i.e. abs(l+r)/2

	//odometry (since checkpoint; global odometry state is part of inputs struct)
	float dx,dy,dtheta,dU;

	
	//behavior control
	uint8 behavior_state[16];
	
	//ir
	s16 ir[8]; //use 8 so we can simply mirror the analog input channel numbers in this array
	u08 line[2];
	
	//lcd
	u08 lcd_screen;
	
	//sonars
	uint16 us_avg[8];
	uint16 us_cycles;

	//navigation
	u08 current_room;
	u08 dog_position; //0 means we don't know (yet) 1=N side of Rm#4,  2=E side,  3=S side
	u08 door_position; //0 means we don't know; Rm#4 North door = 2
	u08 right_turns;
	u08 left_turns;
	
} t_state;


//extern t_outputs default_outputs;
//extern t_outputs o;
extern t_state s;
extern uint32 main_iterations;

extern t_scan scan_data[360];



#endif /* GLOBALS_H_ */