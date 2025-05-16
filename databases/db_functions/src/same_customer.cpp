#include "functions.h"

int same_customer(Con2DB db, int ord, int cid){
    char sqlstm[500];
    snprintf(sqlstm, 500, "SELECT cust FROM Orders WHERE id = '%d'", ord);
    PGresult *res = db.ExecSQLtuples(sqlstm);
    int b = cid == atoi(PQgetvalue(res,0,PQfnumber(res,"cust")));
    PQclear(res);
    return b;
}