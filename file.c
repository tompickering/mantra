#include "file.h"

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

void file_init() {
    char* usr_home = NULL;

    usr_home = getenv("HOME");

    if (usr_home == NULL)
        usr_home = getpwuid(getuid())->pw_dir;

    if (usr_home == NULL) {
        fprintf(stderr, "Error: Unable to determine user home directory.\n");
        exit(1);
    } else if (chdir(usr_home) != 0) {
        fprintf(stderr, "Error: Unable to change to user home directory.\n");
        exit(1);
    }

    errno = 0;
    mkdir(MANTRA_HOME, 0700);

    if (errno != 0 && errno != EEXIST) {
        fprintf(stderr, "Error: Unable to create directory %s.\n", MANTRA_HOME);
        exit(1);
    }
}
