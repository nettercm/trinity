#ifndef _flame_h_
#define _flame_h_


extern void uvtron_update(void);
extern uint8 find_peak(uint8 *history, uint8 size);
extern uint8 hone_in_on_candle(uint8 cmd, uint8 range);
extern uint8 find_flame_fsm(uint8 cmd);


#endif