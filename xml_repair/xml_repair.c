#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  // open
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

int get_mem(char *fname, char **p, off_t *len);
int main (int argc, char *argv[]);

int get_mem(char *fname, char **p, off_t *len) {
    struct stat sb;
    int fd;
    char *x;

    fd = open (fname, O_RDWR);
    if (fd == -1) {
        perror ("open");
        return 1;
    }

    if (fstat (fd, &sb) == -1) {
        perror ("fstat");
        return 1;
    }

    if (!S_ISREG (sb.st_mode)) {
        fprintf (stderr, "%s is not a filen", fname);
        return 1;
    }

    x = mmap (0, sb.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    // printf("%p\n", x);

    if (x == MAP_FAILED) {
        perror ("mmap");
        return 1;
    }

    if (close (fd) == -1) {
        perror ("close");
        return 1;
    }

    *len = sb.st_size;
    *p = x;
    return 0;
}

int main (int argc, char *argv[]){
    off_t len, end_p;
    char *p = NULL;

    if (argc < 4) {
        fprintf (stderr, "usage: %s <file.xml> 'end string' 'add string'\n", argv[0]);
        return 1;
    }

    char *end_s = argv[2];
    int end_l = strlen(end_s);
    char *add_s = argv[3];
    int add_l = strlen(add_s);

    get_mem(argv[1], &p, &len);

    // set end_p as last occurence of end_s
    for (end_p = len-end_l; end_p > 0; end_p--)
        if (memcmp(p+end_p, end_s, end_l) == 0)
            break;

    //copy add string after end string
    memcpy(p+end_p+end_l, add_s, add_l);

    if (munmap (p, len) == -1) {
        perror ("munmap");
        return 1;
    }

    //change size of file
    truncate(argv[1], end_p+end_l+add_l);
    return 0;
}
