#ifndef START_SIGNAL_
#define START_SIGNAL_


#include "typedefs.h"

extern u16 sound_start_count;
extern u16 consecutive_sound_start_count;
extern u16 button_count;
extern u32 frequency_in_hz;

extern int check_for_start_signal();
extern void force_start_signal();
#endif
