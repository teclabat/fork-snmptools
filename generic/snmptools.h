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

#ifndef _SNMPTOOLS_H
#define _SNMPTOOLS_H

#if HAVE_STRING_H
#include <string.h> // strlen
#else
#include <strings.h>
#endif
#include <stdlib.h> // free

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include <tcl.h>

typedef struct {
   netsnmp_session *session;
   netsnmp_session *ss;
} session_data;

char * snmptools_print_objid(const oid * objid, size_t objidlen);
int snmptools_snmp_parse_args(int argc, char **argv, netsnmp_session * session, const char *Opts, int (*proc) (int, char *const *, int));
void snmptools_snmp_sess_log_error(int priority, const char *prog_string, netsnmp_session * ss);
void snmptools_snmp_perror(const char *prog_string);
void snmptools_snmp_sess_perror(const char *prog_string, netsnmp_session * ss);
void snmptools_print_description(oid * objid, size_t objidlen, int width);
void snmptools_fprint_description(oid * objid, size_t objidlen, int width);

int snmpsession(int argc, char *argv[], netsnmp_session **session, netsnmp_session **ss);
int snmpclose(netsnmp_session *session, netsnmp_session *ss);
int snmpget(int argc, char *argv[], netsnmp_session *session, netsnmp_session *ss);
int snmpset(int argc, char *argv[], netsnmp_session *session, netsnmp_session *ss);
int snmpgetwalk(int argc, char *argv[], netsnmp_session *session, netsnmp_session *ss);
int snmpwalk(int argc, char *argv[], netsnmp_session *session, netsnmp_session *ss);
int snmpbulkwalk(int argc, char *argv[], netsnmp_session *session, netsnmp_session *ss);
int snmpbulkget(int argc, char *argv[], netsnmp_session *session, netsnmp_session *ss);
int snmptrap(int argc, char *argv[]);
int snmpgetnext(int argc, char *argv[], netsnmp_session *session, netsnmp_session *ss);
int snmptable(int argc, char *argv[], netsnmp_session *session, netsnmp_session *ss);
int snmptranslate(int argc, char *argv[]);
//TODO
//int encode_keychange(int argc, char *argv[]);
//int snmpusm(int argc, char *argv[], netsnmp_session *session, netsnmp_session *ss);
//int snmpvacm(int argc, char *argv[], netsnmp_session *session, netsnmp_session *ss);

#endif /* _SNMPTOOLS_H */
