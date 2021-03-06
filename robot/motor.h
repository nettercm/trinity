
#ifndef _motor_h_
#define _motor_h_

extern void encoders_reset(void);
extern void encoders_set_checkpoint();
extern sint16 encoders_get_distance_since_checkpoint();
extern uint8 encoders_update_fsm(uint32 event);

extern int motor_command(unsigned char cmd, uint16 p1, uint16 p2, sint16 lm_speed, sint16 rm_speed);
extern void odometry_update_fsm(u08 cmd, u08 *param);
extern void motors_stop(void);
extern void motors_set(sint16 lm_speed, sint16 rm_speed);
extern void motors_reapply_target_speed();
extern void motors_hardware_init(void);

extern void odometry_update(s16 l_ticks, s16 r_ticks, float odo_cml, float odo_cmr, float odo_b);
extern void odometry_set_checkpoint(void);
extern float odometry_get_rotation_since_checkpoint(void);
extern float odometry_get_distance_since_checkpoint(void);

extern void odometry_update_postion(float x, float y, float t);

//the following is in units of encoder ticks per time period;  should not be higher than what can be attained
#define MAX_SPEED 150 

#endif