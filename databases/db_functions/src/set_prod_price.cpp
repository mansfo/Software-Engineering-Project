#include "functions.h"

int set_prod_price(Con2DB db, string prod, string price, string suppl){
    char sqlcmd[500];
    if(stod(price) <= 0) return 0;
    snprintf(sqlcmd, 500, "SELECT supplier FROM Product WHERE id = '%s'", prod.c_str());
    PGresult *res = db.ExecSQLtuples(sqlcmd);
    if(strcmp(suppl.c_str(),PQgetvalue(res,0,PQfnumber(res,"supplier")))) return 0;
    snprintf(sqlcmd, 500, "UPDATE Product SET price = '%s' WHERE id = '%s'", price.c_str(), prod.c_str());
    db.ExecSQLcmd(sqlcmd);
    PQclear(res);
    return 1;
}