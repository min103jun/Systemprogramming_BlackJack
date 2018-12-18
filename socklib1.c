#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define HOSTLEN 256
#define BACKLOG 1

int make_server_socket_q(int, int);

int make_server_socket(int portnum) 
{
	return make_server_socket_q(portnum, BACKLOG);
}

int make_server_socket_q(int port, int backlog)
{
	struct sockaddr_in saddr;
	struct hostent *hp;
	char hostname[HOSTLEN];
	int sock;

	if((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		return -1;
	
	bzero((void*)&saddr, sizeof(saddr));
	gethostname(hostname, HOSTLEN);
	hp = gethostbyname(hostname);
	bcopy((void*)hp->h_addr, (void *)&saddr.sin_addr, hp->h_length);
	saddr.sin_port = htons(port);
	saddr.sin_family = AF_INET;
	
	if((bind(sock, (struct sockaddr*)&saddr, sizeof(saddr))) != 0)
		return -1;

	if((listen(sock, 0)) != 0)
		return -1;

	return sock;
}
