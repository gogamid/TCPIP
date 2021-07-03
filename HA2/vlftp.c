/*
This is client of very lightweight FTP. 

Commands awailable:

pwd - print current working directory
cd directory – change directory
dir [files|directory] – list directory
get remotefile [localfile] - get (copy) file from server to local host
put localfile [remotefile] – put (copy) file from local host to server

Compilation:

gcc vlftp.c -o vlftp
sudo cp vlftp /usr/local/bin

Usage:

vlftp server command [argument1 [argument2]]

Examples:

vlftp localhost pwd
vlftp localhost dir /home
vlftp localhost cd ..
vlftp localhost get aufgabe.txt get.txt
vlftp localhost put aufgabe.txt put.txt


@gogamid ->Imron Gamidli
https://github.com/gogamid/TCPIP/tree/main/HA2

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
#define CONTENT_BUFFER 1000000

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

    char command[CONTENT_BUFFER];
    char localFileName[100];
    int get = 0;

    //pwd
    if (strcmp(argv[2], "pwd") == 0)
    {
        strcpy(command, "pwd");
    }
    // cd
    else if (strcmp(argv[2], "cd") == 0)
    {
        if (argv[3] == NULL)
        {
            fprintf(stderr, "cd requiers directory parameter\nExample: vlftp localhost cd /home\n");
            return 1;
        }
        sprintf(command, "a%s", argv[3]);
    }
    // dir
    else if (strcmp(argv[2], "dir") == 0)
    {
        if (argv[3] != NULL)
        {
            sprintf(command, "dir %s", argv[3]);
        }
        else
            sprintf(command, "dir");
    }
    //get
    else if (strcmp(argv[2], "get") == 0)
    {
        if (argv[3] == NULL)
        {
            fprintf(stderr, "get requires remotefile name as a parameter\n");
            return 1;
        }
        sprintf(command, "bget %s", argv[3]);
        get = 1;
        if (argv[4] != NULL)
            sprintf(localFileName, "%s", argv[4]);
        else
            sprintf(localFileName, "%s", argv[3]);
    }
    //put
    else if (strcmp(argv[2], "put") == 0)
    {
        if (argv[3] == NULL)
        {
            fprintf(stderr, "put requires localfile name as a parameter\n");
            return 1;
        }
        char rFilename[100];
        if (argv[4] != NULL)
            sprintf(rFilename, "%s", argv[4]);
        else
            sprintf(rFilename, "%s", argv[3]);

        char content_buffer[CONTENT_BUFFER];
        int fdr, nr;
        fdr = open(argv[3], O_RDONLY);
        if (fdr == -1)
        {
            fprintf(stderr, "localfile called %s does not exist\n", argv[3]);
            return 1;
        }
        nr = read(fdr, content_buffer, sizeof(content_buffer));
        nr=1+nr+strlen(rFilename);
        sprintf(command, "c %s %s",rFilename,content_buffer); //to be extracted in server
       
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
    char buffer[CONTENT_BUFFER];
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
    char buf[CONTENT_BUFFER]; // Lesepuffer
    int nb;

    nb = read(sock_fd, buf, sizeof(buf));
    buf[nb] = '\0';
    if (get)
    {
        int fdw, nw;
        fdw = open(localFileName, O_WRONLY | O_CREAT, 0644);
        if (fdw == -1)
            printf("fehler beim oefnen des files");
        nw = write(fdw, buf, nb);
        printf("file is copied from server with name %s\n", localFileName);
        get=0;
    }
    else
        fprintf(stderr, "%s", buf);

    // Verbindung und Socket schliessen und kurze Pause
    sleep(1);
    close(sock_fd);

    return 0;
}