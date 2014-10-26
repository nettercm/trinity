#include <windows.h>
#include <stdio.h>

#include "messages.h"
//#include "kalman.h"
//#include "logic.h"
#include "utils.h"
#include "debug.h"


/*

current distance from the wall
am i getting closer to the wall?
am i getting fruther away from the wall?


*/

//#define LOOPBACK



//////////////////////////////////////////////////////////////////////////////


int serial_write(HANDLE p,char *message,DWORD size)
{
	BOOL result;
	DWORD written=0;

	if(0)
	{
		int i;
		log_printf("%7lu: ",timeGetTime());
		for(i=0;i<size;i++)	log_printf("%02X,",(unsigned char)message[i]);
		log_printf(" \n");
	}

	result = WriteFile(p,message,size,&written,NULL);
	FlushFileBuffers(p);

	return (int)written;
}


//////////////////////////////////////////////////////////////////////////////


int serial_read(HANDLE h,char *message,DWORD size)
{
	BOOL result=TRUE;
	DWORD read=0,r=0,t1,t2,t3,t4;
	int count;
	DBG(DBG_SER_READ,("["));

	//wait for the first byte to arrive
	count = 0;
	t1=timeGetTime();
	while(result==TRUE && read==0)
	{
		if(count>400) break;
		t2=timeGetTime();
		result = ReadFile(h,message,1,&read,NULL);
		t3=timeGetTime();
		count++;
		//Sleep(1);
	}
	t4=timeGetTime();
	if(!result || !read) 
	{
		DBG(DBG_SER_READ,("]\n"));
		return 0;
	}

	DBG(DBG_SER_READ,("%2x ",(uint8) message[0]));
	//Sleep(5);
	count=0;
	while(read < size && result==TRUE /*&& count<10*/)
	{
		result = ReadFile(h,&(message[read]),1,&r,NULL);
		//printf("%c",r ? '.' : '*');
		if(r) DBG(DBG_SER_READ,("%2x ",(uint8) message[read])); else DBG(DBG_SER_READ,("*"));
		if(message[read-1]==(char)0xdc && message[read]==(char)0xba)
		{
			DBG(DBG_SER_READ,("x"));
			if(read>=sizeof(t_frame_to_pc)-1 && message[read-(sizeof(t_frame_to_pc)-1)]==(char)0xab && message[read-(sizeof(t_frame_to_pc)-2)]==(char)0xcd)
			{
				DBG(DBG_SER_READ,("y]\n"));
				return read+1;
			}
		}
		read+=r;
		count++;
		if(!r) Sleep(2);
	}
	DBG(DBG_SER_READ,("%d,%d]\n",read,result));
	return -((int)read);
}


#if 0
int read_n_bytes(HANDLE h, char *buffer, int n)
{
	long total=0;
	long read;
	int result;

	//read some bytes
	result = ReadFile(h,buffer,n,&read,NULL);
	total+=read;
	//give up if read fails
	if(!result) return total;

	//keep reading until we have read n bytes
	while(total<n)
	{
		Sleep(0);
		result = ReadFile(h,&(buffer[total]),n-total,&read,NULL);
		total+=read;
		//give up if read fails
		if(!result) return total;
	}
	return total;
}
#else


int read_n_bytes(HANDLE h, char *buffer, int n)
{
	static long total=0;
	long read;
	int result;

	if(total<n)
	{
		result = ReadFile(h,&(buffer[total]),n-total,&read,NULL);
		total+=read;
	}
	else
	{
		total=0;
		result = ReadFile(h,&(buffer[total]),n-total,&read,NULL);
		total+=read;
	}
	return total;
}
#endif


void skip_bytes_until(HANDLE h, unsigned char value)
{
	long read;
	int result;
	unsigned char buffer[1];

	//read some bytes
	result = ReadFile(h,buffer,1,&read,NULL);
	while(result)
	{
		if(read &&  buffer[0]==value) return;
		Sleep(0);
		result = ReadFile(h,buffer,1,&read,NULL);
	}
}

#define SIZE ((unsigned char)(0x80))


