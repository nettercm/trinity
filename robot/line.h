#ifndef LINE_H_
#define LINE_H_

#include "typedefs.h"
#include "cocoos/cocoos.h"

extern u08 lines_crossed;
extern u08 last_lines_crossed;
extern Evt_t line_alignment_start_evt;
extern Evt_t line_alignment_done_evt;

extern void line_detection_fsm_v2(u08 cmd, u08 *param);
extern void line_alignment_fsm(u08 cmd, u08 *param);
extern u08 line_alignment_fsm_v2(u08 cmd, u08 *param);

#endif
