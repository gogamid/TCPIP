#include "c2includes.h"
#define A 5

#if A == 1
// tcpip-programmierung, uebung 2, aufgabe 1
// von stdin lesen, auf stdout schreiben
// bei eof oder eingabe von "eof" enden
// read/write

int main(void)
{
    char buf[128]; // ein-/ausgabepuffer
    int rn, wn;    // anzahl gelesen/geschrieben

    while (1)
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

    return (0);
}

#elif A == 2
// tcpip-programmierung, uebung 2, aufgabe 2
// datei oeffnen, von stdin lesen, auf datei datei.txt schreiben
// bei eof oder eingabe von "eof" enden und datei schliessen
// open/read/write/close

int main(void)
{
    char buf[128]; // ein-/ausgabepuffer
    int fd;        // file descriptor
    int rn, wn;    // anzahl gelesen/geschrieben

    // datei zum schreiben oeffnen
    fd = open("datei.txt", O_WRONLY | O_CREAT, 0644);
    if (fd == -1)
    {
        perror("open datei.txt");
        return (1);
    }
    fprintf(stderr, "open fd=%d\n", fd);

    // bis zur eingabe von "!!" lesen
    while (1)
    {
        // zeile von stdin in lesen
        rn = read(fileno(stdin), buf, sizeof(buf));
        fprintf(stderr, "read stdin: %d\n", rn);
        // ende bei "eof" oder eof
        if (rn == 0 || strncmp(buf, "eof", 3) == 0)
            break;
        // zeile auf datei schreiben
        wn = write(fd, buf, rn);
        fprintf(stderr, "write file %d\n", wn);
    }
    // datei schliessen
    close(fd);

    return (0);
}

#elif A == 3
// tcpip-programmierung, uebungen 2, aufgabe 3
// pipe oeffnen, von stdin lesen und auf pipe schreiben
// dann von pipe lesen und auf stdout schreiben
// bei eof oder eingabe von "eof" enden
// pipe/read/write/close
int main(void)
{
    char buf[128]; // ein-/ausgabepuffer
    int pfd[2];    // pipe fds; pfd[0] read, pfd[1] write
    int rp;        // rueckgabe von pipe()
    int rn, wn;    // anzahl gelesen/geschrieben

    // pipe anlegen/oeffnen
    rp = pipe(pfd);
    if (rp == -1)
    {
        perror("pipe anlegen");
        return (1);
    }
    fprintf(stderr, "pipe: read-fd=%d, write-fd=%d\n", pfd[0], pfd[1]);

    // lesen/schreiben
    while (1)
    {
        // zeichen in puffer einlesen
        rn = read(fileno(stdin), buf, sizeof(buf));
        fprintf(stderr, "read stdin: %d\n", rn);
        // ende bei exit oder eof
        if (rn == 0 || strncmp(buf, "eof", 3) == 0)
            break;
        // zeichen auf pipe schreiben
        wn = write(pfd[1], buf, rn);
        fprintf(stderr, "write pipe: %d\n", wn);
        // zeichen von pipe lesen
        rn = read(pfd[0], buf, sizeof(buf));
        fprintf(stderr, "read pipe: %d\n", rn);
        // gelesene zeichen ausgeben
        wn = write(fileno(stdout), buf, rn);
        fprintf(stderr, "write stdin: %d\n", wn);
    }
    close(pfd[0]);
    close(pfd[1]);

    return (0);
}

