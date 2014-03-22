/*
 * serial.c
 *
 * Created: 11/22/2012 5:39:19 PM
 *  Author: Chris
 */ 

#include "standard_includes.h"

uint8 rx_buffer[RX_BUFFER_SIZE];
//uint8 tx_buffer[128];
t_frame_to_pc tx_buffer;
t_frame_from_pc frame;
Evt_t serial_cmd_evt;

#if _SERIAL_PORTS > 1

#define _serial_set_baud_rate(a,b)		serial_set_baud_rate(a,b)
#define _serial_send_blocking(a,b,c)	serial_send_blocking(a,b,c)
#define _serial_receive_ring(a,b,c)		serial_receive_ring(a,b,c)
#define _serial_get_received_bytes(a)	serial_get_received_bytes(a)
#define _serial_send_buffer_empty(a)	serial_send_buffer_empty(a)
#define _serial_send(a,b,c)				serial_send(a,b,c)
#define _serial_receive(a,b,c)			serial_receive(a,b,c)
#define _serial_receive_buffer_full(a)	serial_receive_buffer_full(a)
#define _usb_power_present()			usb_power_present()
#else

#define _serial_set_baud_rate(a,b)		serial_set_baud_rate(b)
#define _serial_send_blocking(a,b,c)	serial_send_blocking(b,c)
#define _serial_receive_ring(a,b,c)		serial_receive_ring(b,c)
#define _serial_get_received_bytes(a)	serial_get_received_bytes()
#define _serial_send_buffer_empty(a)	serial_send_buffer_empty()
#define _serial_send(a,b,c)				serial_send(b,c)
#define _serial_receive(a,b,c)			serial_receive(b,c)
#define _serial_receive_buffer_full(a)	serial_receive_buffer_full()
#define _usb_power_present()			0

#endif



void serial_test(void)
{
	//serial_test_1();
	//serial_test_2();
}


void serial_hardware_init(void)
{
	_serial_set_baud_rate(UART_PC, 115200);
	
	//serial_send_blocking(UART_PC, "test\r\n", 5);
	//serial_receive(UART_PC, rx_buffer, RX_BUFFER_SIZE);
	#if SIM
	{
		t_outputs *o = (t_outputs*)rx_buffer;
		o->magic1[0] = 0xab;
		o->magic1[1] = 0xcd;
		o->magic2[0] = 0xdc;
		o->magic2[1] = 0xba;
	}
	#endif
}



void serial_send_fsm(void)
{
	//static uint8 state=0;
	//static uint32 t_last=0, t_now=0;
	static uint8 seq=0;
	static uint8 interval_cfg_idx;
	uint8 serial_send_interval;
	u08 i;

	task_open();
	
	interval_cfg_idx	= cfg_get_index_by_grp_and_id(1,1);

	
	while(1)
	{
		serial_send_interval = cfg_get_u08_by_index(interval_cfg_idx);
		task_wait(serial_send_interval);
		tx_buffer.seq = seq; seq++; //=get_ms();
		//tx_buffer.ack = 0;
		tx_buffer.magic1[0] = 0xab;
		tx_buffer.magic1[1] = 0xcd;
		tx_buffer.magic2[0] = 0xdc;
		tx_buffer.magic2[1] = 0xba;
		memset(tx_buffer.payload,0,sizeof(tx_buffer.payload));
		if(dbg_buffer_read != dbg_buffer_write)
		{
			i=0;
			while( (dbg_buffer_read != dbg_buffer_write) && (i < (sizeof(t_inputs)-1)) )
			{
				tx_buffer.payload[2+i]=dbg_buffer[dbg_buffer_read];
				i++;
				dbg_buffer_read++;
				if(dbg_buffer_read>=DBG_BUFFER_SIZE) dbg_buffer_read=0;
			}
			tx_buffer.payload[0] = i;
			tx_buffer.payload[1] = 2;
		}
		else
		{
			tx_buffer.payload[0] = sizeof(t_inputs);
			tx_buffer.payload[1] = 1;
			s.inputs.timestamp = get_ms();
			memcpy(&(tx_buffer.payload[2]),&s.inputs,sizeof(t_inputs));
		}
		//for(i=0;i<sizeof(tx_buffer.payload);i++)tx_buffer.payload[i]=i;
		_serial_send(UART_PC,(char*)&tx_buffer,sizeof(t_frame_to_pc)); //seems to work
	}
	
	task_close();
}






