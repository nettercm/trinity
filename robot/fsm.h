
#ifndef _fsm_h_
#define _fsm_h_

extern void fsm_test_task(void);


#define STATE_HAS_CHANGED ( last_state != state ? ((last_state=state)+1) : 0) /*need to make sure this always returns >0 even if new state is '0'*/
#define ENTER(s) if(STATE_HAS_CHANGED)
#define EXIT(s) _label ## s: if(state!=last_state)
#define FIRST_STATE(s) if(state==s)
#define NEXT_STATE(s) else if(state==s)
#define SWITCH_STATE(cs,ns) state=ns;goto _label ## cs;
#define LEAVE_STATE(cs) goto _label ## cs;

#define enter_(s) ENTER(s)
#define exit_(s) EXIT(s)
#define first_(s) FIRST_STATE(s)
#define next_(s) NEXT_STATE(s)
#define switch_(cs,ns) SWITCH_STATE(cs,ns)
#define leave_(cs) LEAVE_STATE(cs)

volatile char _dummy_;
#define NOP() _dummy_++;


#define FSM_INIT_EVENT 0xffffffffUL
#define FSM_NULL_EVENT 0x00000000UL

#endif
