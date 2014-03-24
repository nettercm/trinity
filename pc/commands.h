
#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "messages.h"


#ifdef __cplusplus 
extern "C" {
#endif


#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
typedef struct
{
	uint8 i;					//index of the next available cmd byte
	uint8 d[10];				//d for data;
}  __attribute__((__packed__)) t_commands;  // 50 bytes
#ifdef WIN32
#pragma pack(pop)
#endif

	
#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
typedef struct 
{
	sint16 lm;
	sint16 rm;
} __attribute__((__packed__)) t_set_motors_cmd;
#ifdef WIN32
#pragma pack(pop)
#endif


#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
typedef struct 
{
	uint8 cmd;
	uint16 p1;
	uint16 p2;
	sint16 lm;
	sint16 rm;
} __attribute__((__packed__)) t_motor_command;
#ifdef WIN32
#pragma pack(pop)
#endif


extern t_commands commands;
extern void CMD_set_motors(sint16 lm, sint16 rm);

#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
typedef struct
{
	uint8 current_value;
	uint8 filtered_value;
	sint8 slope;
} __attribute__((__packed__)) t_analog;
#ifdef WIN32
#pragma pack(pop)
#endif



//use this header file on PC and robot

#define CMD_SET_MOTORS			0x01
#define CMD_MOTOR_COMMAND		0x02
#define CMD_SET_SONAR_SEQUENCE	0x03
#define CMD_SET_SONAR_TIMEOUT	0x04
#define CMD_SET_BEHAVIOR_STATE	0x05
#define CMD_RESET_ENCODERS		0x06
#define CMD_SET_CONFIG_VALUE	0x07
#define CMD_GET_CONFIG_VALUE	0x08
#define CMD_GET_CONFIG			0x09


extern t_commands commands;
//extern t_frame_from_pc tx_buffer;

extern void CMD_set_motors(sint16 lm, sint16 rm);
extern void CMD_motor_command(uint8 cmd, uint16 p1, uint16 p2, sint16 lm, sint16 rm);
extern void CMD_set_behavior_state(uint8 behavior_id, uint8 behavior_state);
extern void CMD_reset_encoders(void);
extern void CMD_set_config_value(uint8 grp, uint8 id, uint8* value);
extern int  CMD_send(void);

extern void commands_process_fsm(uint8 cmd, uint8 *param);

#ifdef __cplusplus 
}
#endif


#endif
