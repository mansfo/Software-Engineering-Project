#include "functions.h"

int supplier_from_order(Con2DB db, int ord){
    char sqlcmd[500];
    snprintf(sqlcmd, 500, "SELECT suppl FROM Orders WHERE id = '%d'", ord);
    PGresult *res = db.ExecSQLtuples(sqlcmd);
    int suppl = atoi(PQgetvalue(res,0,PQfnumber(res,"suppl")));
    PQclear(res);
    return suppl;
}