
#include "data.h"


t_state s = 
{
	INVALID_HANDLE_VALUE,
	"",
	"",
	NULL
};

//t_ir_sensor_state ir_NN_state;

/*
t_lookup lookup_table_ir_NN[] =
{
	{123,	0	},		//0.0 inches
	{113,	5	},		//0.5 inches
	{103,	10	},		//1.0 inches
	{95,	15	},
	{87,	20	},
	{81,	25	},		//2.5 inches
	{76,	30	},
	{71,	35	},
	{66,	40	},
	{62,	45	},
	{58,	50	},		//5.0 inches
	{56,	55	},
	{52,	60	},		//6.0 inches
	{47,	70	},		//7.0 inches
	{44,	80	},		//8.0 inches
	{41,	90	},		//9.0 inches
	{38,	100	},		//10.0 inches
	{32,	120	},		//12.0 inches
	{29,	140	},		//14.0 inches
	{22,	180 },
	{17,	240 },
	{15,    300 },
	{0,     400 },
	{0xffff,500 }
};
*/
t_lookup lookup_table_ir_NN[] =
{
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
	{130	,30  },		
	{0xffff,500 }
};


t_lookup lookup_table_ir_NW[] =
{
	{0		,300},
	{21		,260},  	
	{23		,240},  	
	{24		,220},  	
	{25		,180},		
	{27		,160},		
	{30		,140},		
	{34		,120},		
	{35		,110},		
	{38		,100},		//10.0 inches
	{41		,90	},		//9.0 inches
	{44		,80	},		//8.0 inches
	{48		,70	},		//7.0 inches
	{53		,60	},		//6.0 inches
	{56		,55	},
	{59		,50	},		//5.0 inches
	{62		,45	},
	{67		,40	},
	{70		,35	},
	{75		,30	},
	{79		,25	},		//2.5 inches
	{85		,20	},
	{93		,15	},
	{103	,10	},		//1.0 inches
	{113	,5	},		//0.5 inches
	{120	,0  },		//0.0 inches
	{0xffff ,0  }
};

t_lookup lookup_table_ir_SW[] =
{
	{0		,300},
	{21		,260},  		//8.0 inches
	{28		,150},  		//8.0 inches
	{29		,140},  		//8.0 inches
	{30		,130},  		//8.0 inches
	{31		,120},  		//8.0 inches
	{33		,110},  		//8.0 inches
	{36		,100},  		//8.0 inches
	{42		,80 },  		//8.0 inches
	{47		,70 },		//7.0 inches
	{51		,60	},		//6.0 inches
	{57		,50	},		//5.0 inches
	{65		,40	},
	{74		,30	},
	{86		,20	},
	{101	,15	},
	{104	,10	},		//1.0 inches
	{113	,5	},		//0.5 inches
	{125	,0	},		//0.0 inches
	{0xffff,500 }
};


t_lookup lookup_table_ir_NE[] =
{
	{0		,300},
	{19		,240 },  		
	{36		,120 },  		
	{48		,80 },  	//8.0 inches
	{51		,70 },		//7.0 inches
	{55		,60	},		//6.0 inches
	{58		,55	},
	{61		,50	},		//5.0 inches
	{64		,45	},
	{69		,40	},
	{74		,35	},
	{79		,30	},
	{84		,25	},		//2.5 inches
	{90		,20	},
	{98		,15	},
	{107	,10	},		//1.0 inches
	{117	,5	},		//0.5 inches
	{128	,0	},		//0.0 inches
	{0xffff,500 }
};


t_lookup lookup_table_ir_SE[] =
{
	{0		,300},
	{44		,80 },  		//8.0 inches
	{49		,70 },		//7.0 inches
	{52		,60	},		//6.0 inches
	{55		,55	},
	{58		,50	},		//5.0 inches
	{62		,45	},
	{65		,40	},
	{69		,35	},
	{73		,30	},
	{79		,25	},		//2.5 inches
	{84		,20	},
	{92		,15	},
	{99		,10	},		//1.0 inches
	{110	,5	},		//0.5 inches
	{118	,0	},		//0.0 inches
	{0xffff,500 }
};


t_lookup lookup_table_compass[] =
{
	{0		,0   },
	{490	,450 },
	{980	,900 },
	{1410	,1350},
	{1810	,1800},
	{2250	,2250},
	{2700	,2700},
	{3150	,3150},
	{3600	,3600},
	{0xffff ,3600}
};
/*
short lookup(t_lookup *table, unsigned short raw_value)
{
	int i=0;
	short interpolated_value;
	float d_raw, d_real;
	
	while(raw_value < table[i].raw_value) i++;
	
	if(i==0) return table[0].real_value;

	d_raw = (float)(raw_value-table[i].raw_value) / (float)(table[i-1].raw_value - table[i].raw_value);
	d_real= d_raw * (float)(table[i-1].real_value - table[i].real_value);

	interpolated_value = table[i].real_value + d_real;

	return interpolated_value;
}
*/

