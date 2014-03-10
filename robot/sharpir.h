/*
 * sharpir.h
 *
 * Created: 11/21/2012 12:09:58 AM
 *  Author: Chris
 */ 


#ifndef SHARPIR_H_
#define SHARPIR_H_


extern t_LOOKUP_table SHARPIR_nn_table[];
extern t_LOOKUP_table SHARPIR_nw_table[];
extern t_LOOKUP_table SHARPIR_ne_table[];
extern t_LOOKUP_table SHARPIR_sw_table[];
extern t_LOOKUP_table SHARPIR_se_table[];
extern void SHARPIR_init(void);
extern void SHARPIR_test(void);
extern sint16 SHARPIR_get_real_value(uint8 sensor, sint16 raw_value);


#endif /* SHARPIR_H_ */