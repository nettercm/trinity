//----------------------------------------------------------
GRP(1, "SYSTEM CONFIG")

U08(1, 1, "serial tx interval"		,		0, 0, 20)
U08(1, 2, "lcd update rate"			,		0, 0, 100)
U08(1, 3, "V(Batt) low-pass filter window", 0, 0, 32)
U08(1, 4, "A/D input update interval",		0, 0, 5)
//----------------------------------------------------------

//----------------------------------------------------------
GRP(2, "IR RANGE")

S16(2, 1, "short-range lp filter thresh.",			0, 0, 240)
S16(2, 2, "short-range lp filter amount",			0, 0, 2)
S16(2, 3, "long-range lp filter threshold",			0, 0, 480)
S16(2, 4, "long-range lp filter amount",			0, 0, 2)


//----------------------------------------------------------
GRP(3, "MOTOR CONTROL")

U08(3, 1, "update interval",				0, 0, 20)
S16(3, 2, "ticks/interval to PWM",			0, 0, 1.75*16)
S16(3, 3, "Kp",								0, 0, 1.00*16)
S16(3, 4, "Ki",								0, 0, 0.10*16)
//----------------------------------------------------------


//----------------------------------------------------------
GRP(4, "ULTRASONIC RANGE")

U08(4, 1, "sensor enable/disable bitmap",	0, 0, 0x01)
U32(4, 2, "echo timeout",					0, 0, 50)
U32(4, 3, "minimum delay between sensors",	0, 0, 50)
//----------------------------------------------------------



//----------------------------------------------------------
GRP(5, "ODOMETRY")

FLT(5, 1, "odo_cml",						0, 0, 0.1346671682494093856046448480279)
FLT(5, 2, "odo_cmr",						0, 0, 0.1346671682494093856046448480279)
FLT(5, 3, "odo_b",							0, 0, 156.5)
//----------------------------------------------------------



//----------------------------------------------------------
GRP(6, "LINE DETECTION")

U08(6, 1, "black",							0, 0, 51)//12)//70)
U08(6, 2, "white",							0, 0, 50)//11)//50)
//----------------------------------------------------------


//----------------------------------------------------------
GRP(9, "MASTER LOGIC")

S16(9,10, "turn speed",						0, 0, 50)
S16(9,11, "motor command",					0, 0, 7)		//which motor command to use for certain mannouvers
S16(9,12, "acceleration",					0, 0, 1)		//
S16(9,13, "deceleration",					0, 0, 1)
S16(9,14, "flame_scan_edge_threashold ",	0, 0, 30)
S16(9,15, "flame_found_threashold ",		0, 0, 180)
U16(9,16, "flame scan filter ",				0, 0, 4)

S16(9,20, "search Rm #3: enter distance",	0, 0, 180)		//distance is in mm here
S16(9,21, "search Rm #3: turn #1",			0, 0,-120)
S16(9,22, "search Rm #3: turn #2",			0, 0, 210)		//basically we'll face west after this turn; turn another 90 left to face the door
S16(9,23, "search Rm #3: turn #3",			0, 0, 90)		//basically we'll face sount (the door)

S16(9,30, "search Rm #2: enter distance",	0, 0, 180)
S16(9,31, "search Rm #2: turn #1",			0, 0,-120)
S16(9,32, "search Rm #2: turn #2",			0, 0, 210)		//we'll face south at the end - need to turn another 90deg left to face the door
S16(9,33, "search Rm #2: turn #3",			0, 0, 90)		//we'll face south at the end - need to turn another 90deg left to face the door

S16(9,40, "search Rm #1: etner distance",	0, 0, 180)
S16(9,41, "search Rm #1: turn #1",			0, 0,-100)
S16(9,42, "search Rm #1: turn #2",			0, 0, 220)
S16(9,43, "search Rm #1: turn #3",			0, 0,-180)

S16(9,51, "find Rm #4:   dog scan distance",0, 0, 280)
S16(9,52, "find Rm #4:   dog scan sensor",  0, 0, 1)
S16(9,53, "find Rm #4:   dog scan angle ",  0, 0, 20)
U16(9,54, "find Rm #4:   dog scan m. avg.", 0, 0, 2)

S16(9,60, "find Rm #4:   distance #1",		0, 0, 260)		//how far to move into the intersection after exitinig from Rm #1
S16(9,61, "find Rm #4:   turn #1",			0, 0, 15)		//how far to turn left before starting to follow the left wall 
S16(9,62, "find Rm #4:   distance #2",		0, 0, 240)		//how far to move into the hallway before starting to follow the left wall
S16(9,63, "find Rm #4:   left margin 1",	0, 0, 110)		//margin
S16(9,64, "find Rm #4:   riggt margin 1",	0, 0, 150)		//margin while moving into the hallway
S16(9,65, "find Rm #4:   distance #3",		0, 0, 800)		//how far to move into the hallway before starting to follow the left wall
S16(9,66, "find Rm #4:   left margin 2",	0, 0, 135)
S16(9,67, "find Rm #4:   riggt margin 2",	0, 0, 135)

S16(9,71, "search Rm #4: distance #1",		0, 0, 100)
S16(9,72, "search Rm #4: turn #1",			0, 0,-100)
S16(9,73, "search Rm #4: turn #2",			0, 0, 200)


//----------------------------------------------------------


//----------------------------------------------------------
GRP(10, "WALL FOLLOWING")

U08(10, 1, "update interval",	0, 0, 25)
//100,40,100,65,20,55,35,2,4
S16(10, 2, "nominal speed"	,	0, 0, 120)
S16(10, 3, "target distance",	0, 0, 120)
S16(10, 4, "max error"		,	0, 0, 80)
S16(10, 5, "max correction %",	0, 0, 140)
S16(10, 6, "Kp"				,	0, 0, 60)
S16(10, 7, "Ki"				,	0, 0, 5)
S16(10, 8, "Kd"				,	0, 0, 80)
S16(10, 9, "minimum speed"	,	0, 0, 30)
S16(10,10, "speed up amount",	0, 0, 5)
S16(10,11, "slow down amount",	0, 0, 5)
U08(10,12, "use corner logic",	0, 0, 1)
FLT(10,13, "corner distance",	0, 0, 110)
S16(10,14, "corner speed",		0, 0, 80)
S16(10,15, "integral limit",	0, 0, 40)
S16(10,16, "lost wall distance",0, 0, 150)
S16(10,17, "found wall distance",0,0, 110)
S16(10,18, "corner radius",		 0,0, 24)
S16(10,19, "sharp corner radius",0,0, 24)
//----------------------------------------------------------


//----------------------------------------------------------
GRP(99, "TEST TASK")

S16(99, 1, "accelleration", 0, 0, 1)
S16(99, 2, "decelleration", 0, 0, 1)
S16(99, 3, "speed",			0, 0, 50)
S16(99, 4, "distance",		0, 0, 200)
S16(99, 5, "angle",			0, 0, 90)
//----------------------------------------------------------

/*
//----------------------------------------------------------
GRP(254, "JUST FOR TESTING")

U08(254,1,"u08 test parameter - id 1"		, 0, 0, 1)
S08(254,2,"s08 test parameter - id 2"		, 0, 0, 2)
U16(254,3,"u16 test parameter - id 3"		, 0, 0, 3)
S16(254,4,"s16 test parameter - id 4"		, 0, 0, 4)
U32(254,5,"u32 test parameter - id 5"		, 0, 0, 5)
S32(254,6,"s32 test parameter - id 6"		, 0, 0, 6)
FLT(254,7,"flt test parameter - id 7"		, 0, 0, 7)
*/
