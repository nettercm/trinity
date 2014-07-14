
#ifndef _ip_h_
#define _ip_h_


#ifdef __cplusplus 
extern "C" {
#endif


int tcp_recv(char *buffer, int size,unsigned long nonblocking);
int tcp_send(char *buffer, int size,unsigned long nonblocking);
int tcp_server_init(char *ip_address, unsigned short port);
int tcp_server_accept(unsigned long nonblocking);
int tcp_client_init(char *server_name, int port);


#ifdef __cplusplus 
}
#endif


#endif
