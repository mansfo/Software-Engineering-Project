#include "functions.h"

queue<int> get_deleted_orders(Con2DB db){
    char sqlcmd[100];
    PGresult *res;
    snprintf(sqlcmd,100,"SELECT id FROM Orders WHERE status = 3 ORDER BY id asc");
    res = db.ExecSQLtuples(sqlcmd);
    queue<int> result;
    for(int i=0; i<PQntuples(res); i++){
        result.push(stoi(PQgetvalue(res,i,PQfnumber(res,"id"))));
    }
    PQclear(res);
    return result;
}