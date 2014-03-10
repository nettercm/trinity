#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <stdarg.h>
#include <math.h>

#include "../robot/typedefs.h"
#include "../robot/pid.h"
#include "../robot/config.h"


#include "messages.h"
#include "serial.h"
#include "debug.h"
#include "kalman.h"
#include "utils.h"
#include "data.h"
#include "logic.h"
#include "commands.h"
#include "odometry.h"


t_frame_to_pc *rx_buffer;
t_inputs inputs;
t_outputs outputs;
char *serial_port=NULL;

t_inputs inputs_history[200000]; //1 hour @ 50Hz
int history_index=0;

extern void print1(char *str);


int serial_receive(HANDLE p, unsigned char *buffer)
{
	int result,size;

	size = serial_read(p,buffer,300);  
	result = size >= sizeof(t_frame_to_pc) ? 1 : 0; 

	if(result == 1)
	{
		//the last sizeof(t_inputs) number of bytes from the retrieved buffer contain the t_inputs message
		rx_buffer = (t_frame_to_pc*) &(buffer[size-sizeof(t_frame_to_pc)]);
		memcpy((void*)(&inputs),(void*) &(rx_buffer->payload[2]),sizeof(inputs));
		s.inputs = &inputs;

		if( (rx_buffer->magic1[0] != 0xab) ||
			(rx_buffer->magic1[1] != 0xcd) ||
			(rx_buffer->magic2[0] != 0xdc) ||
			(rx_buffer->magic2[1] != 0xba) )
		{
			int i;
			printf("%02x %02x %02x %02x\n",rx_buffer->magic1[0],rx_buffer->magic1[1],rx_buffer->magic2[0],rx_buffer->magic2[1]);
			for(i=0; i<200; i++) printf("%02x",(unsigned int)buffer[i]);
			printf("\n");
			result = -1;
		}
	}
	return result;
}


void detect_packet_loss(void)
{
				static u08 previous_seq=0,previous_ack=0;
				if(rx_buffer->seq != (u08)(previous_seq+1))     printf("\n=============  PC missed a packet ==============\n\n");
				/* this detection does not work reliably
				if(rx_buffer->ack != previous_ack)
				{
					if(rx_buffer->ack != (u08)(previous_ack+1)) printf("\n-------------  R. missed a packet --------------\n\n");
				}
				*/
				//if(inputs->timestamp_rx != previous_rx_seq+1) printf("TX ERROR (Robot missed a packet)\n");
				previous_seq=rx_buffer->seq;
				previous_ack=rx_buffer->ack;
				//previous_rx_seq=inputs->timestamp_rx;
}


