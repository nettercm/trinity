
#include <pololu/orangutan.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "typedefs.h"
#include "cocoos/cocoos.h"
#include "globals.h"

#include "hardware.h"
#include "i2c.h"
#include "pid.h"
#include "lookup.h"
#include "sharpir.h"
#include "serial.h"
#include "compass.h"
#include "sonar.h"

#include "wall_follow.h"
#include "motor.h"
#include "flame.h"

#include "../pc/commands.h"

#include "config.h"

#define LIMIT(var,min,max)					if((var)>(max)) (var)=(max);  if((var)<(min)) (var)=(min)

#define LIMIT2(var,min,max,flag)	\
if( (var)>(max) ) \
{ \
	(var)=(max); \
	flag=2; \
} \
else if( (var)<(min) ) \
{ \
	(var)=(min); \
	flag=1; \
} \
else flag=0;
	
#include "debug.h" //should always be the last one

