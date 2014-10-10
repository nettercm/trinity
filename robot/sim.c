

#include "standard_includes.h"
#include "../pc/ip.h"

/*
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
*/


/*
D_
( 
036, 008, 492655,   V,9.1,  0x0000,  
A,062,049,247,250,030,021,014,058,     
E,08115,17165,  O,1702.2, 0.0,89.1,  664.5,-845.8,86.2,    
S,0000,0000,0000,0000,  I,0090,0997,0186,0520  
M,000,000, 000,000,  
W,002,013,000,000 
)

*/


#define D_(seq1,seq2,t, V,v, flags,  A,a0,a1,a2,a3,a4,a5,a6,a7,  E,e0,e1,  O,x,y,theta,   S,s0,s1,s2,s3,   I,i0,i1,i2,i3,    M,mt0,mt1,ma0,ma1,mp0,mp1,   W,w0,w1,w2,w3) \
	{ t,  {a0,a1,a2,a3,a4,a5,a6,a7},  {s0,s1,s2,s3},  {i0,i1,i2,i3},  {e0,e1},  {ma0,ma1}, {mt0,mt1}, {mp0,mp1},  x,y,theta,  v,flags,  {w0,w1,w2,w3} },


t_inputs sim_data[] =
{
#ifdef WIN32
//#include "../data1.txt"
#endif
//D_ (   0,   0,      0,   V,0.0,  0x0000,  A,  0,  0,  0,  0,  0,  0,  0,  0,     E,    0,    0,   O, 0.0, 0.0, 0.0,    S,   0,   0,   0,   0,  I,   0,   0,   0,   0,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  0,  0 )
	{0}
};



#ifdef WIN32
int	lcd_printf(const char *__fmt, ...)
{
	int size;
	char buffer[255];
	va_list ap;
	va_start(ap, __fmt);
	size = vsprintf(buffer, __fmt, ap);
	va_end(ap);
	return 0;
}
#endif



void sim_motors(void)
{
	static u32 t_last = 0, t_now = 0;

	m.actual_enc_ab_ticks_per_interval = (s16) ((float)m.m2 / 1.83f)/2;
	m.actual_enc_cd_ticks_per_interval = (s16) ((float)m.m1 / 1.83f)/2;

	t_now = get_ms();
	if(t_now - t_last >= 10)
	{
		m.enc_ab += m.actual_enc_ab_ticks_per_interval;
		m.enc_cd += m.actual_enc_cd_ticks_per_interval;
		t_last = t_now;
	}
}


void sim_serial(void)
{
	enum states { s_none=0, s_waiting=1, s_connected};
	static enum states state=s_waiting;
	static enum states last_state=s_none;
	static u32 t_entry=0;
	int result;
	
	first_(s_waiting)
	{
		enter_(s_waiting)  //required!  this updates the "last_state" variable!
		{  
			usb_printf("sim_serial(): waiting for UI connection\n");
		}

		result = tcp_server_accept(1);
		if(result > 0) 
		{
			state = s_connected;
		}

		exit_(s_waiting)  
		{ 
			NOP();
		}
	}

	next_(s_connected)
	{
		enter_(s_connected)  //required!  this updates the "last_state" variable!
		{  
			NOP();
		}

		exit_(s_connected)  
		{ 
			NOP();
		}
	}
}

void sim_task(u08 cmd, u08 *param)
{
	static u08 initialized=0;


	task_open();

	usb_printf("sim_task()\n");

	m.vbatt = 10000;
	m.rx_ring_buffer_size = 0;
	m.rx_ring_buffer = NULL;

	tcp_server_init("127.0.0.1",2000);

	for(;;)
	{
		task_wait(20);
		{ extern void sim_step(void); sim_step(); }
		sim_motors();
		sim_serial();
	}
	task_close();
}




