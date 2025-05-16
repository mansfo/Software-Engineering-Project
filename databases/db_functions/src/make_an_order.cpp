#include "functions.h"

int make_an_order(Con2DB db, Con2DB logdb, string prod, string cust, string quant){
    if(stoi(quant)==0) return 0;
    char sqlcmd[500];
    snprintf(sqlcmd, 500, "SELECT * FROM Product WHERE id = '%s'", prod.c_str());
    PGresult *res = db.ExecSQLtuples(sqlcmd);
    double price = stod(PQgetvalue(res,0,PQfnumber(res, "price")));
    int avail = stoi(PQgetvalue(res,0,PQfnumber(res,"avail")));
    string suppl = PQgetvalue(res,0,PQfnumber(res, "supplier"));
    snprintf(sqlcmd,100,"SELECT money FROM Customer WHERE id = '%s'", cust.c_str());
    res = db.ExecSQLtuples(sqlcmd);
    double mon = stod(PQgetvalue(res,0,PQfnumber(res,"money")));
    snprintf(sqlcmd, 500, "INSERT INTO Orders (prod,cust,suppl,carr,status,money,quant) VALUES('%s','%s','%s',null,'%d','%lf','%d') RETURNING id", prod.c_str(), cust.c_str(), suppl.c_str(),0,price*stoi(quant),stoi(quant));
    res = db.ExecSQLtuples(sqlcmd);
    int id = atoi(PQgetvalue(res,0,PQfnumber(res,"id")));
    log_purchase(logdb,price,mon,avail,stoi(quant),id);
    if(avail < stoi(quant) || price*stoi(quant) > mon) {
        snprintf(sqlcmd,100,"UPDATE Orders SET status = '%d' WHERE id = '%d'", 4, id);
        res = db.ExecSQLcmd(sqlcmd);
        return 0;
    }
    snprintf(sqlcmd,100,"UPDATE Customer SET money = '%lf' WHERE id = '%s'", mon - price*stoi(quant), cust.c_str());
    res = db.ExecSQLcmd(sqlcmd);
    snprintf(sqlcmd,100,"UPDATE Product SET avail = '%d' WHERE id = '%s'", avail-stoi(quant), prod.c_str());
    res = db.ExecSQLcmd(sqlcmd);
    PQclear(res);
    return id;
}