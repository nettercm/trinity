/*
 * sharpir.c
 *
 * Created: 11/21/2012 12:04:18 AM
 *  Author: Chris
 */ 

#include "typedefs.h"
#include "lookup.h"
#include "hardware.h"

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
	{14		,999},
	{15		,400},
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




void SHARPIR_init(void)
{
	LOOKUP_initialize_table(SHARPIR_4to30_table);
	LOOKUP_initialize_table(SHARPIR_10to80_table);
}	

//timing:  10-20us
sint16 SHARPIR_get_real_value(uint8 sensor, sint16 raw_value)
{
	if(sensor == AI_IR_N_long) return LOOKUP_do(raw_value, SHARPIR_10to80_table);
	else return LOOKUP_do(raw_value, SHARPIR_4to30_table);
}

void SHARPIR_test(void)
{
	volatile s16 r;
	r = LOOKUP_do(0  , SHARPIR_4to30_table);
	r = LOOKUP_do(7  , SHARPIR_4to30_table);
	r = LOOKUP_do(24 , SHARPIR_4to30_table);
	r = LOOKUP_do(120, SHARPIR_4to30_table);
}