/*
char sample[] = "099, 001, 002, 19.0,   V,9.2,  0xA001,  A,047,046,046,045,059,058,027,232,     E,00001,   0,  O, 0.1, 0.0,-0.0,   0.1, 0.0,-0.0,    S,4000,4000,   0,   0,  I, 175, 206,  88, 217  M,100,001, -01,002,  W,001,002,003,004";

	t_frame_to_pc rx_buffer_;
	t_frame_to_pc *rx_buffer = &rx_buffer_;
	t_inputs inputs_;
	t_inputs *inputs = & inputs_;
	u32 t_delta;
	float t_delta_avg;

	float odo_x, odo_y, odo_theta;


	sscanf(sample, "%03u, %03u, %03u, %f,   V,%f,  0x%04x,  A,%03d,%03d,%03d,%03d,%03d,%03d,%03d,%03d,     E,%05d,%d,  O,%f,%f,%f,  %f,%f,%f,    S,%4d,%4d,%4d,%4d,  I,%4d,%4d,%4d,%4d  M,%03d,%03d, %03d,%03d,  W,%03d,%03d,%03d,%03d\n",
				&(rx_buffer->seq),
				&(rx_buffer->ack),
				&(t_delta), 
				&(t_delta_avg),
				&(inputs->vbatt),
				&(inputs->flags),

				&(inputs->analog[0]),
				&(inputs->analog[1]), 
				&(inputs->analog[2]), 
				&(inputs->analog[3]), 
				&(inputs->analog[4]), 
				&(inputs->analog[5]), 
				&(inputs->analog[6]), 
				&(inputs->analog[7]), 

				//inputs->fsm_states[0],inputs->fsm_states[1],inputs->fsm_states[2],inputs->fsm_states[3],
				//inputs->fsm_states[4],inputs->fsm_states[5],inputs->fsm_states[6],inputs->fsm_states[7],

				&(inputs->encoders[0]), 
				&(inputs->encoders[1]), 
				&(odo_x), 
				&(odo_y), 
				&(odo_theta),

				&(inputs->x), 
				&(inputs->y), 
				&(inputs->theta),

				&(inputs->sonar[0]),
				&(inputs->sonar[1]),
				&(inputs->sonar[2]),
				&(inputs->sonar[3]),

				&(inputs->ir[0]),
				&(inputs->ir[1]), 
				&(inputs->ir[2]), 
				&(inputs->ir[3]), //inputs->sonar[6],inputs->sonar[7],
				
				&(inputs->target_speed[0]), 
				&(inputs->target_speed[1]),
				&(inputs->motors[0]), 
				&(inputs->motors[1]),

				&(inputs->watch[0]), 
				&(inputs->watch[1]), 
				&(inputs->watch[2]), 
				&(inputs->watch[3])
	);


*/

