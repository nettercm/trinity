
#ifndef SERVO_H_
#define SERVO_H_

#include "typedefs.h"
#include "cocoos/cocoos.h"

extern void servo_task(u08 cmd, u08 *param);

extern void pan_relative(s16 delta_x);
extern void pan_absolute(u16 x);
extern void tilt_relative(s16 delta_y);
extern void tilt_absolute(u16 y);
extern void pan_tilt_center(void);
extern void tilt_center(void);


#endif

