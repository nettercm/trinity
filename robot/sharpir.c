/*
 * sharpir.c
 *
 * Created: 11/21/2012 12:04:18 AM
 *  Author: Chris
 */ 
#include "standard_includes.h"
/*
t_LOOKUP_table SHARPIR_4to30_table[] =
{
	{19},
	{16		,400},
	{19		,300},
	{22		,250},
	{23		,240},		
	{28		,200},		
	{31		,180},		
	{35		,160},		
	{40		,140},		
	{43		,130},
	{46		,120},		
	{50		,110},
	{55		,100},
	{60		,90	},
	{67		,80	},
	{77		,70	},		
	{92		,60	},
	{105	,50	},
	{124	,40	},		
	{132	,30  }		
};
*/

t_LOOKUP_table SHARPIR_4to30_table[] =
{
	{18},
	{0		,999},
	{16		,999},
	{17		,340},
	{19		,300},
	{21		,260},
	{25		,220},
	{31		,180},
	{35		,160},
	{39		,140},
	{44		,120},
	{52		,100},
	{58		,90	},
	{64		,80	},
	{72		,70	},
	{84		,60	},
	{99		,50	},
	{120	,40	},
	{132	,30  }
};

#if 0
t_LOOKUP_table SHARPIR_10to80_table[] =
{
	{15},
	{9		,999},
	{10		,920},
	{17		,600},
	{23		,500},
	{32		,400},
	{40		,300},
	{66		,180},
	{91		,130},
	{97		,120},
	{105	,110},
	{113	,100},
	{121	,90	},
	{129	,80	},
	{136	,70	},
	{140	,60  } //
};
#endif
t_LOOKUP_table SHARPIR_10to80_table[] =
{
	{17},
	{0		,999},
	{11		,999},
	{12		,730},
	{17		,600},
	{22		,500},
	{28		,400},
	{38		,300},
	{46		,240},
	{63		,180},
	{90		,130},
	{97		,120},
	{105	,110},
	{113	,100},
	{121	,90	},
	{129	,80	},
	{135	,70	},
	{140	,60  } //
};




void SHARPIR_init(void)
{
	LOOKUP_initialize_table(SHARPIR_4to30_table);
	LOOKUP_initialize_table(SHARPIR_10to80_table);
}	

//timing:  10-20us
sint16 SHARPIR_get_real_value(uint8 sensor, sint16 raw_value)
{
	//if(sensor == AI_IR_N_long) return LOOKUP_do(raw_value, SHARPIR_10to80_table);
	//else return LOOKUP_do(raw_value, SHARPIR_4to30_table);
}

void SHARPIR_test(void)
{
	volatile s16 r;
	r = LOOKUP_do(0  , SHARPIR_4to30_table);
	r = LOOKUP_do(7  , SHARPIR_4to30_table);
	r = LOOKUP_do(24 , SHARPIR_4to30_table);
	r = LOOKUP_do(120, SHARPIR_4to30_table);
}


void SHARPIR_update_fsm(u08 cmd, u08 *param)
{
	static u08 initialized=0;
	s16 value;
	DEFINE_CFG2(s16,short_filter_threashold,2,1);					
	DEFINE_CFG2(s16,short_filter_amount,2,2);					
	DEFINE_CFG2(s16,long_filter_threashold,2,3);					
	DEFINE_CFG2(s16,long_filter_amount,2,4);					

	if(!initialized)
	{
		initialized=1;
		usb_printf("SHARPIR_update_fsm()\n");
	
		PREPARE_CFG2(short_filter_threashold);					
		PREPARE_CFG2(short_filter_amount);			
		PREPARE_CFG2(long_filter_threashold);			
		PREPARE_CFG2(long_filter_amount);						
	}

	
	{
		UPDATE_CFG2(short_filter_threashold);		
		UPDATE_CFG2(short_filter_amount);
		UPDATE_CFG2(long_filter_threashold);
		UPDATE_CFG2(long_filter_amount);
	}

	value = LOOKUP_do(s.inputs.analog[AI_IR_NW],SHARPIR_4to30_table);
	if(value > short_filter_threashold) value = ((s.ir[IR_NW]*short_filter_amount) + value)/(short_filter_amount+1);
	s.inputs.ir[IR_NW] = s.ir[IR_NW]		= value;

	value = LOOKUP_do(s.inputs.analog[AI_IR_N], SHARPIR_10to80_table);
	if(value > long_filter_threashold) value = ((s.ir[IR_N]*long_filter_amount) + value)/(short_filter_amount+1);
	s.inputs.ir[IR_N] = s.ir[IR_N]		= value;

	value = LOOKUP_do(s.inputs.analog[AI_IR_NE],SHARPIR_4to30_table);
	if(value > short_filter_threashold) value = ((s.ir[IR_NE]*short_filter_amount) + value)/(short_filter_amount+1);
	s.inputs.ir[IR_NE] = s.ir[IR_NE]		= value;
}

