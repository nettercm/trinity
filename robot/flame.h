#ifndef _flame_h_
#define _flame_h_

typedef struct
{
	u16 position;
	u08 value;
} t_peak;


typedef struct
{
	s16 angle;
	u16 ir_north;
	u08 flame;
} t_scan;

typedef struct
{
	u08 flame_center_value;
	u16 rising_edge_position;
	s16 rising_edge_angle;
	u16 falling_edge_position;
	s16 falling_edge_angle;
	u16 center_position;
	s16 center_angle;
} t_scan_result;

extern void uvtron_update(void);
extern t_peak find_peak(uint8 *data, uint16 size, uint8 threashold);
extern t_scan_result find_peak_in_scan(t_scan *data, u16 number_of_points, uint8 threashold);
extern uint8 hone_in_on_candle(uint8 cmd, uint8 range);
extern  uint8 find_flame_fsm(uint8 cmd);

extern void test_flame(void);

#endif