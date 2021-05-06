#include "c3includes.h"
#define A 3

#if A == 1
// tcpip-programmierung, uebung 3, aufgabe 1
// programm mit dateinamen in der kommandozeile aufrufen,
// dateiname und attribute groesse und modus (octal) ausgeben
int main(int argc, char *argv[])
{
    char *filename;
    struct stat statbuf;

    // kommandozeile pruefen
    if (argc < 2)
    {
        fprintf(stderr, "Aufruf: %s dateiname\n", argv[0]);
        return 1;
    }
    // dateiname ist 2. parameter
    filename = argv[1];

    // dateiattribute lesen - fehler
    if (stat(filename, &statbuf) < 0)
    {
        perror(filename);
        return 2;
    }

    // dateiattribute ausgeben
    printf("Datei:    %s\n", filename);
    printf("Groesse:  %d Bytes\n", statbuf.st_size);
    printf("Besitzer: %d\n", statbuf.st_uid);
    printf("Gruppe:   %d\n", statbuf.st_gid);
    printf("Modus:    %o\n", statbuf.st_mode);
    if (S_ISREG(statbuf.st_mode))
        printf("Dateityp: regulaere Datei\n");
    else if (S_ISDIR(statbuf.st_mode))
        printf("Dateityp: Verzeichnis\n");
    else
        printf("Dateityp: -\n");

    return 0;
}

#elif A == 2
// tcpip-programmierung, uebung 3, aufgabe 1
// programm mit verzeichnisnamen in der kommandozeile aufrufen,
// verzeichnis durchlaufen und alle dateien mit namen und inode ausgeben
int main(int argc, char *argv[])
{
    char *dirname;
    DIR *dptr;
    struct dirent *deptr;

    // kommandozeile pruefen
    if (argc < 2)
    {
        fprintf(stderr, "Aufruf: %s verzeichnis\n", argv[0]);
        return 1;
    }
    // verzeichnisname ist 2. parameter
    dirname = argv[1];

    // dateiattribute lesen - fehler
    if ((dptr = opendir(dirname)) == NULL)
    {
        perror(dirname);
        return 2;
    }

    // verzeichnis durchlaufen und dateien ausgeben
    while (deptr = readdir(dptr))
        printf("%10d %s\n", deptr->d_ino, deptr->d_name);

    // verzeichnis schliessen
    closedir(dptr);

    return 0;
}
#elif A == 3
// tcpip-programmierung, uebung 3, aufgabe 3
// programm mit verzeichnisnamen in der kommandozeile aufrufen,
// verzeichnis durchlaufen und alle dateien mit namen, inode und
// attributen groesse und modus ausgeben
void showattributes(char *filename)
{
    struct stat statbuf;

    // dateiattribute lesen - fehler
    if (stat(filename, &statbuf) < 0)
    {
        perror(filename);
        return;
    }

    // dateiattribute ausgeben
    printf("Groesse:  %d Bytes\n", statbuf.st_size);
    printf("Besitzer: %d\n", statbuf.st_uid);
    printf("Gruppe:   %d\n", statbuf.st_gid);
    printf("Modus:    %o\n", statbuf.st_mode);
    if (S_ISREG(statbuf.st_mode))
        printf("Dateityp: regulaere Datei\n");
    else if (S_ISDIR(statbuf.st_mode))
        printf("Dateityp: Verzeichnis\n");
    else
        printf("Dateityp: -\n");
}

int main(int argc, char *argv[])
{
    char *dirname;
    DIR *dptr;
    struct dirent *deptr;

    // kommandozeile pruefen
    if (argc < 2)
    {
        fprintf(stderr, "Aufruf: %s verzeichnis\n", argv[0]);
        return 1;
    }
    // verzeichnisname ist 2. parameter
    dirname = argv[1];

    // dateiattribute lesen - fehler
    if ((dptr = opendir(dirname)) == NULL)
    {
        perror(dirname);
        return 2;
    }

    // ins verzeichnis wechseln
    chdir(dirname);

    // verzeichnis durchlaufen und dateien ausgeben
    while (deptr = readdir(dptr))
    {
        printf("\nDatei:    %s\n", deptr->d_name);
        printf("Inode:    %d\n", deptr->d_ino);
        showattributes(deptr->d_name);
    }

    // verzeichnis schliessen
    closedir(dptr);

    return 0;
}
#elif A == 4

#endif