
#ifndef _serial_h_
#define _serial_h_

#include <windows.h>
#include <stdio.h>

#ifdef __cplusplus 
extern "C" {
#endif

extern int serial_write(HANDLE p,char *message,DWORD size);
extern int serial_read(HANDLE h,char *message,DWORD size);
extern int serial_validate(char *buffer, int size);
extern HANDLE serial_init(char* port, int baud, int stopbits);
extern HANDLE serial_open(char *port);
extern HANDLE serial_reopen(HANDLE p, char *port);
extern void serial_test(HANDLE h);

#ifdef __cplusplus 
}
#endif

#endif