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
    
#include <stdio.h>
#include <stdlib.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "snmptools.h"
#include "util.h"

#define BUFLEN 500

/**
 * Reimplementation of Net-SNMP's fprint_objid() to print to memory, rather than to
 * a file descriptor.
 * 
 * Prints an oid to a memory buffer.
 *
 * @param s          The memory buffer to print to (at least 256 bytes).
 * @param objid      The oid to print
 * @param objidlen   The length of oidid.
 */
char *
snmptools_print_objid(const oid * objid, size_t objidlen)
{
    static char     buf[BUFLEN];
    u_char          *buff = (u_char *) buf; /* Just to get rid of some warnings */
    size_t          buf_len = BUFLEN, out_len = 0;
    int             buf_overflow = 0;

    netsnmp_sprint_realloc_objid_tree(&buff, &buf_len, &out_len, 0,
            &buf_overflow, objid, objidlen);
    if (buf_overflow) {
        sprintf(buf, "%s [TRUNCATED]\n", buf);
    } else {
        sprintf(buf, "%s\n", buf);
    }

    return(buf);
}

int
snmptools_snmp_parse_args(int argc, char **argv, netsnmp_session * session, const char *Opts, int (*proc) (int, char *const *, int))
{
    int             arg;

    /*
     * get the options
     */
    DEBUGMSGTL(("snmp_parse_args", "starting: %d/%d\n", optind, argc));
    for (arg = 0; arg < argc; arg++) {
        DEBUGMSGTL(("snmp_parse_args", " arg %d = %s\n", arg, argv[arg]));
    }

    optind = 1;
    while ((arg = getopt(argc, argv, Opts)) != EOF) {
        DEBUGMSGTL(("snmp_parse_args", "handling (#%d): %c\n", optind, arg));
        if(proc(argc, argv, arg))
            return(-1);
    }
    DEBUGMSGTL(("snmp_parse_args", "finished: %d/%d\n", optind, argc));

    return(optind);
}

/**
 * Reimplementation of Net-SNMP's print_description() to print to memory, rather than to
 * a file descriptor.
 * 
 * Prints on oid description to memory.
 *
 * @see fprint_description
 */
void
snmptools_print_description(oid * objid, size_t objidlen, /* number of subidentifiers */
                  int width)
{
    snmptools_fprint_description(objid, objidlen, width);
}

/**
 * Reimplementation of Net-SNMP's fprint_description() to print to memory, rather than to
 * a file descriptor.
 * 
 * Prints on oid description into a file descriptor.
 *
 * @param f         The file descriptor to print to.
 * @param objid     The object identifier.
 * @param objidlen  The object id length.
 * @param width     Number of subidentifiers.
 */
void
snmptools_fprint_description(oid * objid, size_t objidlen, int width)
{
    u_char         *buf = NULL;
    size_t          buf_len = 256, out_len = 0;

    if ((buf = (u_char *) calloc(buf_len, 1)) == NULL) {
        printres("[TRUNCATED]\n");
        return;
    } else {
        if (!sprint_realloc_description(&buf, &buf_len, &out_len, 1,
                                   objid, objidlen, width)) {
            printres("%s [TRUNCATED]\n", buf);
        } else {
            printres("%s\n", buf);
        }
    }

    SNMP_FREE(buf);
}

void
snmptools_snmp_perror(const char *prog_string)
{
    const char     *str;
    int             xerr;
    xerr = snmp_errno;          /*MTCRITICAL_RESOURCE */
    str = snmp_api_errstring(xerr);
    printres("%s: %s\n", prog_string, str);
}

/*
 * snmptools_snmp_sess_perror(): print a error stored in a session pointer
 */
void
snmptools_snmp_sess_log_error(int priority,
                       const char *prog_string, netsnmp_session * ss)
{
    char           *err;
    snmp_error(ss, NULL, NULL, &err);
    //snmp_log(priority, "%s: %s\n", prog_string, err);
    printres("%s: %s\n", prog_string, err);
    SNMP_FREE(err);
}

/*
 * snmptools_snmp_sess_perror(): print a error stored in a session pointer
 */
void
snmptools_snmp_sess_perror(const char *prog_string, netsnmp_session * ss)
{
    snmptools_snmp_sess_log_error(LOG_ERR, prog_string, ss);
}

