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

U08(4, 1, "sensor enable/disable bitmap",	0, 0, 0xff)
U32(4, 2, "echo timeout",					0, 0, 50)
U32(4, 3, "minimum delay between sensors",	0, 0, 50)
//----------------------------------------------------------



//----------------------------------------------------------
GRP(5, "ODOMETRY")

FLT(5, 1, "odo_cml",						0, 0, 0.1346671682494093856046448480279)
FLT(5, 2, "odo_cmr",						0, 0, 0.1346671682494093856046448480279)
FLT(5, 3, "odo_b",							0, 0, 155.5)
//----------------------------------------------------------



//----------------------------------------------------------
GRP(10, "WALL FOLLOWING")
U08(10, 1, "update interval",	0, 0, 25)
//100,40,100,65,20,55,35,2,4
S16(10, 2, "nominal speed"	,	0, 0, 80)
S16(10, 3, "target distance",	0, 0, 95)
S16(10, 4, "max error"		,	0, 0, 70)
S16(10, 5, "max correction %",	0, 0, 160)
S16(10, 6, "Kp"				,	0, 0, 55)
S16(10, 7, "Ki"				,	0, 0, 5)
S16(10, 8, "Kd"				,	0, 0, 60)
S16(10, 9, "minimum speed"	,	0, 0, 35)
S16(10,10, "speed up amount",	0, 0, 2)
S16(10,11, "slow down amount",	0, 0, 6)
U08(10,12, "use corner logic",	0, 0, 1)
FLT(10,13, "corner distance",	0, 0, 80)
S16(10,14, "corner speed",		0, 0, 65)
S16(10,15, "integral limit",	0, 0, 40)
S16(10,16, "lost wall distance",0, 0, 150)
S16(10,17, "found wall distance",0,0, 110)
S16(10,18, "corner radius",		0,0, 25)
S16(10,19, "sharp corner radius",0,0, 28)
//----------------------------------------------------------



GRP(254, "JUST FOR TESTING")

U08(254,1,"u08 test parameter - id 1"		, 0, 0, 1)
S08(254,2,"s08 test parameter - id 2"		, 0, 0, 2)
U16(254,3,"u16 test parameter - id 3"		, 0, 0, 3)
S16(254,4,"s16 test parameter - id 4"		, 0, 0, 4)
U32(254,5,"u32 test parameter - id 5"		, 0, 0, 5)
S32(254,6,"s32 test parameter - id 6"		, 0, 0, 6)
FLT(254,7,"flt test parameter - id 7"		, 0, 0, 7)