void serial_test_2(void)
{
	/*
	p=serial_init("\\\\.\\COM31",9600,TWOSTOPBITS);
	while(1)
	{
		t2=timeGetTime();
		td=t2-t1;
		t1=t2;
		buffer[0]=0x12;
		WriteFile(p,buffer,1,&result,NULL);
		buffer[0]=0x00;
		ReadFile(p,buffer,1,&result,NULL);
		printf("%4d  %3d\n",td, (unsigned char)buffer[0]);
		Sleep(15);
	}
	*/
}
void serial_test(HANDLE h)
{
	unsigned char i;
	long read,written;
	long total=0,last_total=0;
	unsigned char rx_buffer[256];
	unsigned char tx_buffer[256];
	int result;
	long t1,t2;
	int iterations=0;
	int avg=0,sum=0,min=2000,max=0;
	int n;

	for(i=0;i<=SIZE-1;i++) tx_buffer[i]=i;

	skip_bytes_until(h,SIZE-1);
	printf("found 0x%02x\n",SIZE-1);
	memset(rx_buffer,0,SIZE);
	WriteFile(h,tx_buffer,32,&written,NULL);

	t1 = timeGetTime();
	while(1)
	{
		n = read_n_bytes(h,rx_buffer,SIZE);
		if(n==SIZE)
		{
			t2=timeGetTime();
			WriteFile(h,tx_buffer,32,&written,NULL);
			for(i=0;i<=SIZE-1;i++)
			{
				//printf("%02x",buffer[i]);
				if(rx_buffer[i] != i)
				{
					printf("error @ %02x: %02x  ",i,rx_buffer[i]);
					for(i=0;i<=SIZE-1;i++) printf("%02x",rx_buffer[i]);
					printf("\n");
					skip_bytes_until(h,SIZE-1);
					printf("found 0x7f\n");
					i=SIZE;
				}
			}
			//printf("\n");
			iterations++;
			sum=sum+(t2-t1);
			avg=sum/iterations;
			min=(t2-t1)<min ? (t2-t1) : min;
			max=(t2-t1)>max ? (t2-t1) : max;
			printf("%4d now=%4d min=%4d avg=%4d max=%4d\n",iterations, t2-t1, min,avg,max);
			t1=t2;
			memset(rx_buffer,0,SIZE);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////


int serial_validate(char *buffer, int size)
{
	int result=0;

	if(size==sizeof(t_inputs)) result = 1;

	return result;
}


//////////////////////////////////////////////////////////////////////////////


HANDLE serial_init(char* port, int baud, int stopbits)
{
	DWORD error;
	HANDLE hSerial;
	DCB dcbSerialParams = {0};
	COMMTIMEOUTS timeouts={0};

	DBG(DBG_ALWAYS,("serial_init(%s)\n",port));

	hSerial = CreateFile(port,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		//FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING,
		0);

	if(hSerial==INVALID_HANDLE_VALUE)
	{
		error = GetLastError();
		if(error==ERROR_FILE_NOT_FOUND)
		{
			//serial port does not exist. Inform user.

		}
		//some other error occurred. Inform user.
		return INVALID_HANDLE_VALUE;
	}

	//dcbSerial.DCBlength=sizeof(dcbSerialParams);

	if (!GetCommState(hSerial, &dcbSerialParams)) 
	{
		//error getting state
	}
	dcbSerialParams.BaudRate=baud;
	dcbSerialParams.ByteSize=8;
	dcbSerialParams.StopBits=stopbits; //ONESTOPBIT;
	dcbSerialParams.Parity=NOPARITY;
	if(!SetCommState(hSerial, &dcbSerialParams))
	{
		//error setting serial port state
	}

#if 1
	timeouts.ReadIntervalTimeout		=	MAXDWORD; //1;
	timeouts.ReadTotalTimeoutConstant	=	1; //10;
	timeouts.ReadTotalTimeoutMultiplier	=	MAXDWORD; //0;
#else
	timeouts.ReadIntervalTimeout		=	1;
	timeouts.ReadTotalTimeoutConstant	=	1;
	timeouts.ReadTotalTimeoutMultiplier	=	0;
#endif
	timeouts.WriteTotalTimeoutConstant	=	0;
	timeouts.WriteTotalTimeoutMultiplier=	0;

	if(!SetCommTimeouts(hSerial, &timeouts))
	{
		perror("SetCommTimeouts");
		//error occureed. Inform user
	}

	PurgeComm(hSerial,0x000f);
	ClearCommError(hSerial,NULL,NULL);

	return hSerial;
}


HANDLE serial_open(char *port)
{
	HANDLE p=INVALID_HANDLE_VALUE;

	DBG(DBG_ALWAYS,("serial_open(%s)\n",port));

	while(p==INVALID_HANDLE_VALUE)
	{
		//p=serial_init("\\\\.\\COM52",115200,ONESTOPBIT); //BT2 is COM26;  BT1 on small USB transceiver is COM27; BT1 on new USB is COM28
		p=serial_init(port,115200,ONESTOPBIT); //BT2 is COM26;  BT1 on small USB transceiver is COM27; BT1 on new USB is COM28
		PurgeComm(p,0x000f);
		ClearCommError(p,NULL,NULL);
		if(p==INVALID_HANDLE_VALUE)
		{
			log_printf("can't open port '%s'\n",port);
			Sleep(1000);
		}
	}
	return p;
}

HANDLE serial_reopen(HANDLE p, char *port)
{
	static int error_count=0;

	DBG(DBG_ALWAYS,("serial_reopen(%s)\n",port));

	//if(port[0] == 0) return INVALID_HANDLE_VALUE;

	//purge and clear error
	PurgeComm(p,0x000f);
	ClearCommError(p,NULL,NULL);
	//outputs.flags=0x0001;
	error_count++;
	if(0) //error_count >=3)
	{
		CloseHandle(p);
		Sleep(2000);
		p=serial_init(port, 115200,ONESTOPBIT); //BT2 is COM26;  BT1 on small USB transceiver is COM27; BT1 on new USB is COM28
		if(p==INVALID_HANDLE_VALUE)
		{
			log_printf("can't open port\n");
		}
		else
		{
			error_count=0;
		}
	}
	return p;
}
