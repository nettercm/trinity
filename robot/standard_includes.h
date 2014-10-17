
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
#include "motor.h"
#include "motion.h"
#include "wall_follow.h"
#include "flame.h"
#include "line.h"
#include "servo.h"
#include "start_signal.h"
#include "../pc/commands.h"
#include "config.h"
#include "fsm.h"
#include "behaviors.h"
#include "testing.h"

#include "debug.h" //should always be the last one
#include "sim.h"   //well, actually this needs to be the last one in case we need to override some functions when running in simulation mode
