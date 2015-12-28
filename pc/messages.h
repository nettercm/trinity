
#ifndef _messages_h_
#define _messages_h_


#ifdef __cplusplus 
extern "C" {
#endif

#define USE_LIDAR

#ifdef WIN32
typedef		unsigned char		uint8;
typedef		signed char			sint8;
typedef		unsigned short		uint16;
typedef		signed short		sint16;
typedef		unsigned long		uint32;
typedef		signed long			sint32;
#define __attribute__(a)
#endif




#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
typedef struct
{
	uint8 magic1[2]; 			//0xabcd
	uint8 seq;
	uint8 ack;					//seq number of the last packet received from the PC
#ifdef USE_LIDAR
	uint8  payload[92+108];
#else
	uint8  payload[92];
#endif
	uint8 magic2[2]; 			//0xdcba
}  __attribute__((__packed__)) t_frame_to_pc; //98 bytes
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
}  __attribute__((__packed__)) t_frame_from_pc; //22 bytes
#ifdef WIN32
#pragma pack(pop)
#endif

#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
typedef struct
{
	int num_samples;
	float angle;
	float samples[25];
}  __attribute__((__packed__)) t_lidar;
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
	uint8 analog[16];		//8

	uint16 sonar[6];		//8
	uint16 ir[8];			//8

	sint16 encoders[2];		//4
	sint16 actual_speed[2];	//4
	sint16 target_speed[2]; //4
	sint16 motors[2];		//4
	float x,y,theta;		//12
	uint16 vbatt;			//2
	uint16 flags;			//2
	sint16 watch[4];		//8
#ifdef USE_LIDAR
	t_lidar lidar;
#endif
}  __attribute__((__packed__)) t_inputs; //88 bytes
#ifdef WIN32
#pragma pack(pop)
#endif


#define INPUTS_HISTORY_SIZE 200000

#ifdef __cplusplus 
}
#endif


#endif
