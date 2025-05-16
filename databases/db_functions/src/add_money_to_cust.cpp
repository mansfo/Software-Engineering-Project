#include "functions.h"

int add_money_to_cust(Con2DB db, string money, int userid){
    char sqlcmd[100];
    snprintf(sqlcmd,100,"UPDATE Customer SET money = money+'%s' WHERE id = '%d'", money.c_str(), userid);
    PGresult *res;
    res = db.ExecSQLcmd(sqlcmd);
    PQclear(res);
    return 1;
}