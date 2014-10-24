
#ifndef MOTION_H_
#define MOTION_H_


#include "typedefs.h"

#define TURN_IN_PLACE(speed,angle) turn_in_place_manneuver(1,(speed),(angle)); while(turn_in_place_manneuver(0,(speed),(angle))) {OS_SCHEDULE;}
#define TURN_IN_PLACE_AND_SCAN(speed,angle,filter) turn_in_place_manneuver(1,(speed),(angle)); scan(1,filter); while(turn_in_place_manneuver(0,(speed),(angle))) {scan(0,filter); OS_SCHEDULE;}

#define MOVE(speed,distance)		move_manneuver(1,(speed),(distance)); while(move_manneuver(0,(speed),(distance))) {OS_SCHEDULE;}
#define MOVE2(speed,distance,sl,sr)	move_manneuver2(1,(speed),(distance),(sl),(sr)); while(move_manneuver2(0,(speed),(distance),(sl),(sr))) {OS_SCHEDULE;}
#define GO(speed)					motor_command(7,2,2,10,10); motor_command(6,1,1,(speed),(speed))

#define HARD_STOP()					motor_command(2,0,0,0,0);OS_SCHEDULE;
#define SOFT_STOP()					motor_command(6,5,5,0,0);OS_SCHEDULE;

#define FAN_ON()					set_digital_output(FAN_PIN,0)
#define FAN_OFF()					set_digital_output(FAN_PIN,1)

#define RESET_LINE_DETECTION()		last_lines_crossed = lines_crossed;
#define WAIT_FOR_LINE_DETECTION()	while(lines_crossed == last_lines_crossed) {OS_SCHEDULE;}
#define LINE_WAS_DETECTED()			(lines_crossed != last_lines_crossed)


extern u08 move_manneuver(u08 cmd, s16 speed, float distance);
extern u08 move_manneuver2(u08 cmd, s16 speed, float distance, s16 safe_left, s16 safe_right);
extern u08 turn_in_place_manneuver(u08 cmd, s16 speed, float angle);
extern t_scan_result find_path_in_scan(t_scan *data, u16 number_of_points, u16 threashold, u16 hysteresis, u08 use_far_north);

#endif
