#include "functions.h"

int change_address(Con2DB db, string addr, int userid, string table){
    char sqlcmd[500];
    snprintf(sqlcmd, 500, "UPDATE %s SET addr = '%s' WHERE id = '%d'", table.c_str(), addr.c_str(), userid);
    PGresult *res = db.ExecSQLcmd(sqlcmd);
    PQclear(res);
    return 1;
}