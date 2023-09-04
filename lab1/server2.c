#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/mman.h>

#define SIZE 200

int main(int argc,char** argv){

  char *ip = "127.0.0.1";
  int port = atoi(argv[1]);
  FILE* fp;
  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;
  int n;

  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0){
    perror("[-]Socket error");
    exit(1);
  }
  printf("[+]TCP server socket created.\n");

  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);

  n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (n < 0){
    perror("[-]Bind error");
    exit(1);
  }
  printf("[+]Bind to the port number: %d\n", port);

  listen(server_sock, 5);
  printf("Listening...\n");

    addr_size = sizeof(client_addr);
      int* flag = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
      *flag=0;
    while(1){
	client_sock = accept(server_sock, (struct sockaddr*)&client_addr, (socklen_t*)&addr_size);
    	if(client_sock<0){
        	printf("Accept error\n");
        	printf("[-] line busy");
        	exit(0);
		}
    	printf("[+]Client connected.\n");
      write(client_sock, flag, sizeof(int));
    write(client_sock, "[+]You have been connected\n", 28);

    pid_t pid = fork();
    if(pid==0){
      while(1){
          char man[10]="man -f ";
          char buffer[SIZE];
          read(client_sock, buffer, sizeof(buffer));
          if(buffer[0] == '$'){
				    close(client_sock);
            printf("[+]Client disconnected.\n\n");
				    break;
			    }else{
          strcat(man,buffer);
          bzero(buffer, sizeof(buffer));
          fp=popen(man,"r");
          if(fp== NULL){
              printf("[-]Unable to open process");
              return 1;
          }
          else{
              fgets(buffer,sizeof(buffer),fp);
          }
          write(client_sock,buffer,sizeof(buffer));
          bzero(buffer, sizeof(buffer));
          }
      }
      }
}

    close(client_sock);
    printf("[+]Client disconnected.\n\n");

  return 0;
}
