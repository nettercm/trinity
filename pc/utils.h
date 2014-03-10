
#ifndef _utils_h_
#define _utils_h_

#ifdef __cplusplus 
extern "C" {
#endif


#define FLAG_NONE		0x00000000UL
#define FLAG_EXIT		0x00000001UL

extern volatile unsigned long g_flags;


void flag_set(unsigned long flag);
void flag_clear(unsigned long flag);
int flag_get(unsigned long flag, int clear);

#ifdef __cplusplus 
}
#endif


#endif