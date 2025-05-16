#include "functions.h"

int get_supplier(Con2DB db, int product){
    char sqlcmd[500];
    snprintf(sqlcmd, 500, "SELECT supplier FROM Product WHERE id = '%d'", product);
    PGresult *res = db.ExecSQLtuples(sqlcmd);
    int id = atoi(PQgetvalue(res,0,PQfnumber(res, "supplier")));
    PQclear(res);
    return id;
}