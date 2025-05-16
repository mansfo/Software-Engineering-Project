#include "functions.h"

PGresult *login_carrier(Con2DB db, string id){
    char sqlcmd[500];
    snprintf(sqlcmd, 500, "SELECT name,surname,company,status FROM Carrier WHERE id = '%s'", id.c_str());
    return db.ExecSQLtuples(sqlcmd);
}