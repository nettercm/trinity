/*
 * serial.c
 *
 * Created: 11/22/2012 5:39:19 PM
 *  Author: Chris
 */ 

#include "standard_includes.h"

extern void commands_process(void);

uint8 rx_buffer[RX_BUFFER_SIZE];
//uint8 tx_buffer[128];
t_frame_to_pc tx_buffer;
t_frame_from_pc frame;
//Evt_t serial_cmd_evt;
u08 packets_received=0;

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
}



void serial_send_fsm(u08 cmd, u08 *param)
{
	static uint8 seq=0;
	//static uint8 interval_cfg_idx;
	//uint8 serial_send_interval;
	static uint8 p_r=0;
	static uint8 iterations;
	u08 i;
	static u08 initialized=0;

	//task_open();

	if(!initialized)
	{
		initialized=1;
		usb_printf("serial_send_fsm()\n");
		//interval_cfg_idx	= cfg_get_index_by_grp_and_id(1,1);
		p_r = packets_received;
	}
	
	//while(1)
	{
		//serial_send_interval = cfg_get_u08_by_index(interval_cfg_idx);
		//iterations=0;
		//task_wait(serial_send_interval); 
		tx_buffer.seq = seq; seq++; //=get_ms();
		tx_buffer.ack = packets_received;
		//packets_received = p_r = 0;
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
	
	//task_close();
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



void serial_receive_fsm(u08 cmd, u08 *param)
{
	static uint8 state=0;
	static uint32 count=0;
	static uint8 wr_idx=0,rd_idx=0,r=0;
	uint8 result = 0;
	t_commands* c;
	t_frame_from_pc *f;
	static u08 initialized=0;

	//task_open();

	if(!initialized)
	{
		initialized=1;
		usb_printf("serial_receive_fsm()\n");
		memset(rx_buffer,0,RX_BUFFER_SIZE);
		serial_receive_ring(UART_PC, (char*) rx_buffer, RX_BUFFER_SIZE);
	}

	//while(1)
	{
		//OS_SCHEDULE;
		//r1 is where the write point is now - that's where the next byte will be written to
		//r2 is where the write pointer was last, i.e. that byte has not yet been consumed
		wr_idx=serial_get_received_bytes(UART_PC);
		while(wr_idx!=rd_idx)
		{
			result = serial_process_byte(rx_buffer[rd_idx]);
			if(result)
			{
				memcpy(&(s.commands),(uint8*)(&frame)+6,sizeof(t_commands));
				packets_received++;
				//tx_buffer.ack=packets_received; //frame.seq;
				commands_process();
			}
			rd_idx++;
			if(rd_idx>=RX_BUFFER_SIZE) 
			{
				rd_idx=0; //wrap-around
			}
			wr_idx=serial_get_received_bytes(UART_PC);
		}
	}
	
	//task_close();
}

