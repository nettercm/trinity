/* Server program example for IPv4 */
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT 2007
// default TCP socket type
#define DEFAULT_PROTO SOCK_STREAM

void Usage(char *progname)
{
	fprintf(stderr,"Usage: %s -p [protocol] -e [port_num] -i [ip_address]\n", progname);
	fprintf(stderr,"Where:\n\t- protocol is one of TCP or UDP\n");
	fprintf(stderr,"\t- port_num is the port to listen on\n");
	fprintf(stderr,"\t- ip_address is the ip address (in dotted\n");
	fprintf(stderr,"\t  decimal notation) to bind to. But it is not useful here...\n");
	fprintf(stderr,"\t- Hit Ctrl-C to terminate server program...\n");
	fprintf(stderr,"\t- The defaults are TCP, 2007 and INADDR_ANY.\n");
	WSACleanup();
	exit(1);
}

int socket_type = DEFAULT_PROTO;
struct sockaddr_in local, from;
struct sockaddr_in server;
WSADATA wsaData;
SOCKET listen_socket=INVALID_SOCKET, message_socket=INVALID_SOCKET;



int tcp_recv(char *buffer, int size, unsigned long nonblocking)
{
	int retval;

	if (message_socket == INVALID_SOCKET) return -1;
	retval = ioctlsocket(message_socket, FIONBIO, &nonblocking);

	// In the case of SOCK_STREAM, the server can do recv() and send() on
	// the accepted socket and then close it.
	// However, for SOCK_DGRAM (UDP), the server will do recvfrom() and sendto()  in a loop.
	retval = recv(message_socket, buffer, size, 0);

	if (retval == SOCKET_ERROR)
	{
		if(!nonblocking)
		{
			log_printf("tcp_recv(): recv() failed: error %d\n", WSAGetLastError());
			closesocket(message_socket);
			message_socket = INVALID_SOCKET;
			return -1;
		}
	}
	//else log_printf("TCP Server: recv() is OK.\n");

	if (retval == 0)
	{
		if(!nonblocking)
		{
			log_printf("tcp_recv(): Client closed connection.\n");
			closesocket(message_socket);
			message_socket = INVALID_SOCKET;
			return -1;
		}
	}

	if(retval>0) buffer[retval] = 0;
	//log_printf("tcp_recv(): Received %d bytes\n", retval);

	return retval;
}



int tcp_server_accept(unsigned long nonblocking)
{
	int retval;
	int fromlen;

	fromlen =sizeof(from);

	retval = ioctlsocket(listen_socket, FIONBIO, &nonblocking);

	message_socket = accept(listen_socket, (struct sockaddr*)&from, &fromlen);
	if (message_socket == INVALID_SOCKET)
	{
		if(!nonblocking) log_printf("TCP Server: accept() error %d\n", WSAGetLastError());
		//WSACleanup();
		return -1;
	}
	else log_printf("TCP Server: accept() is OK.\n");
	log_printf("TCP Server: accepted connection from %s, port %d\n", inet_ntoa(from.sin_addr), htons(from.sin_port)) ;
	return 1;
}



int tcp_send(char *buffer, int size, unsigned long nonblocking)
{
	int retval;

	if (message_socket == INVALID_SOCKET) return -1;

	retval = ioctlsocket(message_socket, FIONBIO, &nonblocking);

	retval = send(message_socket, buffer, size /*sizeof(Buffer)*/, 0);

	if (retval == SOCKET_ERROR)
	{
		log_printf("TCP Server: send() failed: error %d\n", WSAGetLastError());
		closesocket(message_socket);
		message_socket = INVALID_SOCKET;
	}
	//else log_printf("TCP Server: send() is OK.\n");
}



