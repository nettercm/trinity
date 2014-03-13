

#include "standard_includes.h"

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

t_inputs sim_data[] =
{
//		t		a0	a1	a2	a3	a4	a5	a6	a7		s0	s1	s2	s3		i0	i1	i2	i3		enc			act. spd.	tgt. spd.	motors		x,y,thta	vb	f	watch
	{	0,		0,	0,	0,	0,	0,	0,	0,	0,		0,	0,	0,	0,		0,	0,	0,	0,		0,	0,		0,	0,		0,	0,		0,	0,		0,0,0,		0,	0,	0,0,0,0		},
	{	5,		0,	0,	0,	0,	0,	0,	0,	0,		0,	0,	0,	0,		0,	0,	0,	0,		0,	0,		0,	0,		0,	0,		0,	0,		0,0,0,		0,	0,	0,0,0,0		}
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


void sim(void)
{
	static u08 initialized=0;
	static u32 t_last = 0, t_now = 0;


	task_open();
	m.vbatt = 10000;
	m.rx_ring_buffer_size = 0;
	m.rx_ring_buffer = NULL;
	
	while(1)
	{
		task_wait(1);

		m.actual_enc_ab_ticks_per_interval = ((float)m.m2 / 1.83)/2;
		m.actual_enc_cd_ticks_per_interval = ((float)m.m1 / 1.83)/2;

		t_now = get_ms();
		if(t_now - t_last >= 10)
		{
			m.enc_ab += m.actual_enc_ab_ticks_per_interval;
			m.enc_cd += m.actual_enc_cd_ticks_per_interval;
			t_last = t_now;
		}
	}
	task_close();
}


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