
#ifndef _sim_h_
#define _sim_h_

typedef struct
{
	//pololu library model
	u16 vbatt;
	s16 m1;
	s16 m2;
	s16 enc_ab;
	s16 enc_cd;
	SVPStatus svp_status;
	u32 elapsed_milliseconds;
	u08* rx_ring_buffer;
	u08 rx_ring_buffer_size;

	//world state
	float x,y,theta;
	s16 actual_enc_ab_ticks_per_interval;
	s16 actual_enc_cd_ticks_per_interval;

} t_robot_model;

extern void sim(void);

t_robot_model m;

extern t_robot_model m;

#ifndef WIN32
#define lcd_printf printf
#else
extern int	lcd_printf(const char *__fmt, ...);
#endif


#endif
