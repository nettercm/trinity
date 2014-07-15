
#ifndef _ip_h_
#define _ip_h_


#ifdef __cplusplus 
extern "C" {
#endif


extern int tcp_recv(char *buffer, int size,unsigned long nonblocking);
extern int tcp_send(char *buffer, int size,unsigned long nonblocking);
extern int tcp_server_init(char *ip_address, unsigned short port);
extern int tcp_server_accept(unsigned long nonblocking);
extern int tcp_client_init(char *server_name, int port);
extern void tcp_client_close(void);


#ifdef __cplusplus 
}
#endif


#endif
