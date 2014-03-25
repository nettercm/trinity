
#include "typedefs.h"


#define FOLLOW_WALL		1
#define MASTER_LOGIC	3 
#define TEST_LOGIC		11

#define START_BEHAVIOR(b,signal)	s.behavior_state[b]=signal
#define STOP_BEHAVIOR(b)			s.behavior_state[b]=0


//the following is not yet used
typedef struct 
{
	//u08 priority;
	s16 speed;
	u08 speed_flag; //1 if the behavior wants to control speed
	s16 heading;
	u08 heading_flag; //1 if the behavior wants to control heading
	u16 persistance;  //>0 means the behavior is executing a ballistic maneuver and needs n cycles or milliseconds
} t_behavior_output;

/*
behavior input:
* enable/disable flag
* state
* sensors
* indication if this behavior won arbitration during the previous update cycle

behavior output:
* target speed
* heading
* duration (for ballistic behaviors / actions)
* future:  sensor focus (e.g. priority for sonar sensors;  which way to pan the servo-mounted sensors to)
* future:  sound

behaviors can leave speed or heading "blank"
the highest priority behavior w/ speed output controls speed

arbitration output:
* speed
* heading
* duration
* speed winner
* heading winner

arbitration:
for priority = 1 (lowest) to #behaviors / highest
	IF behavior_output[priority].speed_flag == 1  THEN  
		arbitration_output.speed = behavior_output[priority].speed;
		arbitration_output.speed_winner = priority;
	ENDIF
	IF behavior_output[priority].heading_flag == 1  THEN
		arbitration_output.heading = behavior_output[priority].heading;
		arbitration_output.heading_winner = priority;
	ENDIF
	

*/
