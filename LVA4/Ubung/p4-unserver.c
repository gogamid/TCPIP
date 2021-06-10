//
// p4-unserver.c
// Uebung 4 - einfacher UNIX-Socket-Server
// wartet auf Nachricht vom Client, gibt diese aus und schickt eine Antwort.
// Unix-Socket wird dynamisch erzeugt und angezeigt.
// klin, 10.12.2017

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

// Format fuer Socketname: /tmp/tp-sockPID
#define SOCK_FORM "/tmp/tp-sock%d"

// main
int main(int argc, char **argv)
{
  struct sockaddr_un server_addr;
  int server_fd, client_fd;
  char *progname;
  char sockname[128];
  char buffer[1024];
  int nbytes, rval, length;

  // Programmnamen zuweisen
  progname = argv[0];

  // Socketnamen bauen
  sprintf(sockname, SOCK_FORM, getpid());
    
  // Socket anlegen und oeffnen
  // Familie: Internet, Typ: TCP-Socket
  server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if(server_fd < 0) {
    fprintf(stderr, "%s: Kann Socket nicht oeffnen\n", progname);
    return 1;
  }
  
  // Serverstruktur einrichten 
  memset((void *) &server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strcpy(server_addr.sun_path, sockname);
  
  // Socket an Serverstruktur binden
  rval = bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if(rval < 0) {
    fprintf(stderr, "%s: Kann Socket nicht binden\n", progname);
    return 1;
  }

  // Socket auf Empfang einstellen
  rval = listen(server_fd, 6);
  if(rval < 0) {
    fprintf(stderr, "%s: Kann Socket nicht auf Empfang stellen\n", progname);
    return 1;
  }

  // kurze Meldung
  printf("%s: Server gestartet, PID: %d, Socket: %s\n", progname, getpid(), sockname);
    
  // in Schleife auf Verbindung warten und bearbeiten
  while(1) {
  
    // Verbindung herstellen
    client_fd = accept(server_fd, NULL, NULL);
    if(client_fd < 0) {
      fprintf(stderr, "%s: kann Verbindung nicht annehmen\n", progname);
      return 1;
    }
    
    // Nachricht (String) von Client lesen und ausgeben
    nbytes = read(client_fd, buffer, sizeof(buffer) - 1);    
    if(nbytes < 0) {
      fprintf(stderr, "%s: Fehler beim Lesen vom Client\n", progname);
      return 1;
    }
    buffer[nbytes] = '\0';
    printf("%s: Lesen:     %d Bytes, Daten: [%s]\n", progname, nbytes, buffer);

    // Nachricht an Client bauen und senden
    sprintf(buffer, "Hi Client. Hier Server mit PID %d", getpid());
    length = strlen(buffer);
    nbytes = write(client_fd, buffer, length);    
    if(nbytes != length) {
      fprintf(stderr, "%s: Fehler beim Schreiben an Client\n", progname);
      return 1;
    }
    printf("%s: Schreiben: %d Bytes, Daten: [%s]\n", progname, nbytes, buffer);    
	    
    // Client-Socket schliessen
    close(client_fd);

  } // while(1)

  close(server_fd);
  unlink(sockname);
    
  return 0;
}
