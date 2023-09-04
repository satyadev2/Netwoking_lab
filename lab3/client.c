#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h> //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>	   //write
#include <sys/types.h>
#include<pthread.h>

#define MAXLINE 200
pthread_mutex_t socketMutex = PTHREAD_MUTEX_INITIALIZER;
int node_id;

int shouldTerminate = 0;


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

void* readThread(void* sockfd_ptr){
	int sockfd=*(int*)sockfd_ptr;
	while (!shouldTerminate) {
		char buffer[MAXLINE];
		read(sockfd, buffer, sizeof(buffer));
		printf("%s",buffer);
		if (buffer[0] == '$') {
			shouldTerminate=1;
			close(sockfd);
			printf("[-]Client disconnected\n");
			break;
		} else {
			int from, to;
			char message[MAXLINE];
			sscanf(buffer, "%d %d %[^\n]", &from, &to, message);
			if (to == node_id) {
				printf("Message accepted: node [%d] to node[%d] - %s\n", from, to, message);
			} else {
				printf("Message declined: node[%d] to node[%d] - %s\n", from, to, message);
			}
		}
	}

	return NULL;
}

void* writeThread(void* sockfd_ptr){
	int sockfd=*(int*)sockfd_ptr;
	while(1){
		char message[MAXLINE];
		printf("Enter message in format (<Node from> <Node to> <message>): ");
		int n = 0;
		while ((message[n++] = getchar()) != '\n');
		pthread_mutex_lock(&socketMutex);
        write(sockfd, message, strlen(message) + 1);
        pthread_mutex_unlock(&socketMutex);
	}
	return NULL;
}

int main(int argc, char **argv)
{
	int flag = 0;
	int sockfd, n;
	struct sockaddr_in servaddr;
	if (argc != 4){
		printf("Usage: %s <server_ip> <server_port> <node_id>\n", argv[0]);
	}

	node_id=atoi(argv[3]);

	if (node_id<1 || node_id > 5){
		printf("[-]Invalid node ID: %d\n",node_id);
		return 1;
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("[-]socket error");
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
		printf("[-]inet_pton error for %s", argv[1]);
		return 1;
	}

	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("[-]connection errorr");
		return 1;
	}
	write(sockfd,&node_id,sizeof(node_id));
	flag =read_integer(sockfd);
	if (flag == 0)
	{
		printf("Node [%d] is connected to the server..\n",node_id);
		pthread_t readThreadId, writeThreadId;
		pthread_create(&readThreadId,NULL,readThread,&sockfd);
		pthread_create(&writeThreadId,NULL,writeThread,&sockfd);

		pthread_join(readThreadId,NULL);
		pthread_join(writeThreadId,NULL);
	}
	else
	{
		printf("Node %d is not connected to the server\n",node_id);
	}
	shouldTerminate=1;
	close(sockfd);
	exit(0);
}
