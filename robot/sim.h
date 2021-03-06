
#ifndef _sim_h_
#define _sim_h_

typedef struct
{
	//pololu library model
	u16 vbatt;
	s16 m1;
	s16 m2;
	u16 servo[8];
	double enc_ab;
	double enc_cd;
	SVPStatus svp_status;
	u32 elapsed_milliseconds;
	u08* rx_ring_buffer;
	u08 rx_ring_buffer_size;
	u08 start_signal;

	//world state
	float x,y,theta;
	s16 actual_enc_ab_ticks_per_interval;
	s16 actual_enc_cd_ticks_per_interval;

	//some information about the arena state
	u08 candle_location; //don't have a simulated omni-directional flame sensor....
	u08 start_location; //allows us to "skip" ahead to a state where the robot just entered a specific room;  0=home
} t_robot_model;

extern void sim_task(u08 cmd, u08 *param);

t_robot_model m;

extern t_robot_model m;

#ifndef WIN32
#define lcd_printf printf
#else
extern int	lcd_printf(const char *__fmt, ...);
#define PROGMEM
#endif


#endif
