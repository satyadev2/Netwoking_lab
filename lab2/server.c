#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <fcntl.h>

#define SIZE 5000

int main(int argc,char** argv)
{

  char *ip = "127.0.0.1";
  int port = atoi(argv[1]);
  int fout;

  fout = open("output.txt", O_CREAT | O_WRONLY, 0666);
  
  // checking if the file is opened successfully
 
  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;
  int n;

  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0)
  {
    perror("[-]Socket error");
    exit(1);
  }
  printf("[+]TCP server socket created.\n");

  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);

  n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (n < 0)
  {
    perror("[-]Bind error");
    exit(1);
  }
  printf("[+]Bind to the port number: %d\n", port);

  listen(server_sock, 5);
  printf("Listening...\n");

  addr_size = sizeof(client_addr);
  int *flag = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  *flag = 0;
  pid_t pid = fork();
  while (1)
  {
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
    if (client_sock < 0)
    {
      printf("Accept error\n");
      exit(0);
    }

    if (*flag >= 1)
    {
      write(client_sock, flag, sizeof(int));
      close(client_sock);
      continue;
    }

    write(client_sock, flag, sizeof(int));
    *flag = *flag + 1;
    write(client_sock, "[+]You have been connected\n", 28);
    while(1){
      char buffer[SIZE];
      //reading 5000 bytes
      read(client_sock, buffer, sizeof(buffer));
      int stdou = dup(1);
      dup2(fout, 1);
      printf("%s",buffer);
      dup2(stdou, 1);
      bzero(buffer, sizeof(buffer));
    //write ack
      strcpy(buffer,"ACK");
      write(client_sock, buffer, sizeof(buffer));
      bzero(buffer, sizeof(buffer));
    }
    close(client_sock);
    *flag = *flag - 1;
    printf("[+]Client disconnected.\n\n");
  }
  shutdown(server_sock, SHUT_RDWR);
  return 0;
}
