
#ifndef _data_h_
#define _data_h_

#include <windows.h>
#include "messages.h"

#ifdef __cplusplus 
extern "C" {
#endif


typedef struct
{
	unsigned short raw_value;	//8-bit a2d reading
	short real_value;	//scaled int, e.g. 0=0inches, 1=0.5inches, 2=1.0inches, etc.
} t_lookup;

typedef struct
{
	t_lookup *calibration_data;
	int slope;						//0=not changing;  1=increasing slowly; 10=increasing fast; -1=decreasign slowly
	unsigned char history_depth;
	unsigned char history_index;
	unsigned char raw_value;
	unsigned char raw_history[16];
	unsigned char raw_history_avg;
	short		  real_value;
	short         real_history[16];
	short		  real_history_avg;
} t_ir_sensor_state;

#pragma pack(1)
typedef struct
{
	int connection; //0=not connected,  1=serial,   2=tcp
	volatile HANDLE p;	//serial port
	char port[64];
	char msg[500];
	t_inputs *inputs;
	t_outputs outputs;
	t_ir_sensor_state ir_NN_state;
	t_ir_sensor_state ir_NW_state;
	t_ir_sensor_state ir_SW_state;
	t_ir_sensor_state ir_NE_state;
	t_ir_sensor_state ir_SE_state;
} t_state;


extern short lookup(t_lookup *table, unsigned short raw_value);
extern void lookup_test(void);
extern t_lookup lookup_table_ir_front[];
//extern t_ir_sensor_state ir_front_state;
extern void ir_sensor_update(t_ir_sensor_state *sensor_state, unsigned char raw_value);



extern void data_init(void);

extern void ir_test(void);

extern t_state s;

extern t_lookup lookup_table_compass[];


#ifdef __cplusplus 
}
#endif


#endif
