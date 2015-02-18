#include "standard_includes.h"

//exec time: about 900us
void analog_update_fsm(u08 cmd, u08 *param)
{
	static uint8 count=0;
	static uint32 vbatt;
	uint8 i;
	static u08 initialized=0;

	//task_open();

	if(!initialized)
	{
		initialized = 1;

		usb_printf("analog_update_fsm()\n");

		//for(i=0;i<=7;i++) s.inputs.analog[i] = (analog_read(i))>>2;
		s.inputs.vbatt = read_battery_millivolts_svp();
		vbatt = read_battery_millivolts_svp();
	}
	
	//while(1)
	{
		count++;

		//for(i=0;i<=7;i++) s.inputs.analog[i] = (uint8)((((uint16)(s.inputs.analog[i]))*1 + (uint16)(analog_read(i)>>2) )/2);
		for(i=0;i<=15;i++)
		{
			set_digital_output(ANALOG_MUX_ADDR_0_PIN, i & 0x01);
			set_digital_output(ANALOG_MUX_ADDR_1_PIN, i & 0x02);
			set_digital_output(ANALOG_MUX_ADDR_2_PIN, i & 0x04);
			set_digital_output(ANALOG_MUX_ADDR_3_PIN, i & 0x08);
			s.inputs.analog[i] = (uint16)(analog_read(4)>>2);
		}
		
		s.line[RIGHT_LINE] = s.inputs.analog[AI_LINE_RIGHT];
		s.line[LEFT_LINE] = s.inputs.analog[AI_LINE_LEFT];

		//sample the following only once every 5 * 20 == 100ms
		if( count >= 20)
		{
			count = 0;
			vbatt = (vbatt*31UL + (uint32)read_battery_millivolts_svp())/32UL;
			s.inputs.vbatt = (uint16)vbatt;
		}

		//task_wait(interval);
	}

	//task_close();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
