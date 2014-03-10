/*
 * lookup.h
 *
 * Created: 11/20/2012 11:20:57 PM
 *  Author: Chris
 */ 


#ifndef LOOKUP_H_
#define LOOKUP_H_

typedef struct
{
	s16 x;
	s16 y;
	s16 slope;
} t_LOOKUP_table;


extern void LOOKUP_initialize_table(t_LOOKUP_table *t);
extern s16 LOOKUP_do(s16 x, t_LOOKUP_table *t);
extern void LOOKUP_test(void);
extern void LOOKUP_init(void);

#endif /* LOOKUP_H_ */