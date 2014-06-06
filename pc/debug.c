
#include <Windows.h>
#include <stdio.h>
#include "debug.h"

extern int	log_printf(const char *__fmt, ...);

volatile unsigned long debug_functions = DBG_ALWAYS | DBG_MAIN; //  | DBG_SER_READ;


void dbg(unsigned long function, char *formatstring, ...) 
{
   char buff[255];
   va_list args;
   va_start(args, formatstring);

   sprintf( buff, formatstring, args);
   if(debug_functions & function) log_printf("%s",buff);
}


void show_last_error(char *s) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

	log_printf("%s: %s\n",s,lpMsgBuf);

    LocalFree(lpMsgBuf);
}
