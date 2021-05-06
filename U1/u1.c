#include "c1includes.h"
#define A 4

#if A == 1
int main(int argc, char const *argv[])
{
    pid_t pid = getpid();   //PID
    pid_t ppid = getppid(); //PARENT ID
    uid_t uid = getuid();   //User id
    gid_t gid = getgid();   //group id
    printf("pid: %d \n", pid);
    printf("ppid: %d \n", ppid);
    printf("uid: %d \n", uid);
    printf("gid: %d \n", gid);
    return 0;
}

#elif A == 2
int main(int argc, char const *argv[])
{
    int pid;    // returnwert von fork()
    int status; // rueckgabe von status()

    // meldung vor fork()
    printf("PARENT\tvor  fork() mypid=%d\n", getpid());

    // prozesskopie -> neuer prozess / kindprozess
    pid = fork();

    // nach fork() -> 2 prozesse: PARENT und CHILD

    // child process mit pid 0
    if (pid == 0)
    {
        printf("CHILD\t\t\tnach fork() pid:%d mypid=%d\n", pid, getpid());
        printf("CHILD\t\t\tppid=%d\n", getppid());
        printf("CHILD\t\t\texit mit 0xAA\n");
        exit(0xAA);
    }

    // parent process mit pid des child
    else
    {
        printf("PARENT\tnach fork() pid:%d mypid=%d\n", pid, getpid());
        printf("PARENT\tauf child exit warten\n");
        // auf ende vom child warten
        wait(&status);
        // child beendet
        printf("PARENT\tnach wait() child beendet\n");
        // exitwert von child in oberem byte des status
        printf("PARENT\tchild status:%0x04X return:0x%02X\n", status, status >> 8);
    }
    return 0;
}

#elif A == 3
int main(int argc, char const *argv[])
{
    int pid;    // returnwert von fork()
    int status; // rueckgabe von status()

    // meldung vor fork()
    printf("PARENT\tvor  fork() mypid=%d\n", getpid());

    // prozesskopie -> neuer prozess / kindprozess
    pid = fork();

    // nach fork() -> 2 prozesse: PARENT und CHILD

    // child process mit pid 0
    if (pid == 0)
    {
        printf("CHILD\t\t\tnach fork() pid:%d mypid=%d\n", pid, getpid());
        printf("CHILD\t\t\tppid=%d\n", getppid());
        // kommando ps -f laden und ausfuehren
        execl("/bin/ps", "ps", "-f", NULL);
        // nie erreicht, ausser nach fehler in exec()
        printf("CHILD\t\t\texit mit 0xAA\n");
        exit(0xAA);
    }

    // parent process mit pid des child
    else
    {
        printf("PARENT\tnach fork() pid:%d mypid=%d\n", pid, getpid());
        printf("PARENT\tauf child exit warten\n");
        // auf ende vom child warten
        wait(&status);
        // child beendet
        printf("PARENT\tnach wait() child beendet\n");
        // exitwert von child in oberem byte des status
        printf("PARENT\tchild status:%0x04X return:0x%02X\n", status, status >> 8);
    }
    return 0;
}
#elif A == 4
volatile int scnt = 0;
const int smax = 5;

void handleint(int sig)
{
    signal(SIGINT, handleint);
    ++scnt;
    printf("process %d <- signal SIGINT (%d) - %d/%d\n", getpid(), sig, scnt, smax);
}
void handleusr1(int sig)
{
    signal(SIGUSR1, handleusr1);
    ++scnt;
    printf("process %d <- signal SIGUSR1 (%d) - %d / %d\n", getpid(), sig, scnt, smax);
}
int main(int argc, char const *argv[])
{
    signal(SIGINT, handleint);
    signal(SIGUSR1, handleusr1);
    printf("signal handler fuer SIGINT und SIGUSR1 einrichten\n");
    printf("abbruch des programms nach %d signalen\n", smax);
    printf("signal SIGINT  \"<Strg-C>\"\n");
    printf("signal SIGUSR1 \"kill -USR1 %d\"\n", getpid());

    while (scnt < smax)
    {
        printf(".");
        fflush(stdout);
        sleep(2);
    }
    printf("programmende nach %d signalen\n", scnt);
    return 0;
}
#elif A == 5
// zaehler fuer signal SIGUSR1
volatile int scnt = 0;

// maximale anzahl an signalen
const int smax = 3;

// signalhandler fuer SIGUSR1
void handleusr1(int sig)
{
    // zaehler hochzaehlen und ausgeben
    ++scnt;
    printf("PARENT\tsignal SIGUSR1 - #%d\n", scnt);
    // handler erneut installieren
    signal(SIGUSR1, handleusr1);
}

int main(int argc, char const *argv[])
{
    int pid;    // returnwert von fork()
    int status; // rueckgabe von status()

    // meldung vor fork()
    printf("PARENT\tvor  fork() mypid=%d\n", getpid());

    // prozesskopie -> neuer prozess / kindprozess
    pid = fork();

    // nach fork() -> 2 prozesse: PARENT und CHILD

    // child process mit pid 0
    if (pid == 0)
    {
        pid_t cpid, ppid;
        int i;
        cpid = getpid();
        ppid = getppid();
        printf("CHILD\t\t\tnach fork() mypid:%d pid=%d\n", cpid, pid);
        for (i = 1; i <= smax; i++)
        {
            printf("CHILD\t\t\tsende USR1 signal #%d an parent %d\n", i, ppid);
            kill(ppid, SIGUSR1);
            sleep(2);
        }
        printf("CHILD\t\t\texit mit 0xAA\n");
        exit(0xAA);
    }

    // parent process mit pid des child
    else
    {

        pid_t ppid;
        ppid = getpid();
        // signal SIGUSR1 abfangen
        signal(SIGUSR1, handleusr1);
        printf("PARENT\tnach fork() mypid:%d pid=%d\n", ppid, pid);
        // auf signale vom child warten
        while (scnt < smax)
            ;

        // auf ende vom child warten
        wait(&status);
        printf("PARENT\tnach wait() status:0x%04x return:0x%02X\n", status, status >> 8);
    }
    return 0;
}
#endif
