#include "functions.h"

pair<int,int> delete_ord(Con2DB db, Con2DB logdb, string ord, int id){
    if(orderstatus_from_id(db, stoi(ord)) != order_status::not_sent || !same_customer(db,stoi(ord),id)) return pair<int,int>(0,0);
    char sqlcmd[100];
    snprintf(sqlcmd, 100, "UPDATE Orders SET status = '%d' WHERE id = '%s' AND cust = '%d' RETURNING suppl", 3, ord.c_str(), id);
    PGresult *res = db.ExecSQLtuples(sqlcmd);
    int suppl = stoi(PQgetvalue(res,0,PQfnumber(res,"suppl")));
    snprintf(sqlcmd,100,"SELECT prod,money,quant FROM Orders WHERE id = '%s'", ord.c_str());
    res = db.ExecSQLtuples(sqlcmd);
    double money = stod(PQgetvalue(res,0,PQfnumber(res,"money")));
    int quant = stoi(PQgetvalue(res,0,PQfnumber(res,"quant")));
    int prod = stoi(PQgetvalue(res,0,PQfnumber(res,"prod")));
    snprintf(sqlcmd, 100, "UPDATE Product SET avail = avail + '%d' WHERE id = '%d'", quant, prod);
    res = db.ExecSQLcmd(sqlcmd);
    snprintf(sqlcmd, 100, "UPDATE Customer SET money = money + '%lf' WHERE id = '%d'", money, id);
    res = db.ExecSQLcmd(sqlcmd);
    log_order_update(logdb, stoi(ord), order_status::deleted);
    PQclear(res);
    return pair<int,int>(stoi(ord),suppl);
}

void deleting_handled(Con2DB db, int ord){
    if(orderstatus_from_id(db,ord) != order_status::deleted) return;
    char sqlcmd[100];
    snprintf(sqlcmd, 100, "UPDATE Orders SET status = '%d' WHERE id = '%d'", 4, ord);
    db.ExecSQLcmd(sqlcmd);
}