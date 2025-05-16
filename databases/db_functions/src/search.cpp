#include "functions.h"

PGresult* search(Con2DB db, string cat){
    char sqlcmd[500];
    snprintf(sqlcmd, 500, "SELECT id,name,avail,price FROM Product WHERE category = '%s' AND avail > 0 AND removed = 0 ORDER BY price ASC", cat.c_str());
    return db.ExecSQLtuples(sqlcmd);
}