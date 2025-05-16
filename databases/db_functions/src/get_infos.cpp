#include "functions.h"

PGresult* get_infos(Con2DB db, string ord){
    char sqlcmd[500];
    snprintf(sqlcmd, 500, "SELECT status FROM Orders WHERE id = '%s'", ord.c_str());
    return db.ExecSQLtuples(sqlcmd);
}