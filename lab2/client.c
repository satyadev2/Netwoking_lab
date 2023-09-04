#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h> //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>	   //write
#include <sys/types.h>

#define MAXLINE 5000
int read_integer(int sockfd)
{
	int n;
	int value;

	n = read(sockfd, &value, sizeof(value));
	if (n < 0)
	{
		perror("read");
		return -1;
	}
	value = ntohl(value);

	return value;
}
int main(int argc, char **argv)
{
	int flag = 0;
	int sockfd, n;
	FILE* fptr;
	struct sockaddr_in servaddr;
	fptr = fopen("./alice.txt", "r");
  
    if (fptr == NULL) {
        printf("[-] File couldnot be opened!\n");
    }else{
		printf("[+] File opened!\n");
	}
  
	if (argc != 3)
	{
		printf("no port number supplied");
		return 1;
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error");
		return 1;
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	int port = atoi(argv[2]);
	servaddr.sin_port = htons((uint16_t)port); /* daytime server */

	// Convert IPv4 and IPv6 addresses from text to binary form
	int x;
	x = inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	if (x < 0)
	{
		printf("inet_pton error for %s", argv[1]);
		return 1;
	}

	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("connection errorr");
		return 1;
	}
	char data[MAXLINE];

	flag =read_integer(sockfd);
	printf("%d\n", flag);
	if (flag == 0)
	{
		//you have been connected message
		read(sockfd, data, sizeof(data));
		printf("%s", data);
		bzero(data, sizeof(data));
		while (fgets(data, 5000, fptr) != NULL)
		{
			//read from file into data ,5000 bytes
			write(sockfd, data, sizeof(data));
			bzero(data, sizeof(data));
			//reading ack
			if(read(sockfd, data, sizeof(data)) && strcmp(data,"ACK")<0){
				printf("[-] Error received, exiting :\()");
				return 0;
			}else{
			printf("%s\n", data);
			}
		}
	}
	else
	{
		printf("[-] Line busy\n");
	}
	close(sockfd);
	exit(0);
}
