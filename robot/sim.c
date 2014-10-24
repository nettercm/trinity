

#include "standard_includes.h"
#include "../pc/ip.h"



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
	enum states { s_none=0, s_init=1, s_waiting=2, s_connected};
	static enum states state=s_init;
	static enum states last_state=s_none;
	static u32 t_entry=0;
	int result;
	
	first_(s_init)
	{
		enter_(s_init)  //required!  this updates the "last_state" variable!
		{
			usb_printf("sim_serial(): initializing tcp server\n");
			tcp_server_init("127.0.0.1",2000);
		}
		state = s_waiting;		
	}

	next_(s_waiting)
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

		if(!tcp_is_alive()) state=s_init;

		exit_(s_connected)  
		{ 
			NOP();
		}
	}
}



void sim_task(u08 cmd, u08 *param)
{
	static u08 initialized=0;

	//task_open();

	if(!initialized)
	{
		initialized=1;

		usb_printf("sim_task()\n");

		m.vbatt = 10000;
		m.rx_ring_buffer_size = 0;
		m.rx_ring_buffer = NULL;
		s.inputs.vbatt=m.vbatt;
	}



	//for(;;)
	{
		//task_wait(20);
		{ extern void sim_step(void); sim_step(); }
		//sim_motors();
		sim_serial();
	}
	//task_close();
}


