/*
 * snmpset.c - send snmp SET requests to a network entity.
 *
 */
/***********************************************************************
	Copyright 1988, 1989, 1991, 1992 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/
#include <net-snmp/net-snmp-config.h>

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include <sys/types.h>
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#include <stdio.h>
#include <ctype.h>
#if TIME_WITH_SYS_TIME
# ifdef WIN32
#  include <sys/timeb.h>
# else
#  include <sys/time.h>
# endif
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#if HAVE_WINSOCK_H
#include <winsock.h>
#endif
#if HAVE_NETDB_H
#include <netdb.h>
#endif
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#include <net-snmp/net-snmp-includes.h>

#include "snmptools.h"
#include "util.h"

#define BUFLEN 500

static void
usage(void)
{
    printres("USAGE: set ");
    printres(" OID TYPE VALUE [OID TYPE VALUE]...\n\n");
    printres("  -h\t\tThis help message\n");
    printres("  -C APPOPTS\t\tSet various application specific behaviours:\n");
    printres("\t\t\t  q:  don't print results on success\n");
    printres("\n  TYPE: one of i, u, t, a, o, s, x, d, b, n\n");
    printres("\ti: INTEGER, u: unsigned INTEGER, t: TIMETICKS, a: IPADDRESS\n");
    printres("\to: OBJID, s: STRING, x: HEX STRING, d: DECIMAL STRING, b: BITS\n");
#ifdef NETSNMP_WITH_OPAQUE_SPECIAL_TYPES
    printres("\tU: unsigned int64, I: signed int64, F: float, D: double\n");
#endif                          /* NETSNMP_WITH_OPAQUE_SPECIAL_TYPES */
}

static int quiet = 0;

static int
optProc(int argc, char *const *argv, int opt)
{
    switch (opt) {
    case 'h':
        usage();
        break;
    case 'C':
        while (*optarg) {
            switch (*optarg++) {
            case 'q':
                quiet = 1;
                break;

            default:
                printres("Unknown flag passed to -C: %c\n",
                        optarg[-1]);
                return(1);
            }
        }
    }
    return(0);
}

int
snmpset(int argc, char *argv[], netsnmp_session *session, netsnmp_session *ss)
{
    netsnmp_pdu    *pdu, *response = NULL;
    netsnmp_variable_list *vars;
    int             arg;
    int             count;
    int             current_name = 0;
    int             current_type = 0;
    int             current_value = 0;
    char           *names[SNMP_MAX_CMDLINE_OIDS];
    char            types[SNMP_MAX_CMDLINE_OIDS];
    char           *values[SNMP_MAX_CMDLINE_OIDS];
    oid             name[MAX_OID_LEN];
    size_t          name_length;
    int             status;
    int             failures = 0;
    int             exitval = 0;

    putenv(strdup("POSIXLY_CORRECT=1"));

    /*
     * get command line arguments 
     */
    arg = snmptools_snmp_parse_args(argc, argv, session, "hC:", optProc);
    if (arg < 0)
        return(arg * -1);

    if (arg >= argc) {
        printres("Missing object name\n");
        return(1);
    }
    if ((argc - arg) > 3*SNMP_MAX_CMDLINE_OIDS) {
        printres("Too many assignments specified. ");
        printres("Only %d allowed in one request.\n", SNMP_MAX_CMDLINE_OIDS);
        return(1);
    }

    /*
     * get object names, types, and values 
     */
    for (; arg < argc; arg++) {
        DEBUGMSGTL(("snmp_parse_args", "handling (#%d): %s %s %s\n",
                    arg,argv[arg], arg+1 < argc ? argv[arg+1] : NULL,
                    arg+2 < argc ? argv[arg+2] : NULL));
        names[current_name++] = argv[arg++];
        if (arg < argc) {
            switch (*argv[arg]) {
            case '=':
            case 'i':
            case 'u':
            case 't':
            case 'a':
            case 'o':
            case 's':
            case 'x':
            case 'd':
            case 'b':
#ifdef NETSNMP_WITH_OPAQUE_SPECIAL_TYPES
            case 'I':
            case 'U':
            case 'F':
            case 'D':
#endif                          /* NETSNMP_WITH_OPAQUE_SPECIAL_TYPES */
                types[current_type++] = *argv[arg++];
                break;
            default:
                printres("%s: Bad object type: %c\n", argv[arg - 1],
                        *argv[arg]);
                return(1);
            }
        } else {
            printres("%s: Needs type and value\n", argv[arg - 1]);
            return(1);
        }
        if (arg < argc)
            values[current_value++] = argv[arg];
        else {
            printres("%s: Needs value\n", argv[arg - 2]);
            return(1);
        }
    }

    /*
     * create PDU for SET request and add object names and values to request 
     */
    pdu = snmp_pdu_create(SNMP_MSG_SET);
    for (count = 0; count < current_name; count++) {
        name_length = MAX_OID_LEN;
        if (snmp_parse_oid(names[count], name, &name_length) == NULL) {
            snmptools_snmp_perror(names[count]);
            failures++;
        } else
            if (snmp_add_var
                (pdu, name, name_length, types[count], values[count])) {
            snmptools_snmp_perror(names[count]);
            failures++;
        }
    }

    if (failures) {
        return(1);
    }

    /*
     * do the request 
     */
    status = snmp_synch_response(ss, pdu, &response);
    if (status == STAT_SUCCESS) {
        if (response->errstat == SNMP_ERR_NOERROR) {
            if (!quiet) {
                char buf[BUFLEN];
                for (vars = response->variables; vars;
                     vars = vars->next_variable)
                    snprint_variable(buf, BUFLEN, vars->name, vars->name_length, vars);
                    printres("%s", buf);
            }
        } else {
            printres("Error in packet.\nReason: %s\n",
                    snmp_errstring(response->errstat));
            if (response->errindex != 0) {
                printres("Failed object: ");
                for (count = 1, vars = response->variables;
                     vars && (count != response->errindex);
                     vars = vars->next_variable, count++);
                if (vars)
                    printres(snmptools_print_objid(vars->name, vars->name_length));
                printres("\n");
            }
            exitval = 2;
        }
    } else if (status == STAT_TIMEOUT) {
        printres("Timeout: No Response from %s\n",
                session->peername);
        exitval = 1;
    } else {                    /* status == STAT_ERROR */
        snmptools_snmp_sess_perror("set", ss);
        exitval = 1;
    }

    if (response)
        snmp_free_pdu(response);
    return exitval;
}
