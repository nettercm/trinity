
#ifndef _fsm_h_
#define _fsm_h_

extern void fsm_test_task(void);


#define STATE_HAS_CHANGED ( last_state != state ? (last_state=state) : 0)
#define ENTER(s) if(STATE_HAS_CHANGED)
#define enter_(s) ENTER(s)
#define EXIT(s) _label ## s: if(state!=last_state)
#define exit_(s) EXIT(s)
#define FIRST_STATE(s) if(state==s)
#define first_(s) FIRST_STATE(s)
#define NEXT_STATE(s) else if(state==s)
#define next_(s) NEXT_STATE(s)
#define SWITCH_STATE(cs,ns) state=ns;goto _label ## cs;
#define switch_(cs,ns) SWITCH_STATE(cs,ns)
#define LEAVE_STATE(cs) goto _label ## cs;
#define leave_(cs) LEAVE_STATE(cs)

volatile char _dummy_;
#define NOP() _dummy_++;


#define FSM_INIT_EVENT 0xffffffffUL
#define FSM_NULL_EVENT 0x00000000UL

#endif
