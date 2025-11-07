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

#if HAVE_STRING_H
#include <string.h> // strerror
#else
#include <strings.h>
#endif
#include <errno.h> // errno
#include <stdlib.h> // malloc, free

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "snmptools.h"
#include "util.h"

static void
optProc(int argc, char *const *argv, int opt)
{
    /* A dummy function because snmp_parse_args requires one to 
       execute, even if there are no local options */
}

int
snmpsession(int argc, char *argv[], netsnmp_session **sessionPtr, netsnmp_session **ssPtr)
{
    netsnmp_session *session, *ss;
    int arg;
    
    session = (netsnmp_session *) malloc(sizeof(netsnmp_session));

    /* get the common command line arguments */
    arg = snmp_parse_args(argc, argv, session, NULL, optProc);

    /* Return values:
       -2  Error in operation.
           There is some output generated in stdout and/or stderr.
       -1  Error in arguments' syntax.
           There is some output generated in stdout and/or stderr.
      >=0  It went ok. */
    if (arg < 0) {
        free(session);
        return(arg * -1);
    }

    SOCK_STARTUP;

    /*
     * Open an SNMP session.
     */
    ss = snmp_open(session);
    if (ss == NULL) {
        /*
         * diagnose snmp_open errors with the input netsnmp_session pointer
         */
        snmptools_snmp_sess_perror("session", session);
        SOCK_CLEANUP;
        free(session);
        return(2);
    }

    *sessionPtr = (void *) session;
    *ssPtr = (void *) ss;

    return(0);
}
