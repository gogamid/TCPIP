// client
/*

*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

// Port
const int srv_port = 8080;
#define COMMAND_BUFFER 1000
#define CONTENT_BUFFER 1000

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "remotehost and command is needed\nExample: vlftp localhost pwd\n");
        return 1;
    }
    struct hostent *hostptr;

    // hostent-Daten lesen
    if ((hostptr = gethostbyname(argv[1])) == NULL)
    {
        fprintf(stderr, "gethostbyname: Host %s nicht gefunden\n\n", argv[1]);
        return 1;
    }

    char command[COMMAND_BUFFER];

    //pwd
    if (strcmp(argv[2], "pwd") == 0){
        strcpy(command, "pwd");
    }
    // cd   TODO
    else if (strcmp(argv[2], "cd") == 0){
        if(argc < 4) {
            fprintf(stderr, "cd requiers directory parameter\nExample: vlftp localhost cd /home\n");
            return 1;
        }
        sprintf(command, "cd %s", argv[3]);
    }
    // dir
    else if (strcmp(argv[2], "dir") == 0){
        if(argv[3] != NULL) {
            sprintf(command, "dir %s", argv[3]);
        }
        else sprintf(command, "dir");
    }
    //get TODO
    else if (strcmp(argv[2], "get") == 0){
        if(argv[3] == NULL) {
            fprintf(stderr, "get requires remotefile name as a parameter\n");
            return 1;
        }
        sprintf(command, "get %s", argv[3]);
        if(argv[4] != NULL) {
            sprintf(command, "get %s %s",argv[3], argv[4]);
        }
        printf("%s", command);
    }
    //put TODO
    else if (strcmp(argv[2], "put") == 0){
        if(argv[3] == NULL) {
            fprintf(stderr, "put requires localfile name as a parameter\n");
            return 1;
        }
        char content_buffer[CONTENT_BUFFER];
        int fdr, nr;
        fdr = open(argv[3], O_RDONLY);
        if(fdr==-1) {
            fprintf(stderr, "localfile called %s does not exist\n", argv[3]);
            return 1;
        }
        // nr=read(fdr, content_buffer,sizeof(content_buffer));
        if(argv[4] != NULL) {
            sprintf(command, "put %s %s",argv[3], argv[4]);
        }
    }

    struct sockaddr_in server_addr; // Serveradresse
    int sock_fd;                    // Socket
    int rv;

    char *server_ip = inet_ntoa(*(struct in_addr *)hostptr->h_addr);

    // Socket oeffnen
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Serveradresse einstellen
    memset((void *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(srv_port);

    // Zum Server verbinden
    rv = connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Request  an Server schicken
    // SERVER_REQUEST ist Formatstring mit Platzhalter fuer Zaehler (s.o.)
    char buffer[1000];
    int nbytes, rval, length;
    sprintf(buffer, "%s", command);
    length = strlen(buffer);
    nbytes = write(sock_fd, buffer, length);
    if (nbytes != length)
    {
        perror("write");
        return 1;
    }

    // String vom Server lesen
    char buf[COMMAND_BUFFER]; // Lesepuffer
    int nb;

    nb = read(sock_fd, buf, sizeof(buf));
    buf[nb] = '\0';
    fprintf(stderr, "%s", buf);

    // Verbindung und Socket schliessen und kurze Pause
    sleep(1);
    close(sock_fd);

    return 0;
}