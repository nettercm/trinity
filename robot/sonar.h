/*
 * IncFile1.h
 *
 * Created: 2/13/2013 9:28:57 PM
 *  Author: Chris
 */ 


#ifndef SONAR_H_
#define SONAR_H_

extern int ultrasonic_test();
extern void set_ultrasonic_mux(uint8 addr);
extern void ultrasonic_update_fsm(uint8 cmd, uint8 *param);

extern void ultrasonic_set_focus(unsigned char sensor_id);
extern void ultrasonic_set_sequence(unsigned char *new_sequence);
extern void ultrasonic_set_timeout(unsigned char new_timeout);

extern void ultrasonic_hardware_init(void);


extern volatile uint8 us_sequence[];
extern uint8 us_sequence_uniform[];
extern uint8 us_sequence_W_priority[];
extern uint8 us_sequence_E_SE_NE[];
extern uint8 us_sequence_N_priority[];

//10ms == 3.3m max range
extern volatile uint8 us_timeout;
#define US_TIMEOUT us_timeout

/*
0 = NE
1 = N
2 = NW
3 = E
4 = W
5 = SE
6 = SW
*/
#define US_NE 2
#define US_N  0
#define US_NW 1
#define US_E  3
#define US_W  4
#define US_SE 5
#define US_SW 6


#endif /* INCFILE1_H_ */