void display_inputs_and_state(t_inputs *inputs)
{
	static u32 t_last=0;
	static float t_delta_avg=20.0f;
	u32 t_now,t_delta;

	if(t_last==0) t_last=GetTickCount();

	t_now = GetTickCount();
	t_delta = t_now-t_last;
	t_last=t_now;
	t_delta_avg = (t_delta_avg*19.0f + t_delta)/20.0f;

	sprintf(s.msg, "%03u, %03u, %03u, %2.1f,   V,%3.1f,  0x%04x,  A,%03d,%03d,%03d,%03d,%03d,%03d,%03d,%03d,    FSM,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d  E,%4d,%4d,  O,%4.1f,%4.1f,%4.1f,  %4.1f,%4.1f,%4.1f,    S,%4d,%4d,%4d,%4d,  I,%4d,%4d,%4d,%4d  M,%03d,%03d, %03d,%03d,  W,%03d,%03d,%03d,%03d\n",				
				rx_buffer->seq,
				rx_buffer->ack,
				t_delta, t_delta_avg,
				inputs->vbatt/1000.0f,
				inputs->flags,
				//size,
				//inputs->analog[3], inputs->analog[1], inputs->analog[0], inputs->analog[2], inputs->analog[4], 
				inputs->analog[0], inputs->analog[1], inputs->analog[2], inputs->analog[3], inputs->analog[4], inputs->analog[5], inputs->analog[6], inputs->analog[7], 
				//s.ir_SW_state.raw_history_avg,
				//s.ir_NW_state.raw_history_avg,
				//s.ir_NN_state.real_value,
				//s.ir_NE_state.raw_history_avg,
				//s.ir_SE_state.raw_history_avg,

				//inputs->analog[5],

				//s.ir_SW_state.real_history_avg,
				//s.ir_NN_state.real_history_avg,
				//s.ir_NW_state.real_history_avg,
				//s.ir_NE_state.real_history_avg,
				//s.ir_SE_state.real_history_avg,
				//inputs->compass_calibrated,
				//lookup(lookup_table_compass,inputs->compass_calibrated),
				//inputs->p1,
				//inputs->p2,
				inputs->fsm_states[0],inputs->fsm_states[1],inputs->fsm_states[2],inputs->fsm_states[3],
				inputs->fsm_states[4],inputs->fsm_states[5],inputs->fsm_states[6],inputs->fsm_states[7],
				//
				inputs->encoders[0], inputs->encoders[1], 
				odo_x, odo_y, (odo_theta/(2*PI))*360.0f,
				inputs->x, inputs->y, (inputs->theta/(2*PI))*360.0f,

				inputs->sonar[0],inputs->sonar[1],inputs->sonar[2],inputs->sonar[3],
				inputs->ir[0],inputs->ir[1], inputs->ir[2], inputs->ir[3], //inputs->sonar[6],inputs->sonar[7],
				
				inputs->target_speed[0], inputs->target_speed[1],
				inputs->motors[0], inputs->motors[1],

				inputs->watch[0], inputs->watch[1], inputs->watch[2], inputs->watch[3]
			);

		printf(s.msg);
}


int loop(void) //return 0 if we did not actually go throught the loop
{
	static int do_user_input=4;
	int result;
	uint8 buffer[500];

	//t2=timeGetTime();  td=t2-t1;  t1=t2;  //timestamping

	//wait for an incoming t_inputs message; data in buffer; inputs points to t_inputs payload 
	result = serial_receive(s.p,buffer);

	if(result == 1)
	{
		//ir_sensor_update(&s.ir_NN_state,inputs->analog[3]);

		//update the outputs and send t_outputs message back to the robot - note: may happen at a slower rate
		//outputs_update(s.p, inputs, &outputs);
		//if(--do_user_input<=0)
		{
			do_user_input = 2;
			process_user_input();
			CMD_send();
		}

		//kalman_update(&ks,inputs->analog[0]);

		odometry_update(inputs.encoders[0] - odo_last_l, inputs.encoders[1] - odo_last_r , &odo_x , &odo_y , &odo_theta);
		odo_last_l = inputs.encoders[0];
		odo_last_r = inputs.encoders[1];

		display_inputs_and_state(s.inputs);

		detect_packet_loss();

		inputs_history[history_index] = inputs;
		history_index++;
		if(history_index>=200000) history_index=0;
	}
	else 
	{
		printf("\nError.  serial_receive() failed with result=%d\n",result);
		s.p = serial_reopen(s.p,s.port);
		Sleep(2000);
	}
	return result;
}

#ifndef WIN32UI

int main(int argc, char **argv)
{
	timeBeginPeriod(1);
	memset(&outputs,0,sizeof(outputs));
	memset(&commands,0,sizeof(t_commands));

	strcpy(s.port,argv[1]);
	//s.port = argv[1];

	DBG(DBG_MAIN, ("++main().  sizeof(t_inputs)=%d, sizeof(t_frame_from_pc)=%d\n",sizeof(t_inputs),sizeof(t_frame_from_pc)));

	//test_find_peak();
	//test_keyboard();
	//odometry_test();
	//serial_test_2();
	//lookup_test();
	//ir_test();
	//PID_test_2();

	data_init();
	//kalman_init(&ks,4.0,4.0,8.0,50);

	s.p=serial_open(s.port);
	//serial_test(p);

	while(1)  
	{
		// FLAG_EXIT will be set by the keyboard input processing logic if the ESC key was pressed
		if(flag_get(FLAG_EXIT,1)) break;  //exit if the ESC key was pressed

		loop();
	}
}

#endif