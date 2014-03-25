
#ifndef _wall_follow_h
#define _wall_follow_h

#define LEFT_WALL  1
#define RIGHT_WALL 2

extern void wall_follow_fsm(u08 cmd, u08 *param);

/*
extern uint8 follow_left_wall_using_ir(s16 min_distance, s16 max_distance, s16 correction);
extern uint8 follow_right_wall_using_ir(s16 min_distance, s16 max_distance, s16 correction);
extern uint8 follow_left_wall_and_turn(uint8 cmd);
extern uint8 follow_right_wall_and_turn(uint8 cmd);
extern uint8 follow_left_wall_using_us(uint16 min_distance, uint16 max_distance, sint16 correction);
*/


#endif