#include "functions.h"

queue<int> get_pending_orders(Con2DB db){
    char sqlcmd[100];
    PGresult *res;
    snprintf(sqlcmd,100,"SELECT id FROM Orders WHERE status = 0 AND carr IS NULL ORDER BY id asc");
    res = db.ExecSQLtuples(sqlcmd);
    queue<int> result;
    for(int i=0; i<PQntuples(res); i++){
        result.push(stoi(PQgetvalue(res,i,PQfnumber(res,"id"))));
    }
    PQclear(res);
    return result;
}

unordered_map<int,int> get_assigned_pending_orders(Con2DB db){
    char sqlcmd[100];
    unordered_map<int,int> result;
    snprintf(sqlcmd, 100, "SELECT id,carr FROM Orders WHERE status = 0 AND carr IS NOT NULL");
    PGresult* res = db.ExecSQLtuples(sqlcmd);
    for(int i=0; i<PQntuples(res); i++){
        result.insert({stoi(PQgetvalue(res,i,PQfnumber(res,"id"))),stoi(PQgetvalue(res,i,PQfnumber(res,"carr")))});
    }
    PQclear(res);
    return result;
}

pair<int,int> get_assigned_order(Con2DB db, int carrier){
    char sqlcmd[100];
    snprintf(sqlcmd,100,"SELECT id FROM Orders WHERE carr = '%d' AND status = 0", carrier);
    PGresult* res = db.ExecSQLtuples(sqlcmd);
    if(PQntuples(res)<1) {
        snprintf(sqlcmd,100,"SELECT status FROM Carrier WHERE id = '%d'", carrier);
        res = db.ExecSQLtuples(sqlcmd);
        if(stoi(PQgetvalue(res,0,PQfnumber(res,"status"))) == 1){
            snprintf(sqlcmd,100,"UPDATE Carrier SET status = 0 WHERE id = '%d'", carrier);
            res = db.ExecSQLcmd(sqlcmd);
            PQclear(res);
            return pair{-1,0};
        }
        PQclear(res);
        return pair{0,0};
    }
    int ord = stoi(PQgetvalue(res,0,PQfnumber(res,"id")));
    int dist = get_suppl_distance(db, ord);
    PQclear(res);
    return pair{ord,dist};
}