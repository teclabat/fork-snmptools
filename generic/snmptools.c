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

/*
 * snmptools.c
 */

#include <tcl.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "snmptools.h"
#include "util.h"

/*
 * Local (static) Function Declarations
 */

static int sessionCmd_CleanUp(ClientData clientData);

static int snmpCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

static int sessionCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]);

/*
 * Function Bodies
 */

int Snmptools_Init(Tcl_Interp *interp) {
    if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
        return TCL_ERROR;
    }
    Tcl_DStringInit(&Result);

    Tcl_CreateObjCommand(interp, "snmp", snmpCmd, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

    Tcl_PkgProvide(interp, "snmptools", "1.0");
    return TCL_OK;
}

static int snmpCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {
    static char* cmds[] = { "session", "translate", "keychange", "ucm", "vacm", "trap", "inform", NULL };
    char *tempfile;
    int ret, index, i;

    if (objc < 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "option argument ?arg? ...");
        return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objv[1], cmds, "option", 0, &index) != TCL_OK)
        return TCL_ERROR;

    int argc = objc - 1;
    char *argv[objc - 1];
    for(i = 0; i < objc - 1; i++)
        argv[i] = Tcl_GetString(objv[i + 1]);

    tempfile = captureOutput();
    if(tempfile == NULL) {
        restoreOutput(tempfile);
        Tcl_SetResult(interp, "Internal Error: Failed to capture output", TCL_STATIC);
        return TCL_ERROR;
    }

    INITRESULT

    switch(index) {
        case 0: /* session */
            {
                if (objc < 3) {
                    restoreOutput(tempfile);
                    Tcl_WrongNumArgs(interp, 2, objv, "CommandName ?arg? ...");
                    return TCL_ERROR;
                }

                netsnmp_session *session, *ss;
                session_data *sdata;

                ret = snmpsession(argc - 1, argv + 1, &session, &ss);
                getOutput(tempfile);
                Tcl_SetResult(interp, Tcl_DStringValue(&Result), TCL_VOLATILE);
                if(ret) return TCL_ERROR;

                sdata = (session_data *) Tcl_Alloc(sizeof(session_data));
                sdata->session = session;
                sdata->ss = ss;

                Tcl_CreateObjCommand(interp, Tcl_GetString(objv[2]), sessionCmd, (ClientData) sdata, (Tcl_CmdDeleteProc *) sessionCmd_CleanUp);
            }
            break;

        case 1: /* translate */
            {
                ret = snmptranslate(argc, argv);
                getOutput(tempfile);
                Tcl_SetResult(interp, Tcl_DStringValue(&Result), TCL_VOLATILE);
                if(ret) return TCL_ERROR;
            }
            break;

        case 2: /* keychange */
            {
                Tcl_SetResult(interp, "Not Implemented.", TCL_STATIC);
                return TCL_ERROR;
            }
            break;

        case 3: /* ucm */
            {
                Tcl_SetResult(interp, "Not Implemented.", TCL_STATIC);
                return TCL_ERROR;
            }
            break;

        case 4: /* vacm */
            {
                Tcl_SetResult(interp, "Not Implemented.", TCL_STATIC);
                return TCL_ERROR;
            }
            break;

        case 5: /* trap */

                /* Fall through */

        case 6: /* inform */
            {
                ret = snmptrap(argc, argv);
                getOutput(tempfile);
                Tcl_SetResult(interp, Tcl_DStringValue(&Result), TCL_VOLATILE);
                if(ret) return TCL_ERROR;
            }
            break;

    }

    return TCL_OK;
}

static int sessionCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {
    static char* cmds[] = { "close", "get", "set", "getnext", "bulkget", "walk", "bulkwalk", "table", NULL };
    session_data *sdata;
    sdata = (session_data *) clientData;
    netsnmp_session *session, *ss;
    session = sdata->session; ss = sdata->ss;
    int ret, index, i;

    if (objc < 2) {
        Tcl_WrongNumArgs(interp, 1, objv, "option argument ?arg? ...");
        return TCL_ERROR;
    }

    if (Tcl_GetIndexFromObj(interp, objv[1], cmds, "option", 0, &index) != TCL_OK)
        return TCL_ERROR;

    char *argv[objc - 1];
    int argc = objc - 1;
    for(i = 0; i < objc - 1; i++)
        argv[i] = Tcl_GetString(objv[i + 1]);

    INITRESULT

    switch(index) {
        case 0: /* close */
            {
                if (objc != 2) {
                    Tcl_WrongNumArgs(interp, 2, objv, NULL);
                    return TCL_ERROR;
                }
                /* snmpclose() will be called by sessionCmd_CleanUp() */
                ret = Tcl_DeleteCommand(interp, Tcl_GetString(objv[0]));
                if(ret) return TCL_ERROR;
            }
            break;

        case 1: /* get */
            {
                ret = snmpget(argc, argv, session, ss);
                Tcl_SetResult(interp, Tcl_DStringValue(&Result), TCL_VOLATILE);
                if(ret) return TCL_ERROR;
            }
            break;

        case 2: /* set */
            {
                ret = snmpset(argc, argv, session, ss);
                Tcl_SetResult(interp, Tcl_DStringValue(&Result), TCL_VOLATILE);
                if(ret) return TCL_ERROR;
            }
            break;

        case 3: /* getnext */
            {
                ret = snmpgetnext(argc, argv, session, ss);
                Tcl_SetResult(interp, Tcl_DStringValue(&Result), TCL_VOLATILE);
                if(ret) return TCL_ERROR;
            }
            break;

        case 4: /* bulkget */
            {
                ret = snmpbulkget(argc, argv, session, ss);
                Tcl_SetResult(interp, Tcl_DStringValue(&Result), TCL_VOLATILE);
                if(ret) return TCL_ERROR;
            }
            break;

        case 5: /* walk */
            {
                ret = snmpwalk(argc, argv, session, ss);
                Tcl_SetResult(interp, Tcl_DStringValue(&Result), TCL_VOLATILE);
                if(ret) return TCL_ERROR;
            }
            break;

        case 6: /* bulkwalk */
            {
                ret = snmpbulkwalk(argc, argv, session, ss);
                Tcl_SetResult(interp, Tcl_DStringValue(&Result), TCL_VOLATILE);
                if(ret) return TCL_ERROR;
            }
            break;

        case 7: /* table */
            {
                ret = snmptable(argc, argv, session, ss);
                Tcl_SetResult(interp, Tcl_DStringValue(&Result), TCL_VOLATILE);
                if(ret) return TCL_ERROR;
            }
            break;

    }

    return TCL_OK;
}

static int sessionCmd_CleanUp(ClientData clientData) {
    int ret;

    session_data *sdata;
    sdata = (session_data *) clientData;
    
    ret = snmpclose(sdata->session, sdata->ss);
    if(ret) return TCL_ERROR;

    Tcl_Free((char *) sdata);

    return TCL_OK;
}
