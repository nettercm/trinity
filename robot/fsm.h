
#ifndef _fsm_h_
#define _fsm_h_

extern void fsm_test_task(u08 cmd, u08 *param);
extern void fsm_test_2(void);


#define STATE_HAS_CHANGED ( last_state != state ? (t_entry=get_ms()),((last_state=state)+1) : 0) /*need to make sure this always returns >0 even if new state is '0'*/
#define ENTER(s) if(STATE_HAS_CHANGED)
#define EXIT(s) _label ## s: if(state!=last_state)
#define FIRST_STATE(s) if(state==s)
#define NEXT_STATE(s) else if(state==s)
#define SWITCH_STATE(cs,ns) state=ns;goto _label ## cs;
#define LEAVE_STATE(cs) goto _label ## cs;

#define enter_(s) if( last_state != state ? (t_entry=get_ms()),((last_state=state)+1) : 0)
#define exit_(s) _label ## s: if(state!=last_state)
#define first_(s) if(state==s)
#define next_(s) else if(state==s)
#define switch_(cs,ns) state=ns;goto _label ## cs;
#define leave_(cs) goto _label ## cs;
#define time_since_entry_()  (get_ms()-t_entry)

volatile char _dummy_;
#define NOP() _dummy_++;


#define FSM_INIT_EVENT 0xffffffffUL
#define FSM_NULL_EVENT 0x00000000UL

#endif
