/*
 * serial.h
 *
 * Created: 11/22/2012 5:39:38 PM
 *  Author: Chris
 */ 


#ifndef SERIAL_H_
#define SERIAL_H_

#define RX_BUFFER_SIZE 128
#define SERIAL_SEND_INTERVAL 20

//only applies to SVP-1284
#define UART_PC UART1

extern uint8 rx_buffer[];

//extern Evt_t serial_cmd_evt;

extern void serial_receive_fsm(u08 cmd, u08 *param);
//extern void commands_receive_fsm(void);
extern void serial_send_fsm(u08 cmd, u08 *param);
extern void serial_hardware_init(void);
extern void serial_test(void);

#endif /* SERIAL_H_ */