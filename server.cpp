#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>
 
char response[] = "HTTP/1.1 200 OK\nContent-Length:238\nConnection: close\n"
"Content-Type: text/html; charset=UTF-8\n\n"
"<!DOCTYPE html><html><head><title>Static Server</title>"
"<style>body { background-color: #111 }"
"h1 { font-size:4cm; text-align: center; color: black;"
" text-shadow: 0 0 2mm red}</style></head>"
"<body><h1>Hello, world!</h1></body></html>\n";
 
int main(int argc, char **argv)
{
  FILE *fp;
  fp = fopen(argv[2],"r");
  if( fp == NULL )
  {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }
  int len = (long)lseek(fp, (off_t)0, SEEK_END);
  (void)lseek(fp, (off_t)0, SEEK_SET);

  int one = 1, client_fd;
  struct sockaddr_in svr_addr, cli_addr;
  socklen_t sin_len = sizeof(cli_addr);
 
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    err(1, "can't open socket");

  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

  int port = atoi(argv[1]);
  if(port < 0 || port >60000) {
    perror("Invalid port number (try 1->60000)\n");
    exit(EXIT_FAILURE);
  }

  svr_addr.sin_family = AF_INET;
  svr_addr.sin_addr.s_addr = INADDR_ANY;
  svr_addr.sin_port = htons(port);

  if (bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr)) == -1) {
    close(sock);
    err(1, "Can't bind");
  }

  if(fork() != 0) {
    printf("Start server\n");
    return 0;
  }

  int size_response = sizeof(response) - 1; /*-1:'\0'*/
  listen(sock, 5);
  while (1) {
    client_fd = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);
    // printf("got connection\n");
    if (client_fd == -1) {
      perror("Can't accept");
      continue;
    }
    write(client_fd, response, size_response);
    close(client_fd);
  }

}
