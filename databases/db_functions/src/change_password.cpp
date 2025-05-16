#include "functions.h"

int change_password(Con2DB db, string old, string new_psw, int userid, string table){
    char sqlcmd[100];
    snprintf(sqlcmd,100,"SELECT password FROM %s WHERE id = '%d'", table.c_str(), userid);
    PGresult *res;
    res = db.ExecSQLtuples(sqlcmd);
    if(strcmp(PQgetvalue(res,0,PQfnumber(res,"password")),old.c_str())) return 0;
    snprintf(sqlcmd,100,"UPDATE %s SET password = '%s' WHERE id = '%d'", table.c_str(), new_psw.c_str(), userid);
    res = db.ExecSQLcmd(sqlcmd);
    PQclear(res);
    return 1;
}