short lookup( t_lookup* c, unsigned short raw_value ) 
{ 
    int i=0; 
 
    while( c[i].raw_value != 0xffff )
    { 
		if ( c[i].raw_value == raw_value ) return  c[i].real_value;

        if ( c[i].raw_value < raw_value && c[i+1].raw_value > raw_value ) 
        { 
            float diffx = (float)(raw_value - c[i].raw_value); 
            float diffn = (float)(c[i+1].raw_value - c[i].raw_value); 
 
            return c[i].real_value + ( c[i+1].real_value - c[i].real_value ) * ( (float)(diffx / diffn) );  
        } 
		i++;
    } 
 
    return 0; // Not in Range 
} 


void ir_sensor_update(t_ir_sensor_state *sensor_state, unsigned char raw_value)
{
	int i,v;

	sensor_state->raw_value = raw_value;
	sensor_state->real_value= lookup(sensor_state->calibration_data,raw_value);

	sensor_state->raw_history[sensor_state->history_index] = sensor_state->raw_value;
	sensor_state->real_history[sensor_state->history_index] = sensor_state->real_value;

	sensor_state->history_index++;
	if(sensor_state->history_index >= sensor_state->history_depth) sensor_state->history_index=0;

	
	v=0;
	for(i=0;i<sensor_state->history_depth;i++)
	{
		v+=sensor_state->raw_history[i];
	}
	sensor_state->raw_history_avg = v / sensor_state->history_depth;
	
	//sensor_state->raw_history_avg = ((sensor_state->raw_history_avg * 7) + raw_value)/8;



	v=0;
	for(i=0;i<sensor_state->history_depth;i++)
	{
		v+=sensor_state->real_history[i];
	}
	sensor_state->real_history_avg = v / sensor_state->history_depth;
}

void lookup_test(void)
{
	int r;
	
	r = lookup(lookup_table_compass,    0);
	r = lookup(lookup_table_compass,  450);
	r = lookup(lookup_table_compass,  490);
	r = lookup(lookup_table_compass,  900);
	r = lookup(lookup_table_compass,  980);
	r = lookup(lookup_table_compass, 1350);
	r = lookup(lookup_table_compass, 1410);
	r = lookup(lookup_table_compass, 1800);
	r = lookup(lookup_table_compass, 1810);
	r = lookup(lookup_table_compass, 2700);
	r = lookup(lookup_table_compass, 3590);
	r = lookup(lookup_table_compass, 3600);


	r = lookup(lookup_table_ir_NW, 22);
	r = lookup(lookup_table_ir_NW, 23);
	r = lookup(lookup_table_ir_NW, 24);

	r = lookup(lookup_table_ir_NN, 104);
	r = lookup(lookup_table_ir_NN, 112);
	r = lookup(lookup_table_ir_NN, 108);
	r = lookup(lookup_table_ir_NN, 103);
}

/*
void ir_test(void)
{
	memset(&ir_NN_state,0,sizeof(t_ir_sensor_state));

	ir_NN_state.calibration_data = lookup_table_ir_NN;
	ir_NN_state.history_depth	= 2;

	ir_sensor_update(&ir_NN_state,103);
	ir_sensor_update(&ir_NN_state,103);
	ir_sensor_update(&ir_NN_state,103);
	ir_sensor_update(&ir_NN_state,112);
}
*/

void data_init(void)
{
	memset(&s.ir_NN_state,0,sizeof(t_ir_sensor_state));
	memset(&s.ir_NW_state,0,sizeof(t_ir_sensor_state));
	memset(&s.ir_SW_state,0,sizeof(t_ir_sensor_state));
	memset(&s.ir_NE_state,0,sizeof(t_ir_sensor_state));
	memset(&s.ir_SE_state,0,sizeof(t_ir_sensor_state));

	s.ir_NN_state.calibration_data = lookup_table_ir_NN;   s.ir_NN_state.history_depth	= 1;
	s.ir_NW_state.calibration_data = lookup_table_ir_NW;   s.ir_NW_state.history_depth	= 1;
	s.ir_SW_state.calibration_data = lookup_table_ir_SW;   s.ir_SW_state.history_depth	= 1;
	s.ir_NE_state.calibration_data = lookup_table_ir_NE;   s.ir_NE_state.history_depth	= 2;
	s.ir_SE_state.calibration_data = lookup_table_ir_SE;   s.ir_SE_state.history_depth	= 1;
}
