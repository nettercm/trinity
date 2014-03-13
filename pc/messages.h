
#ifndef _messages_h_
#define _messages_h_


#ifdef __cplusplus 
extern "C" {
#endif



#ifdef WIN32
typedef		unsigned char		uint8;
typedef		signed char			sint8;
typedef		unsigned short		uint16;
typedef		signed short		sint16;
typedef		unsigned long		uint32;
typedef		signed long			sint32;
#define __attribute__(a)
#endif


/*
fsm_states
0 master_logic_fsm
1 align_to_wall_on_right
2 monitor_speed
3 align_to_line
4 follow_left_wall_and_turn
5 follow_right_wall_and_turn
6 find_flame_fsm

payload 72

inputs 56+2   14 remain
2 x param 6x2 + 2    0 remain

cmds from PC:
load all/1 from flash
load all/1 from eeprom
save all/1 to eeprom

get all values
get 1 value
set 1 value

*/

#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
typedef struct
{
	uint8 magic1[2]; 			//0xabcd
	uint8 seq;
	uint8 ack;					//seq number of the last packet received from the PC
	uint8  payload[74];
	uint8 magic2[2]; 			//0xdcba
}  __attribute__((__packed__)) t_frame_to_pc; //80 bytes
#ifdef WIN32
#pragma pack(pop)
#endif

#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
typedef struct
{
	uint8 magic1[2]; 			//0xabcd
	uint8 seq;
	uint8 ack;					//seq number of the last packet received from the robot
	uint8 payload[16];			//first 2 bytes of payload reserved for now.
	uint8 magic2[2]; 			//0xdcba
}  __attribute__((__packed__)) t_frame_from_pc;
#ifdef WIN32
#pragma pack(pop)
#endif

#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
typedef struct
{
	uint32 timestamp;
	uint8 analog[8];		//8

	uint16 sonar[4];		//8
	uint16 ir[4];			//8

	sint16 encoders[2];		//4
	sint16 actual_speed[2];	//4
	sint16 target_speed[2]; //4
	sint16 motors[2];		//4
	float x,y,theta;		//12
	//uint8 din;
	//uint8 buttons[3];
	uint16 vbatt;			//2
	uint16 flags;			//2
	//uint8  fsm_states[8];	//8
	sint16 watch[4];		//8
}  __attribute__((__packed__)) t_inputs; //72 bytes
#ifdef WIN32
#pragma pack(pop)
#endif

#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
typedef struct
{
	uint8 magic1[2]; 			//0xabcd
	uint32 timestamp;
	/*
	uint16 servo[1];
	sint16 motors[2];
	uint8  dout;
	uint8  cmd;
	sint16 params[4];
	*/
	uint16 flags;
	uint8 data[20];
	uint8 magic2[2]; 			//0xdcba
}  __attribute__((__packed__)) t_outputs;  //26+32 bytes
#ifdef WIN32
#pragma pack(pop)
#endif

 
/*
Platform:
set tx interval

Servos:
set target angle of servo X

Sonar:
set sonar echo timeout

PID:
set PID loop parameters

Other:
set behavior state, e.g. start / stop / reset

*/

#ifdef __cplusplus 
}
#endif


#endif
