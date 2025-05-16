#include "functions.h"

void add_streams_to_db(Con2DB db, int id, string readstr, string writestr, int table){
    char sqlcmd[300];
    snprintf(sqlcmd, 300, "UPDATE %s SET readstream = '%s' WHERE id = '%d'", table==0? "Customer":(table==1?"Supplier":"Carrier"),readstr.c_str(),id);
    PGresult *res = db.ExecSQLcmd(sqlcmd);
    snprintf(sqlcmd, 300, "UPDATE %s SET writestream = '%s' WHERE id = '%d'", table==0? "Customer":(table==1?"Supplier":"Carrier"),writestr.c_str(),id);
    res = db.ExecSQLcmd(sqlcmd);
    PQclear(res);
}