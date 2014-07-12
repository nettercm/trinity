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
t_frame_to_pc dummy_frame;
t_inputs inputs;
t_outputs outputs;
char *serial_port=NULL;

volatile t_inputs inputs_history[200000]; //1 hour @ 50Hz
volatile int history_index=0;
int update_interval=0;

extern void print1(char *str);

char log_buffer[LOG_BUFFER_SIZE];
volatile int log_write_index=0;
volatile int log_read_index=0;


int	log_printf(const char *__fmt, ...)
{
	int size,i;
	va_list ap;
	char _b[500];

	va_start(ap, __fmt);
	size = vsprintf(_b, __fmt, ap);
	va_end(ap);
	for(i=0;i<size;i++)
	{
		log_buffer[log_write_index] = _b[i];
		log_write_index++;
		if(log_write_index>=LOG_BUFFER_SIZE) log_write_index=0;
		if(log_write_index==log_read_index)
		{
			log_write_index--;
			if(log_write_index<0) log_write_index=LOG_BUFFER_SIZE-1;
			return i+1;
		}
	}
	return i;
}


int serial_receive(HANDLE p, unsigned char *buffer)
{
	int result,size;

	size = serial_read(p,(char*)buffer,300);  
	result = size >= sizeof(t_frame_to_pc) ? 1 : 0; 

	if(result == 1)
	{
		//the last sizeof(t_inputs) number of bytes from the retrieved buffer contain the t_inputs message
		rx_buffer = (t_frame_to_pc*) &(buffer[size-sizeof(t_frame_to_pc)]);

		if(rx_buffer->payload[1]==2)
		{
			log_printf("%s",&(rx_buffer->payload[2]));
		}
		else
		{
			memcpy((void*)(&inputs),(void*) &(rx_buffer->payload[2]),sizeof(inputs));
			s.inputs = &inputs;
		}

		if( (rx_buffer->magic1[0] != 0xab) ||
			(rx_buffer->magic1[1] != 0xcd) ||
			(rx_buffer->magic2[0] != 0xdc) ||
			(rx_buffer->magic2[1] != 0xba) )
		{
			int i;
			log_printf("%02x %02x %02x %02x\n",rx_buffer->magic1[0],rx_buffer->magic1[1],rx_buffer->magic2[0],rx_buffer->magic2[1]);
			for(i=0; i<200; i++) log_printf("%02x",(unsigned int)buffer[i]);
			log_printf("\n");
			result = -1;
		}
	}
	else rx_buffer = &dummy_frame;
	return result;
}


void detect_packet_loss(void)
{
	static u08 previous_seq=0,previous_ack=0;
	if(rx_buffer->seq != (u08)(previous_seq+1))     log_printf("\n=============  PC missed a packet ==============\n\n");
	/* this detection does not work reliably */
	if(rx_buffer->ack != previous_ack)
	{
		if(rx_buffer->ack != (u08)(previous_ack+1)) log_printf("\n-------------  R. missed a packet --------------\n\n");
	}
	
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
	static u08 watch[4] = {0,0,0,0};

	if(t_last==0) t_last=GetTickCount();
	t_now = GetTickCount();
	t_delta = t_now-t_last;
	t_delta_avg = (t_delta_avg*19.0f + t_delta)/20.0f;

	//sprintf(s.msg, "%03u, %03u, %03u, %2.1f,   V,%3.1f,  0x%04x,  A,%03d,%03d,%03d,%03d,%03d,%03d,%03d,%03d,    FSM,%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d  E,%4d,%4d,  O,%4.1f,%4.1f,%4.1f,  %4.1f,%4.1f,%4.1f,    S,%4d,%4d,%4d,%4d,  I,%4d,%4d,%4d,%4d  M,%03d,%03d, %03d,%03d,  W,%03d,%03d,%03d,%03d\n",				
	sprintf(s.msg, "D_ ( %3u, %3u, %6ld,  T,%3d,%3.1f,   V,%3.1f,  0x%04x,  A,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,     E,%5d,%5d,   O,%4.1f,%4.1f,%4.1f,    S,%4d,%4d,%4d,%4d,  I,%4d,%4d,%4d,%4d,  M,%3d,%3d, %3d,%3d, %3d,%3d,   W,%3d,%3d,%3d,%3d )\n",				
				rx_buffer->seq,
				rx_buffer->ack,
				inputs->timestamp,
				t_delta, t_delta_avg,
				inputs->vbatt/1000.0f,
				inputs->flags,

				inputs->analog[0], inputs->analog[1], inputs->analog[2], inputs->analog[3], inputs->analog[4], inputs->analog[5], inputs->analog[6], inputs->analog[7], 

				//inputs->fsm_states[0],inputs->fsm_states[1],inputs->fsm_states[2],inputs->fsm_states[3],
				//inputs->fsm_states[4],inputs->fsm_states[5],inputs->fsm_states[6],inputs->fsm_states[7],

				inputs->encoders[0], inputs->encoders[1], 
				//odo_x, odo_y, (odo_theta/(2*PI))*360.0f,
				inputs->x, inputs->y, (inputs->theta/(2*PI))*360.0f,

				inputs->sonar[0],inputs->sonar[1],inputs->sonar[2],inputs->sonar[3],
				inputs->ir[0],inputs->ir[1], inputs->ir[2], inputs->ir[3], //inputs->sonar[6],inputs->sonar[7],
				
				inputs->target_speed[0], inputs->target_speed[1],
				inputs->actual_speed[0], inputs->actual_speed[1],
				inputs->motors[0], inputs->motors[1],

				inputs->watch[0], inputs->watch[1], inputs->watch[2], inputs->watch[3]
			);

	if( (t_delta >= update_interval) || (memcmp(watch,inputs->watch,4)!=0) )
	{
		printf(s.msg);
		//log_printf(s.msg);
		memcpy(watch,inputs->watch,4);
		t_last=t_now;
	}
}