uint8 serial_process_byte(uint8 byte)
{
	static uint8 state=1;
	static uint8 size=0;
	uint8 result=0;
	
	//usb_printf("%02X ",byte);
	
	switch(state)
	{
		case 1: //waiting for 0xab
		if(byte==0xab) 
		{
			((uint8*)(&frame))[size]=byte;
			state=2;
			size++;
		}			
		else
		{
			//usb_printf("%02X ",byte);
		}
		break;
		
		case 2: //waiting for 0xcd
		if(byte==0xcd) 
		{
			((uint8*)(&frame))[size]=byte;
			state=3;
			size++;
		}
		else 
		{ 
			usb_printf("#");
			s.inputs.flags++;
			state = 1; size=0; 
		}
		break;		
		
		case 3: //waiting for 0xdc @ the right position
		((uint8*)(&frame))[size]=byte;
		size++;
		if(size-1 == sizeof(t_frame_from_pc)-2)
		{
			if(byte==0xdc)
			{
				state=4;
			}
			else
			{
				usb_printf("$");
				s.inputs.flags++;
				state=1;
				size=0;
			}
		}
		break;
		
		case 4:
		((uint8*)(&frame))[size]=byte;
		if(byte==0xba)
		{
			result=1;
		}
		else
		{
			s.inputs.flags++;
			usb_printf("%");
		}
		size=0;
		state=1;
		break;
								
		default:
		break;
	}	
	return result;
}



void serial_receive_fsm(void)
{
	static uint8 state=0;
	static uint32 count=0;
	static uint8 received=0,wr_idx=0,rd_idx=0,r=0;
	uint8 result = 0;
	t_commands* c;
	t_frame_from_pc *f;

	task_open();
	
	memset(rx_buffer,0,RX_BUFFER_SIZE);
	serial_receive_ring(UART_PC,rx_buffer, RX_BUFFER_SIZE);

	while(1)
	{
		OS_SCHEDULE;
		//r1 is where the write point is now - that's where the next byte will be written to
		//r2 is where the write pointer was last, i.e. that byte has not yet been consumed
		wr_idx=serial_get_received_bytes(UART_PC);
		while(wr_idx!=rd_idx)
		{
			result = serial_process_byte(rx_buffer[rd_idx]);
			if(result)
			{
				//usb_printf("got one!\n");
				memcpy(&(s.commands),(uint8*)(&frame)+6,sizeof(t_commands));
				tx_buffer.ack=frame.seq;
				event_signal(serial_cmd_evt);
				OS_SCHEDULE;
			}
			rd_idx++;
			if(rd_idx>=RX_BUFFER_SIZE) 
			{
				//usb_printf("wrap\n");
				rd_idx=0; //wrap-around
			}
			wr_idx=serial_get_received_bytes(UART_PC);
		}
	}
	
	task_close();
}



#if 0

void serial_test_2(void)
{
	serial_receive_fsm(FSM_INIT_EVENT);
	while(1)
	{
		serial_receive_fsm(FSM_NULL_EVENT);
	}
}

void serial_test_1(void)
{
	uint8 i;
	uint32 received=0,r2=0,r1=0,r=0,sent=0,s=0;
	uint32 t1,t2;

	clear();
	lcd_goto_xy(0,0); 
	printf("Serial Test");
	_serial_send_blocking(UART_PC, "test1\r\n", 6);
	
	for(i=0;i<=127;i++) tx_buffer[i]=i;
	_serial_send_blocking(UART_PC, "test2\r\n", 6);

	t1=t2=get_ms();
	s=0;
	
	_serial_receive_ring(UART_PC,rx_buffer,128);
	
	while(1)
	{
		r1=_serial_get_received_bytes(UART_PC);
		if( r1 != r2)
		{
			if(r1>r2) received+=(r1-r2);
			if(r1<r2) received+=r1+(128-r2);
			r2=r1;
		}
		
		if( (get_ms()-t2 >= 20) && _serial_send_buffer_empty(UART_PC) )
		{
			t2=get_ms();
			_serial_send(UART_PC,tx_buffer,128);
			sent+=128;
		}
		if(get_ms()-t1>=1000)
		{
			t1=get_ms();
			lcd_goto_xy(0,1); printf("%4ld %4ld",sent-s,received-r);
			s=sent;
			r=received;
		}
	}
}
#endif





#if 0

void commands_receive_fsm_(void)
{
	static uint8 state=0;
	static uint32 count=0;
	uint8 result = 0;
	t_commands* c;
	t_frame_from_pc *f;

	task_open();
	
	if(state==0)
	{
		state = 1;
		rx_buffer[0] = 0;  rx_buffer[sizeof(t_frame_from_pc)-1] = 0;
		_serial_receive(UART_PC,rx_buffer, sizeof(t_frame_from_pc));
	}

	while(1)
	{
		OS_SCHEDULE;
		if(_serial_receive_buffer_full(UART_PC))
		{
			f =	(t_frame_from_pc*)rx_buffer;
			
			if(	(f->magic1[0] == 0xab) &&
				(f->magic1[1] == 0xcd) &&
				(f->magic2[0] == 0xdc) &&
				(f->magic2[1] == 0xba) )
			{
				
				//only update the actual outputs state if we received a correct packet
				count++;
				memcpy(&(s.commands),rx_buffer+6,sizeof(t_commands));
				//s.inputs.timestamp_rx=get_ms(); //s.commands.seq; //=get_ms();
				tx_buffer.ack=f->seq;
				memset(rx_buffer,0,sizeof(t_frame_from_pc));
				_serial_receive(UART_PC,rx_buffer, sizeof(t_frame_from_pc));
				event_signal(serial_cmd_evt);
				OS_SCHEDULE;
			}

			else
			{
				//s.outputs = &default_outputs;
				if(_usb_power_present())
				{
					int i;
					for(i=0;i<sizeof(t_frame_from_pc);i++)
					{
						usb_printf("%02X ",rx_buffer[i]);
					}
					usb_printf("\r\n");
				}
				s.inputs.flags++;
				//play_note(A(4), 50, 10);
				//rx_buffer[0] = 0;  rx_buffer[sizeof(t_outputs)-1] = 0;
				memset(rx_buffer,0,sizeof(t_frame_from_pc));
				_serial_receive(UART_PC,rx_buffer, sizeof(t_frame_from_pc));
			}
		}
	}
	
	task_close();
}
#endif


