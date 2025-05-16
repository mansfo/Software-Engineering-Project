#include "functions.h"

int cust_from_order(Con2DB db, int ord){
    char sqlcmd[500];
    snprintf(sqlcmd, 500, "SELECT cust FROM Orders WHERE id = '%d'", ord);
    PGresult *res = db.ExecSQLtuples(sqlcmd);
    int customer = atoi(PQgetvalue(res,0,PQfnumber(res,"cust")));
    PQclear(res);
    return customer;
}