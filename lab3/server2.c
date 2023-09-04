#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include<pthread.h>

#define SIZE 200
#define MAX_CLIENTS 5

pthread_mutex_t socketMutex = PTHREAD_MUTEX_INITIALIZER;
int* flag;
int client_sockets[MAX_CLIENTS];
int num_clients = 0; //number of connected clients


void BroadcastMessage(char* message){
  pthread_mutex_lock(&socketMutex);
  for (int i=0; i <num_clients; i++){
    int client_sock=client_sockets[i];
    write(client_sock,message,strlen(message)+1);
  }
  pthread_mutex_unlock(&socketMutex);
}


void* handleClient(void* client_sock_ptr){
  int client_sock =*(int*)client_sock_ptr;
  while(1){
    char buffer[SIZE];
    read(client_sock,buffer,sizeof(buffer));
    if(buffer[0]=='$'){
      close(client_sock);
      printf("[-]Client disconnected\n");
      break;
    }else{
      BroadcastMessage(buffer);
    }
  }
  close(client_sock);
  printf("[-]Client disconnected\n");
  return NULL;
}

int main(int argc,char** argv){
  char *ip = "127.0.0.1";
  int port = atoi(argv[1]);
  FILE* fp;
  int server_sock;
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

//if successful returns 0
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
    int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, (socklen_t*)&addr_size);
    if(client_sock<0){
        printf("Accept error\n");
        exit(0);
    }
    printf("[+]Client connected using port %d.\n",htons(client_addr.sin_port));
    write(client_sock, flag, sizeof(int));
    write(client_sock, "[+]You have been connected\n", 28);

    pthread_mutex_lock(&socketMutex);
    client_sockets[num_clients++]=client_sock;
    pthread_mutex_unlock(&socketMutex);

    pthread_t childThreadId;
    pthread_create(&childThreadId,NULL,handleClient,&client_sock);
}
    munmap(flag,sizeof(int));

  return 0;
}
