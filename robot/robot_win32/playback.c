
#include <Windows.h>
#include <timeapi.h>
#include <stdio.h>
#include <conio.h>
#include <stdarg.h>
#include "cocoos/cocoos.h"

#include "standard_includes.h"

FILE *capture_file=NULL;
t_inputs i;
int enabled=0;

void playback_sim_init(char *path)
{
	m.elapsed_milliseconds = 0;
	m.enc_ab = 0;
	m.enc_cd = 0;
	capture_file=fopen("c:\\Temp\\2015-03-21-18-10-54-capture_file.txt","r");
}


void playback_sim_step(void)
{
	if(enabled)
	{
		//we should have changed state just like in the capture file!
		if(i.watch[2] != s.behavior_state[MASTER_LOGIC_FSM])
		{
			printf("%6d:  ML state:  s.behavior_state[3]=%d  i.watch[2]=%d\n",i.timestamp,s.behavior_state[3],i.watch[2]);
			//if(s.behavior_state[3]<2) s.behavior_state[3] = i.watch[2];
		}
	}
}


void playback_sim_outputs(void)
{
}


void playback_sim_inputs(void)
{
	char line[500];
	char *str;
	static char last_line[500];
	static s16 l_enc=0, r_enc=0;
	int count;
	int sleep_time = 0;

	Sleep(sleep_time);
	
	if(_kbhit())
	{
		int c;
		c = _getch();

		if(c=='s')
		{
			sleep_time = 10;
			enabled=1;
		}
		if (c == 'S')
		{
			sleep_time = 0;
			enabled = 1;
		}
	}

	if(!enabled) 
	{
		s.inputs.analog[AI_START_BUTTON]=255; //otherwise, control logic will think the start button was pressed.
		Sleep(20);
		return;
	}

	str = fgets(line,500,capture_file);

	if (str == NULL)
	{
		enabled = 0;
		return;
	}

	count = sscanf(line,
		"%d,%d,%d,"
		"%d,%d,%d,%d,%d,%d,%d,%d,"
		"%d,%d,%d,%d,%d,%d,%d,%d,"
		"%d,%d,%d,%d,%d,%d,"
		"%d,%d,%d,%d,%d,%d,%d,%d,"
		"%d,%d,"
		"%f,%f,%f,"
		"%d,%d,"
		"%d,%d,"
		"%d,%d,"
		"%d",
		&(i.timestamp), &(i.vbatt), &(i.flags),
		&(i.analog[0]), &(i.analog[1]), &(i.analog[2]), &(i.analog[3]), &(i.analog[4]), &(i.analog[5]), &(i.analog[6]), &(i.analog[7]),
		&(i.analog[8]), &(i.analog[9]), &(i.analog[10]), &(i.analog[11]), &(i.analog[12]), &(i.analog[13]), &(i.analog[14]), &(i.analog[15]),
		&(i.sonar[0]), &(i.sonar[1]), &(i.sonar[2]), &(i.sonar[3]), &(i.sonar[4]), &(i.sonar[5]),
		&(i.ir[0]), &(i.ir[1]), &(i.ir[2]), &(i.ir[3]), &(i.ir[4]), &(i.ir[5]), &(i.ir[6]), &(i.ir[7]),
		&(i.encoders[0]), &(i.encoders[1]),
		&(i.x), &(i.y), &(i.theta),
		&(i.target_speed[0]), &(i.target_speed[1]),
		&(i.actual_speed[0]), &(i.actual_speed[1]),
		&(i.motors[0]), &(i.motors[1]),
		&(i.watch[2])//, &(i.watch[1]), &(i.watch[2]), &(i.watch[3])
	);

	if (i.watch[2] != s.behavior_state[3])
	{
		printf("%6d:  ML should change state:  s.behavior_state[3]=%d  i.watch[2]=%d\n", i.timestamp, s.behavior_state[3], i.watch[2]);
		if ((s.behavior_state[3] == 1) && (i.watch[2] == 2))
		{
			force_start_signal(1);
		}
	}

	if ((i.timestamp - s.inputs.timestamp > 25) || (i.timestamp - s.inputs.timestamp < 15))
	{
		printf("%6d:  Unexpected timestamp change from %d to %d\n", i.timestamp, s.inputs.timestamp, i.timestamp);
	}

	if (count != 45)
	{
		count = 45; //breakpoint
	}
	memcpy(last_line, line, sizeof(line));

	m.elapsed_milliseconds = i.timestamp;
	m.vbatt = i.vbatt;
	m.enc_ab += i.actual_speed[0]; //i.encoders[0];
	m.enc_cd += i.actual_speed[1]; //i.encoders[1];

	//under WIN32, we don't run the post-processing that is associated w/ A2D input capture, so we need to fill in the s. struct values directly
	s.line[RIGHT_LINE] = i.analog[AI_LINE_RIGHT];
	s.line[LEFT_LINE] = i.analog[AI_LINE_LEFT];
	s.ir[0] = i.ir[0];
	s.ir[1] = i.ir[1];
	s.ir[2] = i.ir[2];
	s.ir[3] = i.ir[3];
	s.ir[4] = i.ir[4];
	s.ir[5] = i.ir[5];
	s.ir[6] = i.ir[6];
	s.ir[7] = i.ir[7];

	memcpy(s.inputs.analog,i.analog,sizeof(i.analog));
	memcpy(s.inputs.ir,i.ir,sizeof(i.ir));
	memcpy(s.inputs.sonar,i.sonar,sizeof(i.sonar));
	s.inputs.timestamp  = i.timestamp;
	s.inputs.vbatt		= i.vbatt;

}

