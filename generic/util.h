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

#ifndef _UTIL_H
#define _UTIL_H

#include <tcl.h>

#define INITRESULT \
    Tcl_DStringFree(&Result); \
    Tcl_DStringInit(&Result);

#define APPENDRESULT(str) \
    Tcl_DStringAppend(&Result, str, -1);

#define SETRESULT(str) \
    INITRESULT() \
    APPENDRESULT(str)

#define GETRESULT \
    Tcl_DStringValue(&Result)

#define FREERESULT \
    Tcl_DStringFree(&Result)

// XXX Does "con" for windows (and OS2?) really works?
#ifdef WIN32
#   define TTY "con"
#   define TMP "c:\\windows\\temp\\"
#else
#   define TTY "/dev/tty"
#   define TMP "/tmp"
#endif

Tcl_DString Result;
extern Tcl_DString Result;

int printres(const char *fmt, ...);
char *captureOutput(void);
int restoreOutput(char *tempfile);
int getOutput(char *tempfile);

#endif /* _UTIL_H */
