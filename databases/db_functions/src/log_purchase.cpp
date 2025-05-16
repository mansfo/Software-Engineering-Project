#include "functions.h"

void log_purchase(Con2DB db, double price, double money, int avail, int quantity, int order){
    char sqlcmd[500];
    PGresult *res;
    snprintf(sqlcmd,500,"SELECT ord FROM PurchasesLogTable WHERE id = (SELECT max(id) FROM PurchasesLogTable)");
    res = db.ExecSQLtuples(sqlcmd);
    if(PQntuples(res)>0 && order == stoi(PQgetvalue(res,0,PQfnumber(res,"ord")))) return;
    snprintf(sqlcmd, 500, "INSERT INTO PurchasesLogTable (price,avail,money,quantity,ord,tstamp,unixtime) VALUES ('%lf','%d','%lf','%d','%d',NOW(),'%ld')", price,avail,money,quantity,order,micro_time());
    res = db.ExecSQLcmd(sqlcmd);
    PQclear(res);
}