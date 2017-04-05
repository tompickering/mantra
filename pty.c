/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * Mantra: Man page bookmarker                                           *
 * Copyright (C) 2015  Tom Pickering                                     *
 * Code in this file derived from code (C) 2015 Michael Kerrisk          *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* ACKNOWLEDGEMENT:
 *
 * Mantra represents my first foray into programming pseudoterminals. Much
 * of the code in this file is based heavily upon Michael Kerrisk's example
 * code featured in 'The Linux Programming Interface'.
 *
 * I would have had a very hard time trying to figure out how to piece this
 * together without examples such as this, and I would briefly like to thank
 * Michael for this and his support of the Open Source philosophy. The GPLv3
 * code is available at http://man7.org/tlpi/code/online/book/pty/
 *
 * -- Tom Pickering
 */

#include "pty.h"

#define _GNU_SOURCE   /* Needed for ptsname() definition */
#define PTY_IO_BUF_SZ 256

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <termios.h>
#include <errno.h>

#include "error.h"

int init_tty_raw(int fd, struct termios *tty_base_attrs) {
    struct termios tty_raw_attrs;

    memcpy(&tty_raw_attrs, tty_base_attrs, sizeof(struct termios));

    /* Disable special handling of CR, NL, and BREAK.
     * No 8th-bit stripping or parity error handling.
     * Disable START/STOP output flow control. */
    tty_raw_attrs.c_lflag &= ~(ICANON | ISIG | IEXTEN | ECHO);
    tty_raw_attrs.c_iflag &= ~(BRKINT | ICRNL | IGNBRK | IGNCR | INLCR | INPCK | ISTRIP | IXON | PARMRK);

    /* Disable all output processing */
    tty_raw_attrs.c_oflag &= ~OPOST;

    /* Character-at-a-time input */
    tty_raw_attrs.c_cc[VMIN] = 1;

    /* with blocking */
    tty_raw_attrs.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSADRAIN, &tty_raw_attrs) == -1)
        return -1;

    return 0;
}

int run_pty(char **cmd, char *input) {
    struct termios tty_attrs;
    struct winsize ws;
    pid_t child_pid;
    int master;
    int status;

    master = open("/dev/ptmx", O_RDWR | O_NOCTTY);

    if (master == -1)
        die("Unable to open pty master");
    if (tcgetattr(STDIN_FILENO, &tty_attrs) == -1)
        die(NULL);
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0)
        die(NULL);

    child_pid = fork();
    if (child_pid == -1)
        die(NULL);

    if (child_pid == 0) {
        int slave;

        setsid();
        grantpt(master);
        unlockpt(master);
        slave = open(ptsname(master), O_RDWR);

        if (slave == -1)
            die(NULL);

        /* Set tty attributes */
        if (tcsetattr(slave, TCSANOW, &tty_attrs) == -1)
            die(NULL);

        /* Set window size */
        if (ioctl(slave, TIOCSWINSZ, &ws) == -1)
            die(NULL);

        /* Hook standard file descriptors up
         * to the master/slave interface */
        if (dup2(slave, STDOUT_FILENO) == -1
         || dup2(slave, STDIN_FILENO)  == -1
         || dup2(slave, STDERR_FILENO) == -1)
            die(NULL);

        /* No longer needed - we now have the
         * duplicate descriptors */
        close(slave);


        execvp(cmd[0], cmd);

        /* Only reached if execvp fails */
        die(NULL);

    } else {
        char buf[PTY_IO_BUF_SZ];
        char buf2[PTY_IO_BUF_SZ];
        ssize_t bytes_read;
        fd_set input_fd_set;

        /* Put tty into raw mode */
        if (init_tty_raw(STDIN_FILENO, &tty_attrs) != 0)
            die(NULL);

        /* Run any specified input to our command */
        if (input != NULL)
            if (write(master, input, strlen(input)) != strlen(input))
                die(NULL);

        while (1) {

            /* Add the descriptors that we're listening for
               input from to a file descrptor set */
            FD_ZERO(&input_fd_set);
            FD_SET(STDIN_FILENO, &input_fd_set);
            FD_SET(master, &input_fd_set);

            /* Filter descriptors out of fd_set if no input to read */
            if (select(master + 1, &input_fd_set, NULL, NULL, NULL) == -1) {
                switch (errno) {
                    case EINTR:
                        /* TODO: Resize pty terminal */
                        break;
                    default:
                        die(NULL);
                        break;
                }
            }

            /* stdin -> pty */
            if (FD_ISSET(STDIN_FILENO, &input_fd_set)) {
                bytes_read = read(STDIN_FILENO, buf2, PTY_IO_BUF_SZ);
                if (bytes_read > 0) {
                    if (write(master, buf2, bytes_read) != bytes_read)
                        die(NULL);
                }
            }

            /* pty -> stdout */
            if (FD_ISSET(master, &input_fd_set)) {
                bytes_read = read(master, buf, PTY_IO_BUF_SZ);
                if (bytes_read > 0) {
                    if (write(STDOUT_FILENO, buf, bytes_read) != bytes_read)
                        die(NULL);
                }
            }

            if (waitpid(child_pid, &status, WNOHANG) > 0)
                break;
        }

        /* Close pty master */
        close(master);

        /* Restore original tty settings */
        if (tcsetattr(STDIN_FILENO, TCSADRAIN, &tty_attrs) == -1)
            die(NULL);
    }

    return WEXITSTATUS(status);
}
