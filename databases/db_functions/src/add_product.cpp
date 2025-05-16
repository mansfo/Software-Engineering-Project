#include "functions.h"

int add_product(Con2DB db, string name, string price, string avail, string cat, string id){
    int prod = 0;
    char sqlcmd[500];
    if(stoi(avail) < 0 || stod(price) < 0) return 0;
    snprintf(sqlcmd, 500, "SELECT id FROM Product WHERE name = '%s' AND supplier = '%s'", name.c_str(), id.c_str());
    PGresult *res = db.ExecSQLtuples(sqlcmd);
    int k = PQntuples(res);
    if(k==0){
        sprintf(sqlcmd, "INSERT INTO Product (name,price,avail,supplier,category,removed) VALUES ('%s','%s','%s','%s','%s',0) RETURNING id", name.c_str(), price.c_str(), avail.c_str(), id.c_str(), cat.c_str());
        res = db.ExecSQLtuples(sqlcmd);
        prod = atoi(PQgetvalue(res,0,PQfnumber(res, "id")));
        PQclear(res);
    } else {
        prod = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
        PQclear(res);
        sprintf(sqlcmd, "UPDATE Product SET price = '%s',avail = '%s' WHERE supplier = '%s' AND name = '%s'", price.c_str(), avail.c_str(), id.c_str(), name.c_str());
        res = db.ExecSQLcmd(sqlcmd);
        PQclear(res);
    }
    return prod;
}

int add_product(Con2DB db, string prod_id, string avail, string suppl){
    char sqlcmd[500];
    if(stoi(avail) < 0) return 0;
    snprintf(sqlcmd, 500, "SELECT supplier FROM Product WHERE id = '%s'", prod_id.c_str());
    PGresult *res = db.ExecSQLtuples(sqlcmd);
    if(strcmp(suppl.c_str(),PQgetvalue(res,0,PQfnumber(res,"supplier")))) return 0;
    snprintf(sqlcmd, 500, "UPDATE Product SET avail = '%s' WHERE id = '%s'", avail.c_str(), prod_id.c_str());
    db.ExecSQLcmd(sqlcmd);
    PQclear(res);
    return 1;
}