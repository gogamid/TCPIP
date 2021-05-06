//
// tp03-client.c
// Einfacher TCP-Client, liest Datum vom Server
// Server: maya.rz.hs-fulda.de - 193.174.30.140
// URL: maya.rz.hs-fulda.de/tp/tp-date.php
// klin, 25.10.2020

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

// Adresse, Port und Request
#define SERVER_IPADDR   "193.174.30.140"
#define SERVER_PORT     80
#define SERVER_REQUEST  "GET /tp/tp-date.php HTTP/1.0\r\n\r\n"

// main
int main(int argc, char **argv)
{
  int sock_fd;
  struct sockaddr_in server_addr;
  char buffer[512];
  int nbytes, rval, length;

  // Socket anlegen und oeffnen
  // Familie: Internet, Typ: TCP-Socket
  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  fprintf(stderr, "socket: %d\n", sock_fd);
  if(sock_fd < 0) {
    perror("socket");
    return 1;
  }
  
  // Serverstruktur einrichten 
  // Datenstruktur auf 0 setzen
  // Familie: Internetserver
  // Adresse: maya.rz.hs-fulda.de
  // Port: HTTP-Port 80
  memset((void *) &server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IPADDR);
  server_addr.sin_port = htons(SERVER_PORT);
  
  // Zum Server verbinden  
  rval = connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  fprintf(stderr, "connect: %d\n", rval);
  if(rval < 0) {
    perror("connect");
    return 1;
  }

  // Request  an Server schicken
  sprintf(buffer, "%s", SERVER_REQUEST);
  length = strlen(buffer);
  fprintf(stderr, "request: %s len:%d\n", buffer, length);
  nbytes = write(sock_fd, buffer, length);  
  if(nbytes != length) {
    perror("write");
    return 1;  
  }
  
  // Antwort vom Server lesen
  nbytes = read(sock_fd, buffer, sizeof(buffer));
  if(nbytes < 0) {
    perror("read");
    return 1;
  }
  buffer[nbytes] = '\0';
  fprintf(stderr, "read: %d bytes, data: %s\n", nbytes, buffer);

  // Socket schliessen und Ende
  close(sock_fd);
  return 0;

}