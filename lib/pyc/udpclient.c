#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>

int
send_usage_data( const char *send_data )
{
	int sock, len;
	struct sockaddr_in server_addr;
	struct hostent *host;

	host= (struct hostent *) gethostbyname((char *)"idcas003");

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		return 1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9600);
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(server_addr.sin_zero),8);

	int send_data_len = strlen(send_data);
	len = sendto(sock, send_data, send_data_len, 0,
		(struct sockaddr *)&server_addr, sizeof(struct sockaddr));

	if (len == send_data_len)
		return 0;

	return -1;
}
