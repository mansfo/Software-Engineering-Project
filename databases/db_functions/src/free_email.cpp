#include "functions.h"

int free_email(Con2DB db, string em, string usertype){
    char sqlcmd[100];
    snprintf(sqlcmd,100,"SELECT id FROM %s WHERE email = '%s'", usertype.c_str(), em.c_str());
    PGresult *res = db.ExecSQLtuples(sqlcmd);
    int result = PQntuples(res);
    PQclear(res);
    return result;
}