#include "functions.h"

void log_order_update(Con2DB logdb, int order, int status){
    char sqlcmd[100];
    PGresult *res;
    snprintf(sqlcmd,500,"SELECT ord,status FROM OrderLogTable WHERE id = (SELECT max(id) FROM OrderLogTable)");
    res = logdb.ExecSQLtuples(sqlcmd);
    if(PQntuples(res)>0 && order == stoi(PQgetvalue(res,0,PQfnumber(res,"ord"))) && status == stoi(PQgetvalue(res,0,PQfnumber(res,"status")))) return;
    snprintf(sqlcmd,100,"INSERT INTO OrderLogTable (ord,status,tstamp,unixtime) VALUES ('%d','%d',NOW(),'%ld')", order, status, micro_time());
    res = logdb.ExecSQLcmd(sqlcmd);
    PQclear(res);
}