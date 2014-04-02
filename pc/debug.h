
#ifndef _debug_h_
#define _debug_h_

#include <varargs.h>
#include <stdarg.h>

#ifdef __cplusplus 
extern "C" {
#endif

#define LOG_BUFFER_SIZE  100000

extern int	log_printf(const char *__fmt, ...);

#define DBG_ALWAYS		0x80000000UL
#define DBG_MAIN		0x00000002UL
#define DBG_SER_READ	0x00000001UL

extern volatile unsigned long debug_functions;
extern void show_last_error(char *s) ;

#ifdef WIN32UI

#define DBG(function, params) if(debug_functions & function)  log_printf params

#else

#define DBG(function, params) if(debug_functions & function)  printf params

#endif




#ifdef __cplusplus 
}
#endif


#endif