/*
space, up, up, up, down, down, down, left, right, right, left, x, f, r, f, 0

AB,CD,01,00,00,00,0A,02,02,00,00,00,00,00,00,00,00,00,00,00,DC,BA,00

AB,CD,02,00,00,00,0A,02,07,00,00,00,00,0A,00,0A,00,00,00,00,DC,BA,00
AB,CD,03,00,00,00,0A,02,07,00,00,00,00,0F,00,0F,00,00,00,00,DC,BA,00

AB,CD,04,00,00,00,0A,02,07,00,00,00,00,14,00,14,00,00,00,00,DC,BA,00
AB,CD,05,00,00,00,0A,02,07,00,00,00,00,19,00,19,00,00,00,00,DC,BA,00

AB,CD,06,00,00,00,0A,02,07,00,00,00,00,1E,00,1E,00,00,00,00,DC,BA,00

AB,CD,07,00,00,00,0A,02,07,00,00,00,00,19,00,19,00,00,00,00,DC,BA,00
AB,CD,08,00,00,00,0A,02,07,00,00,00,00,14,00,14,00,00,00,00,DC,BA,00

AB,CD,09,00,00,00,0A,02,07,00,00,00,00,0F,00,0F,00,00,00,00,DC,BA,00
AB,CD,0A,00,00,00,0A,02,07,00,00,00,00,0A,00,0A,00,00,00,00,DC,BA,00

AB,CD,0B,00,00,00,0A,02,07,00,00,00,00,05,00,05,00,00,00,00,DC,BA,00
AB,CD,0C,00,00,00,0A,02,07,00,00,00,00,00,00,00,00,00,00,00,DC,BA,00

AB,CD,0D,00,00,00,0A,02,07,00,00,00,00,F6,FF,0A,00,00,00,00,DC,BA,00
AB,CD,0E,00,00,00,0A,02,07,00,00,00,00,FB,FF,05,00,00,00,00,DC,BA,00
AB,CD,0F,00,00,00,0A,02,07,00,00,00,00,00,00,00,00,00,00,00,DC,BA,00
AB,CD,10,00,00,00,0A,02,07,00,00,00,00,0A,00,F6,FF,00,00,00,DC,BA,00
AB,CD,11,00,00,00,0A,02,07,00,00,00,00,05,00,FB,FF,00,00,00,DC,BA,00
AB,CD,12,00,00,00,0A,02,07,00,00,00,00,00,00,00,00,00,00,00,DC,BA,00
AB,CD,13,00,00,00,01,06,00,00,00,00,00,00,00,00,00,00,00,00,DC,BA,00
AB,CD,14,00,00,00,0A,02,07,00,00,00,00,FF,00,FF,00,00,00,00,DC,BA,00
AB,CD,15,00,00,00,0A,02,07,00,00,00,00,01,FF,01,FF,00,00,00,DC,BA,00
AB,CD,16,00,00,00,0A,02,07,00,00,00,00,FF,00,FF,00,00,00,00,DC,BA,00
AB,CD,17,00,00,00,0A,02,07,00,00,00,00,FA,00,FA,00,00,00,00,DC,BA,00
AB,CD,18,00,00,00,0A,02,07,00,00,00,00,F5,00,F5,00,00,00,00,DC,BA,00
AB,CD,19,00,00,00,0A,02,07,00,00,00,00,F0,00,F0,00,00,00,00,DC,BA,00
AB,CD,1A,00,00,00,0A,02,07,00,00,00,00,EB,00,EB,00,00,00,00,DC,BA,00
AB,CD,1B,00,00,00,0A,02,07,00,00,00,00,E6,00,E6,00,00,00,00,DC,BA,00
AB,CD,1C,00,00,00,0A,02,07,00,00,00,00,E1,00,E1,00,00,00,00,DC,BA,00
AB,CD,1D,00,00,00,0A,02,07,00,00,00,00,DC,00,DC,00,00,00,00,DC,BA,00
AB,CD,1E,00,00,00,0A,02,07,00,00,00,00,D7,00,D7,00,00,00,00,DC,BA,00
AB,CD,1F,00,00,00,0A,02,07,00,00,00,00,D2,00,D2,00,00,00,00,DC,BA,00
AB,CD,20,00,00,00,0A,02,07,00,00,00,00,CD,00,CD,00,00,00,00,DC,BA,00
AB,CD,21,00,00,00,0A,02,07,00,00,00,00,C8,00,C8,00,00,00,00,DC,BA,00
AB,CD,22,00,00,00,0A,02,07,00,00,00,00,C3,00,C3,00,00,00,00,DC,BA,00
AB,CD,23,00,00,00,0A,02,07,00,00,00,00,BE,00,BE,00,00,00,00,DC,BA,00
AB,CD,24,00,00,00,0A,02,07,00,00,00,00,B9,00,B9,00,00,00,00,DC,BA,00
AB,CD,25,00,00,00,0A,02,07,00,00,00,00,B4,00,B4,00,00,00,00,DC,BA,00
AB,CD,26,00,00,00,0A,02,07,00,00,00,00,AF,00,AF,00,00,00,00,DC,BA,00
AB,CD,27,00,00,00,0A,02,07,00,00,00,00,AA,00,AA,00,00,00,00,DC,BA,00
AB,CD,28,00,00,00,0A,02,07,00,00,00,00,A5,00,A5,00,00,00,00,DC,BA,00
AB,CD,29,00,00,00,0A,02,07,00,00,00,00,A0,00,A0,00,00,00,00,DC,BA,00
AB,CD,2A,00,00,00,0A,02,07,00,00,00,00,9B,00,9B,00,00,00,00,DC,BA,00
AB,CD,2B,00,00,00,0A,02,07,00,00,00,00,96,00,96,00,00,00,00,DC,BA,00
AB,CD,2C,00,00,00,0A,02,07,00,00,00,00,91,00,91,00,00,00,00,DC,BA,00
AB,CD,2D,00,00,00,0A,02,07,00,00,00,00,8C,00,8C,00,00,00,00,DC,BA,00
AB,CD,2E,00,00,00,0A,02,07,00,00,00,00,87,00,87,00,00,00,00,DC,BA,00
AB,CD,2F,00,00,00,0A,02,07,00,00,00,00,82,00,82,00,00,00,00,DC,BA,00
AB,CD,30,00,00,00,0A,02,07,00,00,00,00,7D,00,7D,00,00,00,00,DC,BA,00
AB,CD,31,00,00,00,0A,02,07,00,00,00,00,78,00,78,00,00,00,00,DC,BA,00
AB,CD,32,00,00,00,0A,02,07,00,00,00,00,73,00,73,00,00,00,00,DC,BA,00
AB,CD,33,00,00,00,0A,02,07,00,00,00,00,6E,00,6E,00,00,00,00,DC,BA,00
AB,CD,34,00,00,00,0A,02,07,00,00,00,00,69,00,69,00,00,00,00,DC,BA,00
AB,CD,35,00,00,00,0A,02,07,00,00,00,00,64,00,64,00,00,00,00,DC,BA,00
AB,CD,36,00,00,00,0A,02,07,00,00,00,00,5F,00,5F,00,00,00,00,DC,BA,00
AB,CD,37,00,00,00,0A,02,07,00,00,00,00,5A,00,5A,00,00,00,00,DC,BA,00
AB,CD,38,00,00,00,0A,02,07,00,00,00,00,55,00,55,00,00,00,00,DC,BA,00
AB,CD,39,00,00,00,0A,02,07,00,00,00,00,50,00,50,00,00,00,00,DC,BA,00
AB,CD,3A,00,00,00,0A,02,07,00,00,00,00,4B,00,4B,00,00,00,00,DC,BA,00
AB,CD,3B,00,00,00,0A,02,07,00,00,00,00,46,00,46,00,00,00,00,DC,BA,00
AB,CD,3C,00,00,00,0A,02,07,00,00,00,00,41,00,41,00,00,00,00,DC,BA,00
AB,CD,3D,00,00,00,0A,02,07,00,00,00,00,3C,00,3C,00,00,00,00,DC,BA,00
AB,CD,3E,00,00,00,0A,02,07,00,00,00,00,37,00,37,00,00,00,00,DC,BA,00
AB,CD,3F,00,00,00,0A,02,07,00,00,00,00,32,00,32,00,00,00,00,DC,BA,00
AB,CD,40,00,00,00,0A,02,07,00,00,00,00,2D,00,2D,00,00,00,00,DC,BA,00
AB,CD,41,00,00,00,0A,02,07,00,00,00,00,28,00,28,00,00,00,00,DC,BA,00
AB,CD,42,00,00,00,0A,02,07,00,00,00,00,23,00,23,00,00,00,00,DC,BA,00
AB,CD,43,00,00,00,0A,02,07,00,00,00,00,1E,00,1E,00,00,00,00,DC,BA,00
AB,CD,44,00,00,00,0A,02,07,00,00,00,00,19,00,19,00,00,00,00,DC,BA,00
AB,CD,45,00,00,00,0A,02,07,00,00,00,00,14,00,14,00,00,00,00,DC,BA,00
AB,CD,46,00,00,00,0A,02,07,00,00,00,00,0F,00,0F,00,00,00,00,DC,BA,00
AB,CD,47,00,00,00,0A,02,07,00,00,00,00,0A,00,0A,00,00,00,00,DC,BA,00
AB,CD,48,00,00,00,0A,02,07,00,00,00,00,05,00,05,00,00,00,00,DC,BA,00
AB,CD,49,00,00,00,0A,02,07,00,00,00,00,00,00,00,00,00,00,00,DC,BA,00
*/