int loop(void) //return 0 if we did not actually go throught the loop
{
	static int do_user_input=4;
	int result;
	uint8 buffer[500];

	//t2=timeGetTime();  td=t2-t1;  t1=t2;  //timestamping

	//wait for an incoming t_inputs message; data in buffer; inputs points to t_inputs payload 
	result = serial_receive(s.p,buffer);

	//if(result == 1)
	if(1) //let's do this regardless....
	{
		//ir_sensor_update(&s.ir_NN_state,inputs->analog[3]);

		//update the outputs and send t_outputs message back to the robot - note: may happen at a slower rate
		//outputs_update(s.p, inputs, &outputs);
		//if(--do_user_input<=0)
		{
			do_user_input = 2;
			process_user_input();
			//CMD_send(); //this was moved into process_user_input() and is now done only if required;
		}

		//kalman_update(&ks,inputs->analog[0]);

		odometry_update(inputs.encoders[0] - odo_last_l, inputs.encoders[1] - odo_last_r , &odo_x , &odo_y , &odo_theta);
		odo_last_l = inputs.encoders[0];
		odo_last_r = inputs.encoders[1];

		//display_inputs_and_state(s.inputs);
		
		display_inputs_and_state(&inputs);
		if(result) detect_packet_loss();

		inputs_history[history_index] = inputs;
		history_index++;
		if(history_index>=200000) history_index=0;
	}
	else 
	{
		log_printf("\nError.  serial_receive() failed with result=%d\n",result);
		//s.p = serial_reopen(s.p,s.port);
		//Sleep(2000);
	}

	if(!result) 
	{
		Sleep(20); //in case the port is open but no data is coming in....
		//log_printf("No data received!\r\n");
	}

	if(result)
	{

	}

	return result;
}

#ifndef WIN32UI


void serial_loopback_timing_test(void)
{
	DWORD t1,t2,td,min=2000,max=0,avg,sum=0;
	char rx_buffer[500],tx_buffer[500];
	int stats[10];
	int result,size;
	int i;

	for(i=1; i<=201; i+=10)
	{
		t1=timeGetTime();
		serial_write(s.p,tx_buffer,i);
		size = serial_read(s.p,(char*)rx_buffer,i);  
		t2=timeGetTime();
		printf("Timing test: %3lums for loopback for %3d of %3d bytes\n",t2-t1,abs(size),i);
	}

	i=0;
	while(1)//for(i=1; i<=100; i+=1)
	{
		Sleep(10);
		t1=timeGetTime();
		serial_write(s.p,tx_buffer,50);
		size = serial_read(s.p,(char*)rx_buffer,50);  
		t2=timeGetTime();
		td=t2-t1;
		if(td<min) min=td;
		if(td>max) max=td;
		//avg = ((19*avg)+td)/20;
		sum+=td;
		if(td<=8) stats[0]++;
		else if(td<=10) stats[1]++;
		else if(td<=15) stats[2]++;
		else if(td<=20) stats[3]++;
		else stats[4]++;
		i++;
		if(i==100)
		{
			i=0;
			avg=sum/100;
			sum=0;
			printf("50 bytes:  min, avg, max = %3lu, %3lu, %4lu   stats = %2d  %2d  %2d  %2d  %2d\n",
				min, avg, max, stats[0], stats[1], stats[2], stats[3], stats[4]);
			min=2000;
			max=0;
			memset(stats,0,sizeof(stats));
		}

		if(abs(size)!=50) printf("Timing test: %3lums for loopback for %3d of %3d bytes\n",t2-t1,abs(size),50);
	}

}

int main(int argc, char **argv)
{
	DWORD t1,t2;
	unsigned char buffer[500];
	int result,size;

	timeBeginPeriod(1);

	t1 = timeGetTime(); 	Sleep(1);		t2 = timeGetTime(); 		printf("Timing test:  Sleep(1) = %lu\n",t2-t1);
	t1 = timeGetTime(); 	Sleep(2);		t2 = timeGetTime(); 		printf("Timing test:  Sleep(2) = %lu\n",t2-t1);
	t1 = timeGetTime(); 	Sleep(10);		t2 = timeGetTime(); 		printf("Timing test:  Sleep(10) = %lu\n",t2-t1);

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

	{extern int ip_server_test(void); ip_server_test(); }

	data_init();
	//kalman_init(&ks,4.0,4.0,8.0,50);

	s.p=serial_open("\\\\.\\COM10"); //(s.port);
	serial_loopback_timing_test();
	//serial_test(p);

	while(1)  
	{
		// FLAG_EXIT will be set by the keyboard input processing logic if the ESC key was pressed
		if(flag_get(FLAG_EXIT,1)) break;  //exit if the ESC key was pressed

		loop();
	}
}

#endif