#elif A == 4
// tcpip-programmierung, uebungen 2, aufgabe 4
// pipe oeffnen, kind prozess erzeugen
// im elternprozess von stdin lesen und auf pipe schreiben
// im kindprozess von pipe lesen und auf stdout schreiben
// bei eof oder eingabe von "eof" enden
// pipe/fork/read/write/close
int main(void)
{
    char buf[128]; // ein-/ausgabepuffer
    int pfd[2];    // pipe fds
    pid_t pid;     // pid des kindprozesses
    int rp;        // rueckgabe von pipe()
    int rn, wn;    // anzahl gelesen/geschrieben
    int status;    // status kindprozess

    // pipe anlegen/oeffnen
    rp = pipe(pfd);
    if (rp == -1)
    {
        perror("pipe anlegen");
        return (1);
    }
    fprintf(stderr, "pipe: read-fd=%d, write-fd=%d\n", pfd[0], pfd[1]);

    // kindprozess erzeugen
    pid = fork();
    fprintf(stderr, "fork: %d\n", pid);

    // fehler in fork
    if (pid == -1)
    {
        perror("fork");
        return (1);
    }

    // kindprozess
    if (pid == 0)
    {
        fprintf(stderr, "child pid = %d\n", getpid());
        // schreibseite der pipe schliessen
        close(pfd[1]);
        // von pipe lesen
        while (1)
        {
            rn = read(pfd[0], buf, sizeof(buf));
            fprintf(stderr, "child read %d\n", rn);
            if (rn <= 0 || strncmp(buf, "eof", 3) == 0)
            {
                fprintf(stderr, "child terminates\n");
                close(pfd[0]);
                return (0);
            }
        }
    }

    // elternprozess
    else
    {
        fprintf(stderr, "parent pid = %d\n", getpid());
        // leseseite der pipe schliessen
        close(pfd[0]);
        while (1)
        {
            rn = read(fileno(stdin), buf, sizeof(buf));
            wn = write(pfd[1], buf, rn);
            fprintf(stderr, "parent write %d\n", wn);
            if (rn <= 0 || strncmp(buf, "eof", 3) == 0)
                break;
        }
    }
    // schreibseite der pipe schliessen
    close(pfd[1]);
    wait(&status);
    fprintf(stderr, "parent terminates. status = %d\n", status);

    return (0);
}
#elif A == 5
// tcpip-programmierung, uebungen 2, aufgabe 5
// pipe oeffnen, kind prozess erzeugen
// im kindprozess leseseite pipe -> stdin = 0, dann exec "cat -n"
// im elternprozess schreibseite pipe -> stdout = 1, dann exec "ls -l"
// pipe/fork/dup2/read/write/close
int main(void)
{
    int pfd[2]; // pipe fds
    pid_t pid;  // pid des kindprozesses
    int rp;     // rueckgabe von pipe()

    // pipe anlegen/oeffnen
    rp = pipe(pfd);
    if (rp == -1)
    {
        perror("pipe anlegen");
        return (1);
    }
    fprintf(stderr, "pipe: read-fd=%d, write-fd=%d\n", pfd[0], pfd[1]);

    // kindprozess erzeugen
    pid = fork();
    fprintf(stderr, "fork: %d\n", pid);

    // fehler in fork
    if (pid == -1)
    {
        perror("fork");
        return (1);
    }

    // kindprozess
    if (pid == 0)
    {
        fprintf(stderr, "child pid = %d\n", getpid());
        // schreibseite der pipe schliessen
        close(pfd[1]);
        // leseseite der pipe auf stdin/0 duplizieren
        if (dup2(pfd[0], fileno(stdin)) < 0)
        {
            perror("child dup");
            return (1);
        }
        close(pfd[0]);
        // cat -n ausfuehren
        execl("/usr/bin/cat", "cat", "-n", NULL);
        // hier gehts nur bei fehler weiter
        perror("child exec cat -n");
        return (1);
    }

    // elternprozess
    else
    {
        fprintf(stderr, "parent pid = %d\n", getpid());
        // leseseite der pipe schliessen
        close(pfd[0]);
        // schreibseite der pipe auf stdout/1
        if (dup2(pfd[1], fileno(stdout)) < 0)
        {
            perror("parent dup2");
            return (1);
        }
        close(pfd[1]);
        // ls -l ausfuehren
        execl("/usr/bin/ls", "ls", "-l", NULL);
        // hier gehts nur bei fehler weiter
        perror("parent exec ls -l");
        return (1);
    }

    return (0);
}
#endif