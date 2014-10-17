
#include "standard_includes.h"


//line crossing / candle circle / home circle detection
u08 lines_crossed=0;
u08 last_lines_crossed=0;
DEFINE_CFG2(u08,black,6,1);					
DEFINE_CFG2(u08,white,6,2);					
Evt_t line_alignment_start_evt;
Evt_t line_alignment_done_evt;



//normalizing the left/right line sensors via code is not needed if the "weaker" one of the 2 is driven via an additional resistor
#if 0
t_LOOKUP_table line_sensor_table[] = // R,L  I.E. Left senser reads a smaller value
{
	{16},
	{0		,0},
	{27		,10},
	{29		,13},
	{42		,19},
	{51		,26},	
	{55		,33},	
	{60		,38},	
	{78		,51},	
	{98		,78},
	{100	,84},	
	{129	,102},
	{177	,156},
	{183	,165},
	{188	,170},
	{199	,182},
	{255	,255},	
};
#endif



void line_detection_fsm_v2(u08 cmd, u08 *param)
{
	static u08 l_state=0;
	static u08 r_state=0;
	static s32 l_ticks=0;
	static s32 l_ticks_crossed=0;
	static s32 r_ticks=0;
	static s32 r_ticks_crossed=0;
	static s32 d_ticks=0;
	static u08 l_crossed=0;
	static u08 r_crossed=0;
	static u32 t_crossed=0;
	
	task_open();

	usb_printf("line_detection_fsm_v2()\n");

	PREPARE_CFG2(black);					
	PREPARE_CFG2(white);					

	UPDATE_CFG2(black);					
	UPDATE_CFG2(white);

	while(1)
	{
		if(l_state==0) //current on black...
		{
			if(s.line[LEFT_LINE] <= white) //now on white...
			{
				l_state=1;
				l_crossed=0;
				l_ticks = s.encoder_ticks; //s.inputs.encoders[0];
			}
		}
		if(l_state==1) //current on white...
		{
			if(s.line[LEFT_LINE] > black) //now back on black...
			{
				l_state=0;
				if(s.encoder_ticks /*inputs.encoders[0]*/ - l_ticks < 700) 
				{
					l_crossed=1;
					if(t_crossed==0) t_crossed=get_ms();
					l_ticks_crossed = s.encoder_ticks; //s.inputs.encoders[0];
				}
				dbg_printf("left line blip: %ld @ %ldms, %ld\n",s.encoder_ticks/*s.inputs.encoders[0]*/ - l_ticks,get_ms(),s.encoder_ticks); //s.inputs.encoders[0]);
			}
		}

		if(r_state==0) //current on black...
		{
			if(s.line[RIGHT_LINE] <= white) //now on white...
			{
				r_state=1;
				r_crossed=0;
				r_ticks = s.encoder_ticks /*s.inputs.encoders[1]*/;
			}
		}
		if(r_state==1) //current on white...
		{
			if(s.line[RIGHT_LINE] > black) //now back on black...
			{
				r_state=0;
				if(s.encoder_ticks /*s.inputs.encoders[1]*/ - r_ticks < 700)
				{
					r_crossed=1;
					if(t_crossed==0) t_crossed=get_ms();
					r_ticks_crossed = s.encoder_ticks /*s.inputs.encoders[1]*/;
				}
				dbg_printf("right line blip: %ld @ %ldms, %ld\n",s.encoder_ticks /*s.inputs.encoders[1]*/ - r_ticks, get_ms(), s.encoder_ticks /*s.inputs.encoders[1]*/);
			}
		}

		if( (l_crossed==1) && (r_crossed==1) )
		{
			if(r_ticks_crossed > l_ticks_crossed) d_ticks = r_ticks_crossed-l_ticks_crossed;
			else d_ticks = l_ticks_crossed-r_ticks_crossed;
			dbg_printf("crossed line w/ both sensors within %ldms  &  %ld ticks!\n", 
				get_ms() - t_crossed,
				d_ticks
			);
			if(d_ticks < 1500) 
			{
				lines_crossed++;
				dbg_printf("\n\nLINE! count=%d\n\n",lines_crossed);
			}
			t_crossed = 0;
			l_crossed = r_crossed = 0;
			l_ticks_crossed = r_ticks_crossed = 0;
		}

		/*
		if( (t_crossed!=0) && ((get_ms() - t_crossed) > 700) )
		{
			dbg_printf("not a line!\n");
			t_crossed = 0;
			l_crossed = r_crossed = 0;
		}
		*/

		OS_SCHEDULE;
		s.inputs.watch[1]=lines_crossed;
	}
	task_close();
}



void line_alignment_fsm(u08 cmd, u08 *param)
{
	task_open();

	usb_printf("line_alignment_fsm()\n");

	for(;;)
	{
		event_wait(line_alignment_start_evt);
		while(s.line[RIGHT_LINE] <= white) { motor_command(7,0,0,0,20); task_wait(10); } motor_command(7,0,0,0,0); //move right wheel fwd until off the line
		while(s.line[RIGHT_LINE] > white) { motor_command(7,0,0,0,-20); task_wait(10); } motor_command(7,0,0,0,0); //now move right whell backwards until back on the line
		while(s.line[LEFT_LINE] <= white) { motor_command(7,0,0,20,0); task_wait(10); } motor_command(7,0,0,0,0); //move left wheel fwd until off the line
		while(s.line[LEFT_LINE] > white) { motor_command(7,0,0,-20,0); task_wait(10); } motor_command(7,0,0,0,0); //now move left whell backwards until back on the line
		task_wait(100);
		event_signal(line_alignment_done_evt);
	}

	task_close();
}