//0 means use the new version
#if 0
#if 1 

uint8 serial_receive_fsm(uint32 event)
{
	static uint8 state=0;
	static uint32 count=0;
	uint8 result = 0;
	t_outputs* o;

	if(event == FSM_INIT_EVENT)
	{
		state = 1;
		rx_buffer[0] = 0;  rx_buffer[sizeof(t_outputs)-1] = 0;
		_serial_receive(UART_PC,rx_buffer, sizeof(t_outputs));
		result = 0;
	}
	else
	{
		switch(state)
		{
			case 1:
			if(_serial_receive_buffer_full(UART_PC))
			{
				o =	(t_outputs*)rx_buffer;
				if(	(o->magic1[0] == 0xab) &&
				(o->magic1[1] == 0xcd) &&
				(o->magic2[0] == 0xdc) &&
				(o->magic2[1] == 0xba) )
				{
					//only update the actual outputs state if we received a correct packet
					count++;
					//s.outputs = (t_outputs*)rx_buffer;
					memcpy(&(s.outputs),rx_buffer,sizeof(t_outputs));
					s.inputs.timestamp_rx=s.outputs.timestamp; //=get_ms();
					state = 2;
					result = state;
					if((count%100)==0 && _usb_power_present())
					{
						int i;
						char string[8];
						for(i=0;i<sizeof(t_outputs);i++)
						{
							//sprintf(string,"%02X ",rx_buffer[i]);
							//serial_send_blocking(USB_COMM,string,3);
							usb_printf("%02X ",rx_buffer[i]);
						}
						//serial_send_blocking(USB_COMM,"OK \r\n",5);
						usb_printf("OK \r\n");
					}

				}
				else
				{
					//s.outputs = &default_outputs;
					if(_usb_power_present())
					{
						int i;
						char string[8];
						for(i=0;i<sizeof(t_outputs);i++)
						{
							//sprintf(string,"%02X ",rx_buffer[i]);
							//serial_send_blocking(USB_COMM,string,3);
							usb_printf("%02X ",rx_buffer[i]);
						}
						//serial_send_blocking(USB_COMM,"\r\n",2);
						usb_printf("\r\n");
					}
					s.inputs.flags++;
					//play_note(A(4), 50, 10);
					//rx_buffer[0] = 0;  rx_buffer[sizeof(t_outputs)-1] = 0;
					memset(rx_buffer,0,sizeof(t_outputs));
					_serial_receive(UART_PC,rx_buffer, sizeof(t_outputs));
				}
			}
			break;
			case 2:
			//rx_buffer[0] = 0;  rx_buffer[sizeof(t_outputs)-1] = 0;
			memset(rx_buffer,0,sizeof(t_outputs));
			_serial_receive(UART_PC,rx_buffer, sizeof(t_outputs));
			state = 1;
			result = state;
			break;
		}
	}
	return result;
}

#else


uint8 serial_receive_fsm(uint32 event)
{
	static uint8 state=0;
	static uint32 count=0;
	static uint8 received=0,r2=0,r1=0,r=0;
	uint8 result = 0;

	//r1 is where the write point is now - that's where the next byte will be written to
	//r2 is where the write pointer was last, i.e. that byte has not yet been consumed
	if(event == FSM_INIT_EVENT)
	{
		state = 1;
		#if SIM
		{
			t_outputs *o = (t_outputs*)rx_buffer;
			o->magic1[0] = 0xab;
			o->magic1[1] = 0xcd;
			o->magic2[0] = 0xdc;
			o->magic2[1] = 0xba;
		}
		#else
		memset(rx_buffer,0,RX_BUFFER_SIZE);
		#endif
		//rx_buffer[0] = 0;  rx_buffer[sizeof(t_outputs)-1] = 0;
		serial_receive_ring(UART_PC,rx_buffer, RX_BUFFER_SIZE);
		result = 0;
	}
	else
	{
		switch(state)
		{
			case 1:
			r1=serial_get_received_bytes(UART_PC);
			if( r1 != r2) //did new data arrive?
			{
				/*
				if(r1>r2) received=(r1-r2);
				if(r1<r2) received=r1+(RX_BUFFER_SIZE-r2); //wrap-around....
				r2=r1;
				*/
				while(r2!=r1)
				{
					result = serial_process_byte(rx_buffer[r2]);
					r2++;
					if(r2>=RX_BUFFER_SIZE) r2=0; //wrap-around
				}
			}
			break;
		}
	}
	return result;
}

#endif

#endif
