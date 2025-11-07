/*
    snmptools - SNMP Agent Access Tools

    Copyright (C) 2008  Alexandros Stergiakis

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
    
#include <stdio.h> // tempnam, fopen, freopen, fclose, fread, feof, remove, fflush
#include <stdarg.h> // vsnprintf
#include <string.h> // strerror
#include <errno.h> // errno
#include <stdlib.h> // free

#include "util.h"

#define BUFLEN 500

char *tempfile;

/* 
 * Print to a buffer rather than a file descriptor
 */
int printres(const char *fmt, ...) {
    va_list ap;
    static char buf[BUFLEN]; /* static because this function is expected to be invoked very often */
    int res;

    va_start(ap, fmt);
    
    res = vsnprintf(buf, BUFLEN, fmt, ap);
    APPENDRESULT(buf);

    va_end(ap);
    return res;
}

/*
 * In this section we use a trick to catch all output sent to
 * stdout and stderr by the library, so that we can use it as
 * the result of the command. We don't have to worry about
 * threads, because the Tcl interpreter (PID) is not using
 * threads anyway (it uses an event loop). With Tcl Threads
 * we have many interpreters (with different PID), so the
 * thread-safe issue doesn't come up as well.
 */
char *captureOutput(void) {
    extern char *tempfile;
    FILE *fd;
    tempfile = tempnam(TMP, "TEMP");

    fd = freopen(tempfile, "a", stdout);
    if(fd == NULL) {
        printres("%s", strerror(errno));
        free(tempfile);
        return(NULL);
    }
    setvbuf(fd, (char *) NULL, _IONBF, 0);

    fd = freopen(tempfile, "a", stderr);
    if(fd == NULL) {
        printres("%s", strerror(errno));
        freopen(TTY, "w", stdout); /* Ignore any errors here */
        free(tempfile);
        return(NULL);
    }
    setvbuf(fd, (char *) NULL, _IONBF, 0);

    return(tempfile);
}

int restoreOutput(char *tempfile) {
    int errors = 0;

    /* Ignore errors but include in the output and return value */
    if(freopen(TTY, "w", stdout) == NULL) {
        errors++; 
        printres("%s", strerror(errno)); 
    }
    if(freopen(TTY, "w", stderr) == NULL) {
        errors++;
        printres("%s", strerror(errno));
    }

    if(remove(tempfile)) {
        errors++;
        printres("%s", strerror(errno));
    }
    free(tempfile);

    return(errors);
}

int getOutput(char *tempfile) {
    FILE *fd;
    int errors = 0;

    /* Ignore errors but include in the output and return value */
    if(freopen(TTY, "w", stdout) == NULL) {
        errors++; 
        printres("%s", strerror(errno)); 
    }
    if(freopen(TTY, "w", stderr) == NULL) {
        errors++;
        printres("%s", strerror(errno));
    }

    fd = fopen(tempfile, "r");
    if(fd == NULL) {
        errors++;
        printres("%s", strerror(errno));
    } else {
        char buf[BUFLEN];
        int ret;
        while(! feof(fd)) {
            ret = fread(buf, 1, BUFLEN - 1, fd);
            if(ret != 0) {
                buf[ret] = '\0';
                printres("%s", buf);
            }
            if(ret < BUFLEN)
                if(ferror(fd)) {
                    errors++;
                    break;
                }
        }

        if(fclose(fd) == EOF) {
            errors++;
            printres("%s", strerror(errno));
        }
    }

    if(remove(tempfile)) {
        errors++;
        printres("%s", strerror(errno));
    }
    free(tempfile);

    return(errors);
}

