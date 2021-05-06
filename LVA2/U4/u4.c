#include "c4includes.h"
#define A 2

#if A == 1
// tcpip-programmierung, uebung 4, aufgabe 1
// von stdin lesen, auf stdout schreiben
// bei eof oder eingabe von "eof" enden
// io-bereitschaft mit select pruefen
// read/write

int main(void)
{
    char buf[128]; // ein-/ausgabepuffer
    int rn, wn;    // anzahl gelesen/geschrieben
    int rs;
    fd_set rfds;

    while (1)
    {
        // stdin mit select pruefen
        FD_ZERO(&rfds);
        FD_SET(fileno(stdin), &rfds);
        rs = select(8, &rfds, NULL, NULL, NULL);
        fprintf(stderr, "select: %d -> fd: %d\n", rs, FD_ISSET(fileno(stdin), &rfds));
        if (FD_ISSET(fileno(stdin), &rfds))
        {
            // zeichen in puffer einlesen
            rn = read(fileno(stdin), buf, sizeof(buf));
            fprintf(stderr, "read:  %d\n", rn);
            // ende bei exit oder eof
            if (rn == 0 || strncmp(buf, "eof", 3) == 0)
                break;
            // gelesene zeichen ausgeben
            wn = write(fileno(stdout), buf, rn);
            fprintf(stderr, "write: %d\n", wn);
        }
    }

    return (0);
}
#elif A == 2
// tcpip-programmierung, uebung 4, aufgabe 2
// io-bereitschaft mit select pruefen
// read/write
int main(void)
{
    char buf[256];
    fd_set fds; // fd_set fuer select
    int pfd[2]; // pipe fds
    int n, i, status, ret, pid;

    // pipe oeffnen
    if (pipe(pfd) < 0)
    {
        perror("pipe()");
        exit(1);
    }

    // kindprozess erzeugen
    if ((pid = fork()) < 0)
    {
        perror("pipe()");
        exit(1);
    }

    // kindprozess
    if (pid == 0)
    {
        close(pfd[0]); // leseseite der pipe schliessen
        pid = getpid();
        ret = 0x11;
        printf("C %d started\n", pid);
        for (i = 1; i < 1000000; i++)
        {
            sleep(5);
            sprintf(buf, "Hallo - %d", i);
            n = strlen(buf);
            printf("C write %d [%s]\n", n, buf);
            n = write(pfd[1], buf, n);
            printf("C wrote %d bytes\n", n);
            if (n <= 0)
            {
                printf("C parent terminated\n");
                ret = 0x44;
                break;
            }
        }
        close(pfd[1]); // pipe schliessen und ende
        printf("C %d terminates %04x\n", pid, ret);
        exit(ret);
    }

    // elternprozess
    else
    {
        close(pfd[1]); // schreibseite der pipe schliessen
        printf("P child %d started\n", pid);
        while (1)
        {
            printf("P select io\n");
            // fd_set initialisieren
            FD_ZERO(&fds);        // nullen
            FD_SET(0, &fds);      // stdin eintragen
            FD_SET(pfd[0], &fds); // leseseiet der pipe eintragen
            ret = select(pfd[0] + 1, &fds, NULL, NULL, NULL);
            // blockiert hier bis stdin oder pipe i/o hat
            if (ret <= 0)
            {
                perror("select()");
                exit(1);
            }

            else
            {
                // io auf stdin pruefen
                if (FD_ISSET(0, &fds))
                {
                    n = read(0, buf, 255);
                    if (n > 0)
                        --n;
                    buf[n] = '\0';
                    printf("P stdin %d -> [%s]\n", n, buf);
                    if (strncmp(buf, "eof", 3) == 0)
                    {
                        printf("P terminates\n");
                        break;
                    }
                }
                // io auf pipe pruefen
                else if (FD_ISSET(pfd[0], &fds))
                {
                    n = read(pfd[0], buf, 255);
                    buf[n] = '\0';
                    if (n <= 0)
                    {
                        printf("P child %d terminated\n", pid);
                        break;
                    }
                    printf("P pipe %d -> [%s]\n", n, buf);
                }
            }
        }
        close(pfd[0]);
        wait(&status);
        printf("P status %04x\n", status);
    }

    return (0);
}
#elif A == 3
#endif