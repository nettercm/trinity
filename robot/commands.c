
#include "standard_includes.h"


void commands_process(void)
{
	uint8 *c; //index into the data/payload
	uint8 *end;
	uint8 i;
	uint8 cmd;
	t_config_value *v;
	t_motor_command *motor_cmd;
		
	c  = (uint8*) &(s.commands);
	end= c+sizeof(t_commands);
	//usb_printf("%08ld: commands_process(end=0x%02x, c=0x%02x:  ",get_ms(),end,c);
	//for(i=0;i<sizeof(t_commands);i++)	{	usb_printf("%02x ",c[i]); 	}
	//usb_printf(")\n");

	c = &(s.commands.d[0]);
	
	while( (cmd=*c)!=0 )  //0 means no more commands
	{
		//usb_printf("0x%04x: 0x%02x\n",c,cmd);
		c++;
		switch(cmd)
		{
			case CMD_SET_MOTORS:
				s.lm_target = s.lm_actual = ((t_set_motors_cmd*)c)->lm;
				s.rm_target = s.rm_actual = ((t_set_motors_cmd*)c)->rm;
				//don't actually update the motors just yet - let the motor command fsm do that
				c+=sizeof(t_set_motors_cmd);
			break;


			case CMD_MOTOR_COMMAND:
				motor_cmd = (t_motor_command*)c;
				motor_command(motor_cmd->cmd, motor_cmd->p1, motor_cmd->p2, motor_cmd->lm, motor_cmd->rm);
				c+=sizeof(t_motor_command);
			break;


			case CMD_RESET_ENCODERS:
				encoders_reset();
				c+=0; //this command has no parameters
			break;
			

			case CMD_SET_BEHAVIOR_STATE:
				play_note(A(4), 50, 10);			
				s.behavior_state[ c[0] ] = c[1];
				usb_printf("CMD_SET_BEHAVIOR_STATE %02x %02x\r\n",c[0],c[1]);
				c+=2;
			break;
			

			case CMD_SET_SONAR_SEQUENCE:
			break;
			

			case CMD_SET_SONAR_TIMEOUT:
				ultrasonic_set_timeout(c[0]);
				c+=1;
			break;


			case CMD_SET_CONFIG_VALUE:
				v = (t_config_value *)&(c[2]);
				usb_printf("config %d,%d = %d\r\n",c[0],c[1],v->s16);
				cfg_set_value_by_grp_id(c[0], c[1], *v);
				c+=6;
			break;
			

			default:
			break;
		}
		if(c>=end) break;
	}
	
}

#if 0
void commands_process_fsm(u08 cmd, u08 *param)
{
	task_open();

	usb_printf("commands_process_fsm()\n");
	
	for(;;)
	{
		event_wait(serial_cmd_evt);
		commands_process();
	}
	
	task_close();
}
#endif
