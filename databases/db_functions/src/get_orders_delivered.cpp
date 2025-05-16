#include "functions.h"

string get_orders_delivered(Con2DB db, string id){
    PGresult *res;
    char sqlcmd[500];
    snprintf(sqlcmd, 500, "SELECT o.id as id,p.name as name FROM Orders o, Product p WHERE o.prod = p.id AND o.carr = '%s' AND o.status = 2 ORDER BY o.id DESC", id.c_str());
    res = db.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(res);
    string result = rows>0? "" : "empty";
    for(int i=0; i<rows; i++){
        if(i != 0) result.append(" ");
        result.append(PQgetvalue(res,i,PQfnumber(res, "id")));
        result.append(" ");
        result.append(PQgetvalue(res,i,PQfnumber(res, "name")));
    }
    PQclear(res);
    return result;
}