int tcp_server_init(char *ip_address, unsigned short port)
{
	//unsigned short port=DEFAULT_PORT;
	int retval;
	int fromlen;
	int i;

	message_socket=INVALID_SOCKET;

	// Request Winsock version 2.2
	if ((retval = WSAStartup(0x202, &wsaData)) != 0)
	{
		log_printf("TCP Server: WSAStartup() failed with error %d\n", retval);
		WSACleanup();
		return -1;
	}
	else log_printf("TCP Server: WSAStartup() is OK.\n");

	local.sin_family = AF_INET;
	local.sin_addr.s_addr = (!ip_address) ? INADDR_ANY:inet_addr(ip_address);

	/* Port MUST be in Network Byte Order */
	local.sin_port = htons(port);
	// TCP socket
	listen_socket = socket(AF_INET, socket_type,0);

	if (listen_socket == INVALID_SOCKET){
		log_printf("TCP Server: socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	else log_printf("TCP Server: socket() is OK.\n");

	// bind() associates a local address and port combination with the socket just created.
	// This is most useful when the application is a
	// server that has a well-known port that clients know about in advance.
	if (bind(listen_socket, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR)
	{
		log_printf("TCP Server: bind() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	else log_printf("TCP Server: bind() is OK.\n");

	// So far, everything we did was applicable to TCP as well as UDP.
	// However, there are certain steps that do not work when the server is
	// using UDP. We cannot listen() on a UDP socket.
	if (listen(listen_socket,5) == SOCKET_ERROR)
	{
		log_printf("TCP Server: listen() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	else log_printf("TCP Server: listen() is OK.\n");
	log_printf("TCP Server: %s: I'm listening and waiting connection on port %d, protocol %s\n", ip_address, port, (socket_type == SOCK_STREAM)?"TCP":"UDP");

	return 0;
}



int tcp_client_init(char *server_name, int port)
{
	unsigned int addr;
	int retval, loopflag = 0;
	struct hostent *hp;

	message_socket = INVALID_SOCKET;

	if ((retval = WSAStartup(0x202, &wsaData)) != 0)
	{
		log_printf("TCP Client: WSAStartup() failed with error %d\n", retval);
		WSACleanup();
		return -1;
	}
	else log_printf("TCP Client:  WSAStartup() is OK.\n");

	// Attempt to detect if we should call gethostbyname() or gethostbyaddr()
	if (isalpha(server_name[0]))
	{   // server address is a name
		hp = gethostbyname(server_name);
	}
	else
	{ // Convert nnn.nnn address to a usable one
		addr = inet_addr(server_name);
		hp = gethostbyaddr((char *)&addr, 4, AF_INET);
	}
	if (hp == NULL )
	{
		log_printf("TCP Client: Cannot resolve address \"%s\": Error %d\n", server_name, WSAGetLastError());
		WSACleanup();
		exit(1);
	}
	else log_printf("TCP Client: gethostbyaddr() is OK.\n");
	// Copy the resolved information into the sockaddr_in structure
	memset(&server, 0, sizeof(server));
	memcpy(&(server.sin_addr), hp->h_addr, hp->h_length);
	server.sin_family = hp->h_addrtype;
	server.sin_port = htons(port);

	message_socket = socket(AF_INET, socket_type, 0); /* Open a socket */
	if (message_socket <0 )
	{
		log_printf("TCP Client: Error Opening socket: Error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	else log_printf("TCP Client: socket() is OK.\n");

	// Notice that nothing in this code is specific to whether we
	// are using UDP or TCP.
	// We achieve this by using a simple trick.
	//    When connect() is called on a datagram socket, it does not
	//    actually establish the connection as a stream (TCP) socket
	//    would. Instead, TCP/IP establishes the remote half of the
	//    (LocalIPAddress, LocalPort, RemoteIP, RemotePort) mapping.
	//    This enables us to use send() and recv() on datagram sockets,
	//    instead of recvfrom() and sendto()
	log_printf("TCP Client: Client connecting to: %s.\n", hp->h_name);
	if (connect(message_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		log_printf("TCP Client: connect() failed: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	else log_printf("TCP Client: connect() is OK.\n");

	return 0;
}





#if 0


int ip_server_test(void) //(int argc, char **argv)
{
	char Buffer[128];
	char *ip_address= "127.0.0.1";
	unsigned short port=DEFAULT_PORT;
	int retval;
	int fromlen;
	int i;
	int socket_type = DEFAULT_PROTO;
	struct sockaddr_in local, from;
	WSADATA wsaData;
	SOCKET listen_socket, message_socket;

#if 0
	/* Parse arguments, if there are arguments supplied */
	if (argc > 1)
	{
		for(i=1; i<argc; i++)
		{
			// switches or options...
			if ((argv[i][0] == '-') || (argv[i][0] == '/'))
			{
				// Change to lower...if any
				switch(tolower(argv[i][1]))
				{
					// if -p or /p
				case 'p':
					if (!stricmp(argv[i+1], "TCP"))
						socket_type = SOCK_STREAM;
					else if (!stricmp(argv[i+1], "UDP"))
						socket_type = SOCK_DGRAM;
					else
						Usage(argv[0]);
					i++;
					break;
					// if -i or /i, for server it is not so useful...
				case 'i':
					ip_address = argv[++i];
					break;
					// if -e or /e
				case 'e':
					port = atoi(argv[++i]);
					break;
					// No match...
				default:
					Usage(argv[0]);
					break;
				}
			}
			else
				Usage(argv[0]);
		}
	}
#endif

	// Request Winsock version 2.2
	if ((retval = WSAStartup(0x202, &wsaData)) != 0)
	{
		fprintf(stderr,"Server: WSAStartup() failed with error %d\n", retval);
		WSACleanup();
		return -1;
	}
	else
		printf("Server: WSAStartup() is OK.\n");



	local.sin_family = AF_INET;
	local.sin_addr.s_addr = (!ip_address) ? INADDR_ANY:inet_addr(ip_address);

	/* Port MUST be in Network Byte Order */
	local.sin_port = htons(port);
	// TCP socket
	listen_socket = socket(AF_INET, socket_type,0);

	if (listen_socket == INVALID_SOCKET){
		fprintf(stderr,"Server: socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	else
		printf("Server: socket() is OK.\n");

	// bind() associates a local address and port combination with the socket just created.
	// This is most useful when the application is a
	// server that has a well-known port that clients know about in advance.
	if (bind(listen_socket, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR)
	{
		fprintf(stderr,"Server: bind() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	else
		printf("Server: bind() is OK.\n");

	// So far, everything we did was applicable to TCP as well as UDP.
	// However, there are certain steps that do not work when the server is
	// using UDP. We cannot listen() on a UDP socket.
	if (socket_type != SOCK_DGRAM)
	{
		if (listen(listen_socket,5) == SOCKET_ERROR)
		{
			fprintf(stderr,"Server: listen() failed with error %d\n", WSAGetLastError());
			WSACleanup();
			return -1;
		}
		else
			printf("Server: listen() is OK.\n");
	}
	printf("Server: %s: I'm listening and waiting connection on port %d, protocol %s\n", ip_address, port, (socket_type == SOCK_STREAM)?"TCP":"UDP");

	while(1)
	{
		fromlen =sizeof(from);
		// accept() doesn't make sense on UDP, since we do not listen()
		if (socket_type != SOCK_DGRAM)
		{
			message_socket = accept(listen_socket, (struct sockaddr*)&from, &fromlen);
			if (message_socket == INVALID_SOCKET)
			{
				fprintf(stderr,"Server: accept() error %d\n", WSAGetLastError());
				WSACleanup();
				return -1;
			}
			else
				printf("Server: accept() is OK.\n");
			printf("Server: accepted connection from %s, port %d\n", inet_ntoa(from.sin_addr), htons(from.sin_port)) ;

		}
		else
			message_socket = listen_socket;

		// In the case of SOCK_STREAM, the server can do recv() and send() on
		// the accepted socket and then close it.
		// However, for SOCK_DGRAM (UDP), the server will do recvfrom() and sendto()  in a loop.
		if (socket_type != SOCK_DGRAM)
			retval = recv(message_socket, Buffer, sizeof(Buffer), 0);

		else
		{
			retval = recvfrom(message_socket,Buffer, sizeof(Buffer), 0, (struct sockaddr *)&from, &fromlen);
			printf("Server: Received datagram from %s\n", inet_ntoa(from.sin_addr));
		}

		if (retval == SOCKET_ERROR)
		{
			fprintf(stderr,"Server: recv() failed: error %d\n", WSAGetLastError());
			closesocket(message_socket);
			continue;
		}
		else
			printf("Server: recv() is OK.\n");

		if (retval == 0)
		{
			printf("Server: Client closed connection.\n");
			closesocket(message_socket);
			continue;
		}

		if(retval>0) Buffer[retval] = 0;
		printf("Server: Received %d bytes, data \"%s\" from client\n", retval, Buffer);

		printf("Server: Echoing the same data back to client...\n");
		if (socket_type != SOCK_DGRAM)
			retval = send(message_socket, Buffer, retval /*sizeof(Buffer)*/, 0);
		else
			retval = sendto(message_socket, Buffer, retval /*sizeof(Buffer)*/, 0, (struct sockaddr *)&from, fromlen);

		if (retval == SOCKET_ERROR)
		{
			fprintf(stderr,"Server: send() failed: error %d\n", WSAGetLastError());
		}
		else
			printf("Server: send() is OK.\n");

		if (socket_type != SOCK_DGRAM)
		{
			printf("Server: I'm waiting more connection, try running the client\n");
			printf("Server: program from the same computer or other computer...\n");
			closesocket(message_socket);
		}
		else
			printf("Server: UDP server looping back for more requests\n");
		continue;
	}
	return 0;
}


int ip_client_test(void) //(int argc, char **argv)
{
	char Buffer[128];
	// default to localhost
	char *server_name= "localhost";
	unsigned short port = DEFAULT_PORT;
	int retval, loopflag = 0;
	int i, loopcount, maxloop=-1;
	unsigned int addr;
	int socket_type = DEFAULT_PROTO;
	struct sockaddr_in server;
	struct hostent *hp;
	WSADATA wsaData;
	SOCKET  conn_socket;

#if 0
	if (argc >1)
	{
		for(i=1; i<argc; i++)
		{
			if ((argv[i][0] == '-') || (argv[i][0] == '/'))
			{
				switch(tolower(argv[i][1]))
				{
				case 'p':
					if (!stricmp(argv[i+1], "TCP"))
						socket_type = SOCK_STREAM;
					else if (!stricmp(argv[i+1], "UDP"))
						socket_type = SOCK_DGRAM;
					else
						Usage(argv[0]);
					i++;
					break;
				case 'n':
					server_name = argv[++i];
					break;
				case 'e':
					port = atoi(argv[++i]);
					break;
				case 'l':
					loopflag =1;
					if (argv[i+1]) {
						if (argv[i+1][0] != '-')
							maxloop = atoi(argv[i+1]);
					}
					else
						maxloop = -1;
					i++;
					break;
				default:
					Usage(argv[0]);
					break;
				}
			}
			else
				Usage(argv[0]);
		}
	}
#endif
	if ((retval = WSAStartup(0x202, &wsaData)) != 0)
	{
		fprintf(stderr,"Client: WSAStartup() failed with error %d\n", retval);
		WSACleanup();
		return -1;
	}
	else
		printf("Client: WSAStartup() is OK.\n");


	// Attempt to detect if we should call gethostbyname() or gethostbyaddr()
	if (isalpha(server_name[0]))
	{   // server address is a name
		hp = gethostbyname(server_name);
	}
	else
	{ // Convert nnn.nnn address to a usable one
		addr = inet_addr(server_name);
		hp = gethostbyaddr((char *)&addr, 4, AF_INET);
	}
	if (hp == NULL )
	{
		fprintf(stderr,"Client: Cannot resolve address \"%s\": Error %d\n", server_name, WSAGetLastError());
		WSACleanup();
		exit(1);
	}
	else
		printf("Client: gethostbyaddr() is OK.\n");
	// Copy the resolved information into the sockaddr_in structure
	memset(&server, 0, sizeof(server));
	memcpy(&(server.sin_addr), hp->h_addr, hp->h_length);
	server.sin_family = hp->h_addrtype;
	server.sin_port = htons(port);

	conn_socket = socket(AF_INET, socket_type, 0); /* Open a socket */
	if (conn_socket <0 )
	{
		fprintf(stderr,"Client: Error Opening socket: Error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	else
		printf("Client: socket() is OK.\n");

	// Notice that nothing in this code is specific to whether we
	// are using UDP or TCP.
	// We achieve this by using a simple trick.
	//    When connect() is called on a datagram socket, it does not
	//    actually establish the connection as a stream (TCP) socket
	//    would. Instead, TCP/IP establishes the remote half of the
	//    (LocalIPAddress, LocalPort, RemoteIP, RemotePort) mapping.
	//    This enables us to use send() and recv() on datagram sockets,
	//    instead of recvfrom() and sendto()
	printf("Client: Client connecting to: %s.\n", hp->h_name);
	if (connect(conn_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		fprintf(stderr,"Client: connect() failed: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	else
		printf("Client: connect() is OK.\n");

	// Test sending some string
	loopcount = 0;
	while(1)
	{
		wsprintf(Buffer,"This is a test message from client #%d", loopcount++);
		retval = send(conn_socket, Buffer, sizeof(Buffer), 0);
		if (retval == SOCKET_ERROR)
		{
			fprintf(stderr,"Client: send() failed: error %d.\n", WSAGetLastError());
			WSACleanup();
			return -1;
		}
		else
			printf("Client: send() is OK.\n");
		printf("Client: Sent data \"%s\"\n", Buffer);

		retval = recv(conn_socket, Buffer, sizeof(Buffer), 0);
		if (retval == SOCKET_ERROR)
		{
			fprintf(stderr,"Client: recv() failed: error %d.\n", WSAGetLastError());
			closesocket(conn_socket);
			WSACleanup();
			return -1;
		}
		else
			printf("Client: recv() is OK.\n");

		// We are not likely to see this with UDP, since there is no
		// 'connection' established.
		if (retval == 0)
		{
			printf("Client: Server closed connection.\n");
			closesocket(conn_socket);
			WSACleanup();
			return -1;
		}

		printf("Client: Received %d bytes, data \"%s\" from server.\n", retval, Buffer);
		if (!loopflag)
		{
			printf("Client: Terminating connection...\n");
			break;
		}
		else
		{
			if ((loopcount >= maxloop) && (maxloop >0))
				break;
		}
	}
	closesocket(conn_socket);
	WSACleanup();

	return 0;
}


#endif
