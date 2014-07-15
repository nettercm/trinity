

#include "commands.h"
#include <memory.h>
#include <string.h>
#include "serial.h"
#include "data.h"
#include "ip.h"

t_commands commands;
t_frame_from_pc tx_buffer;


int CMD_send(void)
{
	int result=0;
	static uint8 seq=0;

	if(s.connection==0) return -1;

	if(commands.i > 0)
	{
		seq++;
		tx_buffer.seq = seq;

		tx_buffer.magic1[0] = 0xab;
		tx_buffer.magic1[1] = 0xcd;
		tx_buffer.magic2[0] = 0xdc;
		tx_buffer.magic2[1] = 0xba;

		memcpy(tx_buffer.payload+2, &commands, sizeof(t_commands));

		//now call the right send method
		if(s.connection==1) result = serial_write(s.p,(char*)&tx_buffer,sizeof(t_frame_from_pc));
		else if(s.connection==2) result = tcp_send((char*)&tx_buffer,sizeof(t_frame_from_pc),0);

		memset(&commands,0,sizeof(t_commands));
		memset(&tx_buffer,0,sizeof(t_frame_from_pc));
	}
	return result;
}



void CMD_append_byte(uint8 data)
{
	uint8 i = commands.i;
	uint8* c = &(commands.d[i]);

	*c = data;
	
	//set pointers to the byte after the command id
	c++; i++;
	
	//terminate the command w/ a 0x00
	*c = 0x00;

	//update the command structure to point to the next available free byte
	commands.i = i;
}


void CMD_append_data(uint8 *data, uint8 size)
{
	uint8 i = commands.i;
	uint8* c = &(commands.d[i]);

	memcpy(c,data,size);
	
	c+=size; i+=size;
	
	//terminate the command w/ a 0x00
	*c = 0x00;

	//update the command structure to point to the next available free byte
	commands.i = i;
}



void CMD_append_word(uint16 data)
{
	CMD_append_data((uint8*) &data,2);
}



void CMD_set_behavior_state(uint8 behavior_id, uint8 behavior_state)
{
	uint8 i = commands.i;
	uint8* c = &(commands.d[i]);

	//write the command identifier
	*c = CMD_SET_BEHAVIOR_STATE;
	
	//set pointers to the byte after the command id
	c++; i++;

	//now write the actual command data
	c[0]=behavior_id;
	c[1]=behavior_state;
	
	//increemnt pointers by command size
	c+=2; i+=2;
	
	//terminate the command w/ a 0x00
	*c = 0x00;

	//update the command structure to point to the next available free byte
	commands.i = i;
}



void CMD_reset_encoders(void)
{
	CMD_append_byte(CMD_RESET_ENCODERS);
}


void CMD_set_config_value(uint8 grp, uint8 id, uint8 *value)
{
	CMD_append_byte(CMD_SET_CONFIG_VALUE);
	CMD_append_byte(grp);
	CMD_append_byte(id);
	CMD_append_data(value, 4);
}


void CMD_sonar_set_timeout(unsigned char new_timeout)
{
	uint8 i = commands.i;
	uint8* c = &(commands.d[i]);

	//write the command identifier
	*c = CMD_SET_SONAR_TIMEOUT;
	//set pointers to the byte after the command id
	c++;
	i++;
	*c=new_timeout;
	//increemnt pointers by command size
	c+=1;
	i+=1;
	//terminate the command w/ a 0x00
	*c = 0x00;
	//update the command structure to point to the next available free byte
	commands.i = i;
}


void CMD_set_motors(sint16 lm, sint16 rm)
{
	uint8 i = commands.i;
	uint8* c = &(commands.d[i]);

	*c = CMD_SET_MOTORS;
	c++;
	i++;
	((t_set_motors_cmd*)c)->lm = lm;
	((t_set_motors_cmd*)c)->rm = rm;
	c+=sizeof(t_set_motors_cmd);
	i+=sizeof(t_set_motors_cmd);
	*c = 0x00;
	commands.i = i;
}



void CMD_motor_command(uint8 cmd, uint16 p1, uint16 p2, sint16 lm, sint16 rm)
{
	/*
	uint8 i = commands.i;
	uint8* c = &(commands.d[i]);

	*c = CMD_MOTOR_COMMAND;
	c++;
	i++;
	((t_motor_command*)c)->cmd = cmd;
	((t_motor_command*)c)->p1 = p1;
	((t_motor_command*)c)->p2 = p2;
	((t_motor_command*)c)->lm = lm;
	((t_motor_command*)c)->rm = rm;
	c+=sizeof(t_motor_command);
	i+=sizeof(t_motor_command);
	*c = 0x00;
	commands.i = i;
	*/

	CMD_append_byte(CMD_MOTOR_COMMAND);
	CMD_append_byte(cmd);
	CMD_append_word(p1);
	CMD_append_word(p2);
	CMD_append_word(lm);
	CMD_append_word(rm);

/*
	outputs->cmd = cmd;
	outputs->params[0]=time;
	outputs->params[1]=rm;
	outputs->params[2]=lm;
*/

}
