#include "functions.h"

pair<int,string> pick_up_order(Con2DB db, Con2DB logdb, string id){
    char sqlcmd[500];
    PGresult *res;
    snprintf(sqlcmd,500, "UPDATE Orders SET status = 1 WHERE id = '%s'", id.c_str());
    res = db.ExecSQLcmd(sqlcmd);
    snprintf(sqlcmd,500,"UPDATE Carrier SET status = 2 WHERE id = (SELECT carr FROM Orders WHERE id = '%s')", id.c_str());
    res = db.ExecSQLcmd(sqlcmd);
    PQclear(res);
    log_order_update(logdb, stoi(id), order_status::sent);
    return pair<int,string>(stoi(id),"sent");
}