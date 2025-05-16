#include "functions.h"

double get_money(Con2DB db, int id){
    char sqlcmd[100];
    snprintf(sqlcmd,100,"SELECT money FROM Customer WHERE id = '%d'", id);
    PGresult *res;
    res = db.ExecSQLtuples(sqlcmd);
    double result = stod(PQgetvalue(res,0,PQfnumber(res,"money")));
    PQclear(res);
    return result;
}