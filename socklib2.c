#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int connect_to_server(char *host, int port)
{
	struct sockaddr_in saddr;
	struct hostent *hp;
	int sock;

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return -1;

	bzero(&saddr, sizeof(saddr));
	hp = gethostbyname(host);
	if(hp == NULL)
		return -1;
	bcopy(hp->h_addr, (struct sockaddr *)&saddr.sin_addr, hp->h_length);
	saddr.sin_port = htons(port);
	saddr.sin_family = AF_INET;

	if((connect(sock, (struct sockaddr *)&saddr, sizeof(saddr))) != 0)
		return -1;

	return sock;
}
	
